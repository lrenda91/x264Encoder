#include <jni.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include "helper.h"
#include "encoder/X264_Encoder.hpp"

#include "coffeecatch/coffeejni.h"

X264_Encoder *encoder = NULL;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT jstring Java_it_polito_mec_video_raven_sender_encoding_StreamSenderJNI_hello(JNIEnv* env, jobject javaThis){
    jstring result;
    const char msg[60] = "HELLO from JNI";
    result = env->NewStringUTF(msg);
    return result;
}


JNIEXPORT void Java_it_polito_mec_video_raven_sender_encoding_StreamSenderJNI_nativeInitEncoder(JNIEnv* env, jobject thiz){
    encoder = new X264_Encoder();
}

JNIEXPORT void Java_it_polito_mec_video_raven_sender_encoding_StreamSenderJNI_nativeReleaseEncoder(JNIEnv* env, jobject thiz){
    delete encoder;
}

JNIEXPORT jboolean Java_it_polito_mec_video_raven_sender_encoding_StreamSenderJNI_nativeApplyParams(JNIEnv* env, jobject thiz,
        const jint width, const jint height, const jint bitrateKbps){
    return (jboolean) encoder->applyParams(width, height, bitrateKbps);
}

JNIEXPORT jbyteArray Java_it_polito_mec_video_raven_sender_encoding_StreamSenderJNI_nativeDoEncode(JNIEnv* env, jobject thiz,
        const jint width, const jint height, jbyteArray _yuv, const jint bitrate){
    //jboolean isCopy = JNI_TRUE;
    jbyte *yuvData = env->GetByteArrayElements(_yuv, NULL);
    jsize yuvSize = env->GetArrayLength(_yuv);
    //LOGD("yuv size: %d -> (%dx%d) %d kbps", yuvSize, width, height, bitrate);
    //jbyte *nalsData = env->GetByteArrayElements(_outBuffer, &isCopy);
    //LOGD("array size: %d expected: %d", (int)yuvSize, (int)(width*height*1.5));
    uint8_t *native_yuvData = (uint8_t*)yuvData;
    uint8_t *native_nalsData = (uint8_t *)calloc(yuvSize, sizeof(uint8_t));
    if (!native_nalsData || !native_yuvData){
        LOGE("CAN'T ALLOC!!");
        return NULL;
    }
    //uint8_t native_nalsData[yuvSize];
    //uint8_t *native_nalsData = (uint8_t*)nalsData;
    int ret = 0;
    COFFEE_TRY() {
        ret = encoder->encode(native_yuvData, width, height, bitrate, native_nalsData);
    } COFFEE_CATCH() {
        const char*const message = coffeecatch_get_message();
        jclass cls = env->FindClass("java/lang/RuntimeException");
        env->ThrowNew(cls, strdup(message));
        return NULL;
    } COFFEE_END();
    //int ret = encoder->encode(native_yuvData, width, height, bitrate, native_nalsData);
    //env->ReleaseByteArrayElements(_outBuffer, nalsData, 0);
    env->ReleaseByteArrayElements(_yuv, yuvData, JNI_ABORT);

    jbyteArray result = env->NewByteArray(ret);
    env->SetByteArrayRegion(result, 0, ret, (jbyte *)native_nalsData);

    free(native_nalsData);
    return result;
}

JNIEXPORT jobjectArray Java_it_polito_mec_video_raven_sender_encoding_StreamSenderJNI_nativeGetHeaders(JNIEnv* env, jobject thiz){
    uint8_t *sps = NULL, *pps = NULL;
    size_t sps_size = 0, pps_size = 0;
    encoder->getConfigBytes(&sps, &sps_size, &pps, &pps_size);

    jbyteArray jni_SPS = env->NewByteArray(sps_size);
    env->SetByteArrayRegion(jni_SPS, 0, sps_size, (jbyte *)sps);

    jbyteArray jni_PPS = env->NewByteArray(pps_size);
    env->SetByteArrayRegion(jni_PPS, 0, pps_size, (jbyte *)pps);


    jobjectArray result = env->NewObjectArray(2, env->GetObjectClass(jni_SPS), NULL);

    env->SetObjectArrayElement(result, 0, jni_SPS);
    env->SetObjectArrayElement(result, 1, jni_PPS);
    return result;
}

#ifdef __cplusplus
}
#endif
