/**
 * Optimize Edilmiş RTSP Akış Programı (C++ versiyonu)
 * dev.nacibaran@gmail.com
 */

#include "RTSPServer.h"
#include "Options.h"
#include <iostream>
#include <gst/gst.h>

int main(int argc, char *argv[]) {
    // Argümanları ayrıştır
    Options options = parse_arguments(argc, argv);
    
    // Gstreamer başlat
    gst_init(&argc, &argv);
    
    // RTSP sunucusu oluştur
    RTSPServer server(options);
    
    // Sunucuyu başlat
    if (!server.start()) {
        std::cerr << "Sunucu başlatılamadı!" << std::endl;
        return -1;
    }
    
    return 0;
}
