extern "C" {
    #include <string.h>
    #include <stdio.h>
    #include "helper.h"
}
#include "X264_Encoder.hpp"

X264_Encoder::X264_Encoder()
:x264_hdl_(NULL){
    init();
    LOGD("x264 encoder created!");
}

X264_Encoder::~X264_Encoder(){
    release();
    LOGD("x264 encoder released!");
}

void X264_Encoder::getConfigBytes(uint8_t **sps, size_t *sps_size, uint8_t **pps, size_t *pps_size){
    int nals;
    x264_nal_t *nal_pointer;
    x264_encoder_headers(x264_hdl_, &nal_pointer, &nals);
    int idx;
    //char c[500];
    //c[0] = '\0';
    for (idx=0;idx<nals-1;idx++){
        //unsigned int h; for (h=0;h<strlen(c);h++){ c[h] = 0; }
        x264_nal_t nalu = nal_pointer[idx];
        if (nalu.i_type == 7){  //SPS
            *sps = nalu.p_payload;      //data
            *sps_size = nalu.i_payload; //size
        }
        if (nalu.i_type == 8){  //PPS
            *pps = nalu.p_payload;      //data
            *pps_size = nalu.i_payload; //size
        }
        /*strcat(c, "[ ");
        int j;
        for (j=0;j<nalu.i_payload;j++){
            char ss[10] = {0};
            sprintf(ss, "%d ", nalu.p_payload[j]);
            strcat(c, ss);
        }
        strcat(c, "]");*/
        //LOGD("NALU #%d: %s\n", idx, c);
    }
}

int X264_Encoder::encode(uint8_t *yuv, const int width, const int height, const int bitrate, uint8_t *output){

    if (applyParams(width, height, bitrate) == true){
        LOGD("Params changed!!!!!");
    }

    memcpy(picture_in.img.plane[0], yuv, width*height);
    memcpy(picture_in.img.plane[1], yuv + width*height - 1, width*height/2);
    picture_in.i_type = X264_TYPE_IDR;

    int nals;
    x264_nal_t *nal_pointer;
    int ret = x264_encoder_encode(x264_hdl_, &nal_pointer, &nals, &picture_in, &picture_out);
    if ( ret <= 0) {
        return ret;
    }

    int outLength = 0;
    int i;
    for (i = 0; i < nals; i++) {
        if( nal_pointer[i].i_type != 6) {
            x264_nal_t* nal = &nal_pointer[i];
            memcpy(&output[outLength], nal->p_payload, nal->i_payload);
            outLength += nal->i_payload;
        }
   }
   return outLength;
}

bool X264_Encoder::applyParams(const int width, const int height, const int bitrateKbps){
    //check for any parameters update
    int currentWidth = x264_opt.i_width;
    int currentHeight = x264_opt.i_height;
    int currentBitrate = x264_opt.rc.i_bitrate;
    if ( currentWidth == width
        && currentHeight == height
        && currentBitrate == bitrateKbps ){
        return false;
    }

    // 0. Release data structures
    release();

    // 1. Re-init data structures
    // set OUTPUT parameters
    x264_opt.i_width = width;
    x264_opt.i_height = height;
    // set RATE CONTROL parameters
    x264_opt.rc.i_bitrate = bitrateKbps;

    int a = x264_picture_alloc(&picture_in,  X264_CSP_YV12, x264_opt.i_width, x264_opt.i_height);
    //int b = x264_picture_alloc(&picture_out, X264_CSP_YV12, x264_opt.i_width, x264_opt.i_height);
    if (a != 0 || (&picture_in) == NULL){
        LOGE("ERROR ALLOC PICTURES");
        return false;
    }

    // 2. Building the encoder handler
    x264_hdl_ = x264_encoder_open(&x264_opt);
    x264_encoder_parameters(x264_hdl_, &x264_opt);

    LOGD("Re-init x264 encoder: (%dx%d) %d Kbps",width, height, bitrateKbps);
    return true;
}

void X264_Encoder::init(){
    // 0. building default encoder parameters.
    x264_param_default_preset(&x264_opt, "ultrafast", "zerolatency");

    x264_opt.i_level_idc = 41;         //recommended: level_idc 4.1
    x264_opt.i_sps_id = 0;
    x264_opt.i_threads = 2;     //con i_threads=1 non funziona sopra 640x480!!

    /*
    rc (RATE CONTROL) parameters:
    Use bitrate instead of cq (Constant Quantizer) or crf (Constant RateFactor)
    */
    x264_opt.rc.i_rc_method = X264_RC_ABR;

    x264_opt.b_aud = 0;
    x264_opt.b_annexb = 1;
    x264_opt.b_repeat_headers = 0;     //DO NOT repeat SPS+PPS per each encoded frame
    x264_opt.b_intra_refresh = 0;

    x264_param_apply_profile(&x264_opt, "baseline");
}

void X264_Encoder::release(){
    if (x264_hdl_) {
        //void *p = (void *)&picture_in;  LOGD("&IN: %p",p);
        //void *prev = *( ( ( void **) p ) - 1 ); LOGD("&prevIN: %p",prev);
        //if (p){ free(p); LOGD("free IN");}
        //if (prev) {free(prev); LOGD("free prevIN");}
        //p = (void *)&picture_out;     LOGD("&OUT: %p",p);
        //prev = *( ( ( void **) p ) - 1 ); LOGD("&prevOUT: %p",prev);
        //if (p){ free(p); LOGD("free OUT");}
        //if (prev){ free(prev); LOGD("free prevOUT");}
        /*if (&picture_in){
            LOGD("freeing IN: %p", &picture_in);
            //x264_picture_clean(&picture_in);
            free(picture_in.img.plane[0]);
            memset( &picture_in, 0, sizeof( x264_picture_t ) );
            //free(&picture_in);
            LOGD("freed IN");
        }*/
        x264_picture_clean(&picture_in);
        memset( &picture_in, 0, sizeof( picture_in ) );
        memset( &picture_out, 0, sizeof( picture_out ) );
        x264_encoder_close(x264_hdl_);
        x264_hdl_ = NULL;
    }
}
