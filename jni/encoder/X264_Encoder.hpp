#ifndef H264ENCODER_H
#define H264ENCODER_H

extern "C" {
    #include <stdint.h>
    #include <inttypes.h>
    #include "x264/x264.h"
}

class X264_Encoder {

    public:
      X264_Encoder();
      ~X264_Encoder();

     private:
         x264_t *x264_hdl_;
         x264_param_t x264_opt;
         x264_picture_t picture_in, picture_out;
         void init();
         void release();

     public:
         bool applyParams(const int width, const int height, const int bitrateKbps);
         int encode(uint8_t *yuv, const int width, const int height, const int bitrate, uint8_t *output);
         void getConfigBytes(uint8_t **sps, size_t *sps_size,
                             uint8_t **pps, size_t *pps_size);
};

#endif
