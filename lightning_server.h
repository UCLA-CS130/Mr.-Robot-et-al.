#ifndef LIGHTNING_SERVER_H
#define LIGHTNING_SERVER_H

#include "config_parser.h"

class LightningServer {
public:
    LightningServer(const char* file_name);
    void start();

private:
    NginxConfig config_;
    NginxConfigParser config_parser_;
};

#endif