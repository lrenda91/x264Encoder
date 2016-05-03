#ifndef H264ENCODER_H
#define H264ENCODER_H

#include <stdint.h>
#include <inttypes.h>
#include "x264/x264.h"


void init_encoder(const int width, const int height);

int doEncode(
    const int width,
    const int height,
    const unsigned char *yuv,
    unsigned char *outBuffer,
    const int flag);

void release_encoder();

void getConfigBytes(char **sps, size_t *sps_size,
                    char **pps, size_t *pps_size);

#endif //H264ENCODER_H
