#include "Options.h"
#include <getopt.h>
#include <cstring>
#include <iostream>

Options parse_arguments(int argc, char *argv[]) {
    Options opt;
    
    static struct option long_options[] = {
        {"device_id", required_argument, 0, 'd'},
        {"fps", required_argument, 0, 'f'},
        {"image_width", required_argument, 0, 'w'},
        {"image_height", required_argument, 0, 'h'},
        {"port", required_argument, 0, 'p'},
        {"stream_uri", required_argument, 0, 'u'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "d:f:w:h:p:u:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'd':
                opt.device_id = atoi(optarg);
                break;
            case 'f':
                opt.fps = atoi(optarg);
                break;
            case 'w':
                opt.image_width = atoi(optarg);
                break;
            case 'h':
                opt.image_height = atoi(optarg);
                break;
            case 'p':
                opt.port = atoi(optarg);
                break;
            case 'u':
                opt.stream_uri = optarg;
                break;
            default:
                std::cerr << "Bilinmeyen parametre: " << static_cast<char>(c) << std::endl;
                break;
        }
    }
    
    return opt;
}
