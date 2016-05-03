LOCAL_PATH := $(call my-dir)
LIBAV_PATH := $(LOCAL_PATH)/libav-11.6/
include $(CLEAR_VARS)

##########################################################
# building static prebuilt libraries
##########################################################
LOCAL_MODULE    := x264-prebuilt
LOCAL_SRC_FILES := x264/libx264.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE    := libavfilter-prebuilt
LOCAL_SRC_FILES := libav-11.6/libavfilter/libavfilter.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE    := libswscale-prebuilt
LOCAL_SRC_FILES := libav-11.6/libswscale/libswscale.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE    := libavresample-prebuilt
LOCAL_SRC_FILES := libav-11.6/libavresample/libavresample.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE    := libavdevice-prebuilt
LOCAL_SRC_FILES := libav-11.6/libavdevice/libavdevice.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE    := libavcodec-prebuilt
LOCAL_SRC_FILES := libav-11.6/libavcodec/libavcodec.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE    := libavutil-prebuilt
LOCAL_SRC_FILES := libav-11.6/libavutil/libavutil.a
include $(PREBUILT_STATIC_LIBRARY)

LOCAL_MODULE    := libavformat-prebuilt
LOCAL_SRC_FILES := libav-11.6/libavformat/libavformat.a
include $(PREBUILT_STATIC_LIBRARY)

##########################################################
# building application library
##########################################################
include $(CLEAR_VARS)
LOCAL_MODULE := MediaEncoder
LOCAL_CPP_EXTENSION := .cc .cpp
#add this folder to header files paths
LOCAL_CFLAGS += -I$(LIBAV_PATH)
#LOCAL_CFLAGS += -std=c++11
#LOCAL_C_INCLUDES :=  $(LOCAL_PATH)
#LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true

LOCAL_CPPFLAGS := -O2 -Werror -Wall -fexceptions
#LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/libav-11.6/

## -llog links android logs library in helper.h
LOCAL_LDLIBS +=	-llog -lz -landroid
LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel
#libav-11.6/libavutil/libavutil.a libav-11.6/libavcodec/libavcodec.a libav-11.6/libavresample/libavresample.a

LOCAL_SRC_FILES += jni_main.cpp			## C++ JNI entrypoint
#LOCAL_SRC_FILES += h264_encoder.c		## encoder functions implementation
#LOCAL_SRC_FILES += h264_decoder.c		## decoder functions implementation
#LOCAL_SRC_FILES += prova.cpp
LOCAL_SRC_FILES += ENC_CPP/enc.cpp

#LOCAL_SHARED_LIBRARIES := libcutils\
                          libgnustl\
                          libdl

LOCAL_STATIC_LIBRARIES := 	x264-prebuilt \
 							libavcodec-prebuilt \
							libavutil-prebuilt \
							libavfilter-prebuilt \
							libavformat-prebuilt \
							libavdevice-prebuilt \
							libswscale-prebuilt \
							libavresample-prebuilt
#libav-11.6/libavutil/libavutil.a libav-11.6/libavcodec/libavcodec.a libav-11.6/libavresample/libavresample.a

include $(BUILD_SHARED_LIBRARY)
