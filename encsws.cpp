#include "encsws.h"

X264Encoder::X264Encoder()
  :in_width(0)
  ,in_height(0)
  ,out_width(0)
  ,out_height(0)
  ,fps(25)
  ,in_pixel_format(AV_PIX_FMT_NONE)
  ,out_pixel_format(AV_PIX_FMT_NONE)
  ,encoder(NULL)
  ,num_nals(0)
  ,pts(0)
  ,sws(NULL)
{
  memset((char*)&pic_raw, 0, sizeof(pic_raw));
}

X264Encoder::~X264Encoder() {
  if(sws) {
    close();
  }
}

bool X264Encoder::open() {

  if(!validateSettings()) {
    return false;
  }

  int r = 0;
  int nheader = 0;
  //int header_size = 0;

  // @todo add validate which checks if all params are set (in/out width/height, fps,etc..);
  if(encoder) {
    LOGE("Already opened. first call close()");
    return false;
  }

  if(out_pixel_format != AV_PIX_FMT_YUV420P) {
    LOGE("At this moment the output format must be AV_PIX_FMT_YUV420P");
    return false;
  }

  sws = sws_getContext(in_width, in_height, in_pixel_format,
                       out_width, out_height, out_pixel_format,
                       SWS_FAST_BILINEAR, NULL, NULL, NULL);

  if(!sws) {
    LOGE("Cannot create SWS context");
    ::exit(EXIT_FAILURE);
  }

  x264_picture_alloc(&pic_in, X264_CSP_I420, out_width, out_height);

  setParams();

  // create the encoder using our params
  encoder = x264_encoder_open(&params);
  if(!encoder) {
    LOGE("Cannot open the encoder");
    goto error;
  }

  // write headers
  r = x264_encoder_headers(encoder, &nals, &nheader);
  if(r < 0) {
    LOGE("x264_encoder_headers() failed");
    goto error;
  }

  pts = 0;

  return true;

 error:
  close();
  return false;
}

bool X264Encoder::close() {
  if(encoder) {
    x264_picture_clean(&pic_in);
    memset((char*)&pic_in, 0, sizeof(pic_in));
    memset((char*)&pic_out, 0, sizeof(pic_out));

    x264_encoder_close(encoder);
    encoder = NULL;
  }

  if(sws) {
    sws_freeContext(sws);
    sws = NULL;
  }

  memset((char*)&pic_raw, 0, sizeof(pic_raw));

  return true;
}
/*
bool X264Encoder::encode(char* pixels) {
  if(!sws) {
    LOGE("Not initialized, so cannot encode");
    return false;
  }

  // copy the pixels into our "raw input" container.
  int bytes_filled = avpicture_fill(&pic_raw, (uint8_t*)pixels, in_pixel_format, in_width, in_height);
  if(!bytes_filled) {
    LOGE("Cannot fill the raw input buffer");
    return false;
  }

  // convert to I420 for x264
  int h = sws_scale(sws, pic_raw.data, pic_raw.linesize, 0,
                    in_height, pic_in.img.plane, pic_in.img.i_stride);

  if(h != out_height) {
    LOGE("scale failed: %d", h);
    return false;
  }

  // and encode and store into pic_out
  pic_in.i_pts = pts;

  int frame_size = x264_encoder_encode(encoder, &nals, &num_nals, &pic_in, &pic_out);
  printf("%d", frame_size);
  ++pts;

  return true;
}

void X264Encoder::setParams() {
  x264_param_default_preset(&params, "ultrafast", "zerolatency");
  params.i_threads = 1;
  params.i_width = out_width;
  params.i_height = out_height;
  params.i_fps_num = fps;
  params.i_fps_den = 1;
}

bool X264Encoder::validateSettings() {
  if(!in_width) {
    LOGE("No in_width set");
    return false;
  }
  if(!in_height) {
    LOGE("No in_height set");
    return false;
  }
  if(!out_width) {
    LOGE("No out_width set");
    return false;
  }
  if(!out_height) {
    LOGE("No out_height set");
    return false;
  }
  if(in_pixel_format == AV_PIX_FMT_NONE) {
    LOGE("No in_pixel_format set");
    return false;
  }
  if(out_pixel_format == AV_PIX_FMT_NONE) {
    LOGE("No out_pixel_format set");
    return false;
  }
  return true;
}
*/
