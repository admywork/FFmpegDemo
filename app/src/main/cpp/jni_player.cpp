#include <jni.h>
#include "ALog.h"
#include "FFmpegPlayer.h"

#define LOG_TAG "jni_player"

#define CLASS_FFMPEGPLAYER      "com/niklaus/player/FFmpegPlayer"

jstring getInfo(JNIEnv *env, jobject thiz);
jlong create(JNIEnv *env, jobject thiz);

static JNINativeMethod playerMethods[] = {
        {"native_create",  "()J", (void *) getInfo},
        {"native_getInfo",  "()Ljava/lang/String;", (void *) create},
};

JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint ret = JNI_ERR;
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return ret;
    }

    jclass playerClazz = env->FindClass(CLASS_FFMPEGPLAYER);

    ret = env->RegisterNatives(playerClazz,playerMethods,sizeof(playerMethods) /
                                                   sizeof(playerMethods[0]));
    LOGI(LOG_TAG, "JNI_OnLoad RegisterNatives ret = %d",ret);
}

JNIEXPORT
jstring getInfo(JNIEnv *env, jobject thiz,jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    return env->NewStringUTF(ffmpegPlayer->getInfo().c_str());
}

JNIEXPORT
jlong create(JNIEnv *env, jobject thiz){
    auto* ffmpegPlayer = new FFmpegPlayer();
    return reinterpret_cast<jlong>(ffmpegPlayer);
}

