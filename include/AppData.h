#ifndef APP_DATA_H
#define APP_DATA_H

#include <opencv2/opencv.hpp>
#include <gst/gst.h>

/**
 * Video akışını yönetmek için kullanılan veri yapısı
 */
typedef struct {
    cv::VideoCapture *cap;       // OpenCV kamera nesnesi
    int fps;                     // Kare hızı
    guint64 number_frames;       // İşlenen kare sayısı
    GstClockTime duration;       // Kare süresi
    int width;                   // Görüntü genişliği
    int height;                  // Görüntü yüksekliği
} AppData;

#endif // APP_DATA_H
