#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H

#include <gst/gst.h>
#include <gst/rtsp-server/rtsp-server.h>
#include <string>
#include "AppData.h"
#include "Options.h"

/**
 * RTSP sunucu sınıfı
 */
class RTSPServer {
public:
    /**
     * Yapılandırıcı
     * 
     * @param options Sunucu seçenekleri
     */
    RTSPServer(const Options& options);
    
    /**
     * Yıkıcı
     */
    ~RTSPServer();
    
    /**
     * Sunucuyu başlatır
     * 
     * @return Başlatma başarılı ise true
     */
    bool start();
    
    /**
     * Sunucuyu durdurur
     */
    void stop();
    
private:
    Options m_options;           // Sunucu seçenekleri
    GMainLoop* m_loop;           // Ana olay döngüsü
    GstRTSPServer* m_server;     // RTSP sunucu nesnesi
    AppData* m_appData;          // Uygulama verileri
    
    /**
     * Need data sinyali için callback
     */
    static void need_data_callback(GstElement* appsrc, guint size, gpointer user_data);
    
    /**
     * Medya yapılandırma callback'i
     */
    static void media_configure_callback(GstRTSPMediaFactory* factory, GstRTSPMedia* media, gpointer user_data);
};

#endif // RTSP_SERVER_H
