#ifndef _HELPER_H_
#define _HELPER_H_

#if defined(ANDROID)
#include <jni.h>
#include <android/log.h>

#define  LOG_TAG    "JNI"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#else      // endof ANDROID

#define LOGD(...)
#define LOGE(...)

#endif

#endif
