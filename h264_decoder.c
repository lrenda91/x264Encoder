#include <stdlib.h>

#include "tinylib.h"
#include "h264_decoder.h"

#define MAX_LEN 1024*1024

AVCodec* codec;                                                                        /* the AVCodec* which represents the H264 decoder */
AVCodecContext* codec_context;                                                         /* the context; keeps generic state */
AVCodecParserContext* parser;                                                          /* parser that is used to decode the h264 bitstream */
AVFrame* picture;                                                                      /* will contain a decoded picture */

int frame;                                                                             /* the number of decoded frames */
h264_decoder_callback cb_frame;                                                        /* the callback function which will receive the frame/packet data */
uint64_t frame_timeout;                                                                /* timeout when we need to parse a new frame */
uint64_t frame_delay;                                                                  /* delay between frames (in ns) */


uint8_t buffer[MAX_LEN];
uint8_t *cursor = buffer;
size_t filled_bytes = 0; /* first filled bytes */

bool fill_buffer(uint8_t *data, size_t size){
    int remaining = MAX_LEN - filled_bytes;
    if (remaining < size){
        return false;
    }
    memcpy(cursor, data, size);
    filled_bytes += size;
    return true;
}

//erase first 'size' bytes from buffer:
void empty_buffer(size_t size){
    memcpy(buffer, cursor, filled_bytes-size);
    cursor = buffer;
    filled_bytes -= size;
}

bool init_decoder(h264_decoder_callback frameCallback, float fps){
    codec = NULL;
    codec_context = NULL;
    parser = NULL;
    //fp = NULL;
    frame = 0;
    cb_frame = frameCallback;
    frame_timeout = 0;
    frame_delay = 0;
    avcodec_register_all();

    codec = avcodec_find_decoder(AV_CODEC_ID_H264);
    if(!codec) {
      printf("Error: cannot find the h264 codec\n");
      return false;
    }

    codec_context = avcodec_alloc_context3(codec);

    if(codec->capabilities & CODEC_CAP_TRUNCATED) {
      codec_context->flags |= CODEC_FLAG_TRUNCATED;
    }

    if(avcodec_open2(codec_context, codec, NULL) < 0) {
      printf("Error: could not open codec.\n");
      return false;
    }

    /*
    fp = fopen(filepath, "rb");

    if(!fp) {
      printf("Error: cannot open: %s\n", filepath);
      return false;
    }
    */

    picture = av_frame_alloc();
    parser = av_parser_init(AV_CODEC_ID_H264);

    if(!parser) {
      printf("Erorr: cannot create H264 parser.\n");
      return false;
    }

    if(fps > 0.0001f) {
      frame_delay = (1.0f/fps) * 1000ull * 1000ull * 1000ull;
      //frame_timeout = rx_hrtime() + frame_delay;
    }

    return true;
}

void submit_buffer(uint8_t *data, size_t data_size){

    uint64_t now = rx_hrtime();
    if(now < frame_timeout) {
        return;
    }

    if (!fill_buffer(data, data_size)){
        return;
    }
    uint8_t* parsed_data = NULL;   //parsed data
    int parsed_size = 0;   //size of parsed data or 0 if not finishedd:

    //the int return value is how many bytes have been used inside 'buffer' to perform parsing
    //it should always be <= filled_bytes
    int len = av_parser_parse2(parser, codec_context,
                                &parsed_data, &parsed_size,
                                cursor, filled_bytes,
                                0, 0, AV_NOPTS_VALUE);



    if (!parsed_size) {  //we need more bytes
        return;
    }

    cursor += len;
    decodeFrame(parsed_data, parsed_size);
    empty_buffer(len);

    // it may take some 'reads' before we can set the fps
    if(frame_timeout == 0 && frame_delay == 0) {
        double fps = av_q2d(codec_context->time_base);
        if(fps > 0.0) {
            frame_delay = fps * 1000ull * 1000ull * 1000ull;
        }
    }

    if (frame_delay > 0) {
        //frame_timeout = rx_hrtime() + frame_delay;
    }

}


void decodeFrame(uint8_t* data, int size){
    AVPacket pkt;
    int got_picture = 0;
    int len = 0;

    av_init_packet(&pkt);

    pkt.data = data;
    pkt.size = size;

    len = avcodec_decode_video2(codec_context, picture, &got_picture, &pkt);
    if(len < 0) {
      printf("Error while decoding a frame.\n");
    }

    if(got_picture == 0) {
      return;
    }

    ++frame;

    if(cb_frame) {
      cb_frame(picture, &pkt);
    }
}
void release_decoder(){
  if(parser) {
    av_parser_close(parser);
    parser = NULL;
  }

  if(codec_context) {
    avcodec_close(codec_context);
    av_free(codec_context);
    codec_context = NULL;
  }

  if(picture) {
    av_free(picture);
    picture = NULL;
  }

  cb_frame = NULL;
  frame = 0;
  frame_timeout = 0;
}
