#ifndef H264_DECODER_H
#define H264_DECODER_H

#define H264_INBUF_SIZE 16384

#include <stdio.h>
#include <stdbool.h>
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"


/* the decoder callback, which will be called when we have decoded a frame */
typedef void(*h264_decoder_callback)(AVFrame* frame, AVPacket* pkt);


bool init_decoder(h264_decoder_callback frameCallback, float fps);
void submit_buffer(uint8_t *data, size_t data_size);
//bool update(bool *needsMoreBytes);                                                     /* internally used to update/parse the data we read from the buffer or file */
//int readBuffer();                                                                      /* read a bit more data from the buffer */
void decodeFrame(uint8_t* data, int size);
void release_decoder();

#endif
