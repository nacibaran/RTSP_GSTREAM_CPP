#include "RTSPServer.h"
#include <gst/app/gstappsrc.h>
#include <iostream>
#include <cstring>

RTSPServer::RTSPServer(const Options& options) 
    : m_options(options), m_loop(nullptr), m_server(nullptr), m_appData(nullptr) {
}

RTSPServer::~RTSPServer() {
    stop();
}

bool RTSPServer::start() {
    // RTSP sunucusu oluştur
    m_loop = g_main_loop_new(NULL, FALSE);
    m_server = gst_rtsp_server_new();
    
    // Port ayarla
    char port_str[16];
    sprintf(port_str, "%d", m_options.port);
    gst_rtsp_server_set_service(m_server, port_str);
    
    // Kamera açma
    cv::VideoCapture *cap = new cv::VideoCapture(m_options.device_id);
    cap->set(cv::CAP_PROP_FRAME_WIDTH, m_options.image_width);
    cap->set(cv::CAP_PROP_FRAME_HEIGHT, m_options.image_height);
    cap->set(cv::CAP_PROP_FPS, m_options.fps);
    
    if (!cap->isOpened()) {
        std::cerr << "Kamera açılamadı!" << std::endl;
        stop();
        return false;
    }
    
    // AppData yapısını doldur
    m_appData = new AppData;
    m_appData->cap = cap;
    m_appData->fps = m_options.fps;
    m_appData->duration = GST_SECOND / m_options.fps;
    m_appData->number_frames = 0;
    m_appData->width = m_options.image_width;
    m_appData->height = m_options.image_height;
    
    // Pipeline oluştur
    GstRTSPMediaFactory *factory = gst_rtsp_media_factory_new();
    
    // Ultra düşük gecikmeli RTSP pipeline
    std::string launch_str = "appsrc name=source ! videoconvert ! video/x-raw,format=I420 ! "
                           "x264enc speed-preset=ultrafast tune=zerolatency key-int-max=15 "
                           "bitrate=1000 byte-stream=true threads=4 aud=true ! "
                           "h264parse ! rtph264pay config-interval=1 name=pay0 pt=96";
    
    gst_rtsp_media_factory_set_launch(factory, launch_str.c_str());
    gst_rtsp_media_factory_set_shared(factory, TRUE);
    
    // Medya konfigürasyon callback'ini ayarla
    g_signal_connect(factory, "media-configure", G_CALLBACK(media_configure_callback), m_appData);
    
    // RTSP mount points oluştur
    GstRTSPMountPoints *mounts = gst_rtsp_server_get_mount_points(m_server);
    gst_rtsp_mount_points_add_factory(mounts, m_options.stream_uri.c_str(), factory);
    g_object_unref(mounts);
    
    // Sunucuyu başlat
    gst_rtsp_server_attach(m_server, NULL);
    
    std::cout << "RTSP Sunucusu başlatıldı: rtsp://IP_ADRESINIZ:" << m_options.port 
              << m_options.stream_uri << std::endl;
    
    // Ana döngüyü çalıştır
    g_main_loop_run(m_loop);
    
    return true;
}

void RTSPServer::stop() {
    if (m_loop) {
        g_main_loop_quit(m_loop);
        g_main_loop_unref(m_loop);
        m_loop = nullptr;
    }
    
    if (m_appData) {
        if (m_appData->cap) {
            delete m_appData->cap;
            m_appData->cap = nullptr;
        }
        delete m_appData;
        m_appData = nullptr;
    }
}

void RTSPServer::need_data_callback(GstElement *appsrc, guint size, gpointer user_data) {
    AppData *data = static_cast<AppData*>(user_data);
    cv::Mat frame;
    
    if (data->cap->isOpened()) {
        if (data->cap->read(frame)) {
            // Frame verisini buffer'a çevir
            size_t data_size = frame.total() * frame.elemSize();
            GstBuffer *buffer = gst_buffer_new_allocate(NULL, data_size, NULL);
            
            GstMapInfo map;
            if (gst_buffer_map(buffer, &map, GST_MAP_WRITE)) {
                // Buffer'a frame verisini kopyala
                memcpy(map.data, frame.data, data_size);
                gst_buffer_unmap(buffer, &map);
                
                // Buffer zaman damgası
                buffer->duration = data->duration;
                buffer->pts = buffer->dts = data->number_frames * data->duration;
                buffer->offset = data->number_frames;
                data->number_frames++;
                
                // Buffer'ı gönder
                GstFlowReturn ret = gst_app_src_push_buffer(GST_APP_SRC(appsrc), buffer);
                if (ret != GST_FLOW_OK) {
                    std::cerr << "Push buffer hatası: " << ret << std::endl;
                }
            } else {
                gst_buffer_unref(buffer);
            }
        }
    }
}

void RTSPServer::media_configure_callback(GstRTSPMediaFactory *factory, GstRTSPMedia *media, gpointer user_data) {
    AppData *data = static_cast<AppData*>(user_data);
    data->number_frames = 0;
    
    GstElement *element = gst_rtsp_media_get_element(media);
    GstElement *appsrc = gst_bin_get_by_name_recurse_up(GST_BIN(element), "source");
    
    // AppSrc özelliklerini ayarla
    g_object_set(G_OBJECT(appsrc), "format", GST_FORMAT_TIME, NULL);
    g_object_set(G_OBJECT(appsrc), "is-live", TRUE, NULL);
    g_object_set(G_OBJECT(appsrc), "block", FALSE, NULL);
    
    // AppSrc caps (yetenekler) oluştur
    GstCaps *caps = gst_caps_new_simple("video/x-raw",
                                        "format", G_TYPE_STRING, "BGR",
                                        "width", G_TYPE_INT, data->width,
                                        "height", G_TYPE_INT, data->height,
                                        "framerate", GST_TYPE_FRACTION, data->fps, 1,
                                        NULL);
    gst_app_src_set_caps(GST_APP_SRC(appsrc), caps);
    gst_caps_unref(caps);
    
    // need-data sinyalini bağla
    g_signal_connect(appsrc, "need-data", G_CALLBACK(need_data_callback), data);
    
    gst_object_unref(appsrc);
    gst_object_unref(element);
}
