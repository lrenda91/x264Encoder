#include <string.h>
#include "helper.h"
#include "h264encoder.h"

H264Encoder::H264Encoder(int width, int height) {
    x264_hdl_ = NULL;
    init(width,height);
    LOGD("H264 Encoder created!");
}

H264Encoder::~H264Encoder() {
    release();
}

void H264Encoder::init(const int wid, const int hei) {
    // 0. building encoder parameters.
    x264_param_default_preset(&x264_opt_, "ultrafast", "zerolatency");

    x264_opt_.i_width = wid;
    x264_opt_.i_height = hei;
    x264_opt_.i_threads = 1;
    x264_opt_.b_repeat_headers = 0;
    x264_opt_.b_intra_refresh = 0;

    x264_opt_.rc.i_rc_method = X264_RC_CQP;
    x264_opt_.rc.i_qp_constant = 24;
    x264_opt_.rc.i_qp_min = 24;
    x264_opt_.rc.i_qp_max = 24;

    /* Added by me */
    x264_opt_.rc.i_bitrate = 500000;
    x264_opt_.b_aud = 0;
    x264_opt_.b_annexb = 1;
    /* Added by me */

    //x264_param_default(&opt);
    x264_param_apply_profile(&x264_opt_, "baseline");

    // 1. Prepare the output buffer and target file
    x264_picture_alloc(&x264_picin_,  X264_CSP_NV12, x264_opt_.i_width, x264_opt_.i_height);
    x264_picture_alloc(&x264_picout_, X264_CSP_NV12, x264_opt_.i_width, x264_opt_.i_height);

    // 2. Building the encoder handler
    x264_hdl_ = x264_encoder_open(&x264_opt_);
    x264_encoder_parameters(x264_hdl_, &x264_opt_);
    LOGD("init(%dx%d)",wid, hei);
}

void H264Encoder::release(){
    if (x264_hdl_ == NULL) {
        x264_picture_clean(&x264_picin_);
        x264_picture_clean(&x264_picout_);
        x264_encoder_close(x264_hdl_);
        x264_hdl_ = NULL;
    }
    LOGD("H264 Encoder released!");
}

int H264Encoder::doEncode(const int width, const int height, const unsigned char* yuv,
                            unsigned char* outBuffer, const int flag) {
    int cur_width = x264_opt_.i_width;
    int cur_height = x264_opt_.i_height;

    if (width != cur_width || height != cur_height){
        release();
        //re-init all
        init(width, height);
    }

    memcpy(x264_picin_.img.plane[0], yuv, width*height);
    memcpy(x264_picin_.img.plane[1], yuv + width*height - 1, width*height/2);

    if ( flag == 1) {
        x264_picin_.i_type = X264_TYPE_IDR;
    } else {
        x264_picin_.i_type = X264_TYPE_P;
    }

    int nals;
    x264_nal_t *nal_pointer;
    int ret = x264_encoder_encode(x264_hdl_, &nal_pointer, &nals, &x264_picin_, &x264_picout_);
    //LOGD("NALUs: %d", nals);
    if ( ret <= 0) {
        return ret;
    }

    int outLength = 0;
    for ( int i = 0; i < nals; i++) {
        if( nal_pointer[i].i_type != 6) {
            x264_nal_t* nal = &nal_pointer[i];
            memcpy(&outBuffer[outLength], nal->p_payload, nal->i_payload);
            outLength += nal->i_payload;
        }
   }

   return outLength;
}
