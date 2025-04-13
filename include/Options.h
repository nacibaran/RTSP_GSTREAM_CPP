#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

/**
 * Komut satırı argümanları için yapı
 */
struct Options {
    int device_id = 0;                        // Kamera cihaz ID'si
    int fps = 30;                             // Kare hızı
    int image_width = 1920;                   // Görüntü genişliği
    int image_height = 1080;                  // Görüntü yüksekliği
    int port = 8554;                          // RTSP port numarası
    std::string stream_uri = "/zed-stream";   // RTSP akış yolu
};

/**
 * Komut satırı argümanlarını ayrıştırır
 * 
 * @param argc Argüman sayısı
 * @param argv Argüman dizisi
 * @return Options ayarlanmış seçenekler yapısı
 */
Options parse_arguments(int argc, char *argv[]);

#endif // OPTIONS_H
