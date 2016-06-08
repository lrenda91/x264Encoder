LOCAL_PATH := $(call my-dir)
LIBAV_PATH := $(LOCAL_PATH)/libav-11.6/
include $(CLEAR_VARS)

##########################################################
# building static prebuilt libraries
##########################################################
LOCAL_MODULE    := x264-prebuilt
LOCAL_SRC_FILES := x264/libx264.a
include $(PREBUILT_STATIC_LIBRARY)

##########################################################
# building application library
##########################################################
include $(CLEAR_VARS)
LOCAL_MODULE := MediaEncoder
LOCAL_CPP_EXTENSION := .cc .cpp
#add this folder to header files paths
#LOCAL_CFLAGS += -I$(LIBAV_PATH)
LOCAL_CFLAGS += -funwind-tables
#LOCAL_CFLAGS += -std=c++11
#LOCAL_C_INCLUDES :=  $(LOCAL_PATH)
#LOCAL_DISABLE_FATAL_LINKER_WARNINGS := true

LOCAL_CPPFLAGS := -O2 -Werror -Wall -fexceptions
#LOCAL_CPPFLAGS += -I$(LOCAL_PATH)/libav-11.6/

## -llog links android logs library in helper.h
LOCAL_LDLIBS +=	-llog -lz -landroid
#LOCAL_LDFLAGS := -Wl,--no-warn-shared-textrel
#libav-11.6/libavutil/libavutil.a libav-11.6/libavcodec/libavcodec.a libav-11.6/libavresample/libavresample.a

LOCAL_SRC_FILES += jni_main.cpp			## C++ JNI entrypoint
LOCAL_SRC_FILES += encoder/X264_Encoder.cpp
LOCAL_SRC_FILES += coffeecatch/provaucontext.c
LOCAL_SRC_FILES += coffeecatch/coffeejni.c

#LOCAL_SHARED_LIBRARIES := libcutils\
                          libgnustl\
                          libdl

LOCAL_STATIC_LIBRARIES := 	x264-prebuilt \
 							#libavcodec-prebuilt \
							libavutil-prebuilt \
							libavfilter-prebuilt \
							libavformat-prebuilt \
							libavdevice-prebuilt \
							libswscale-prebuilt \
							libavresample-prebuilt#

include $(BUILD_SHARED_LIBRARY)
