cmake_minimum_required(VERSION 3.10)
project(rtsp_server)

# C++11 standardını kullan
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include dizinlerini ekle
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Kaynak dosyalarını bul
file(GLOB SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# Derleme hedefini oluştur
add_executable(rtsp_server ${SOURCES})

# OpenCV bağımlılığını bul ve ekle
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
target_link_libraries(rtsp_server ${OpenCV_LIBS})

# GStreamer bağımlılıklarını bul ve ekle
find_package(PkgConfig REQUIRED)
pkg_check_modules(GST REQUIRED gstreamer-1.0 gstreamer-rtsp-server-1.0 gstreamer-app-1.0)
include_directories(${GST_INCLUDE_DIRS})
link_directories(${GST_LIBRARY_DIRS})
add_definitions(${GST_CFLAGS_OTHER})
target_link_libraries(rtsp_server ${GST_LIBRARIES})

# Kurulum hedefi
install(TARGETS rtsp_server DESTINATION bin)
