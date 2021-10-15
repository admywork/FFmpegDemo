#include <jni.h>
#include "ALog.h"

#define LOG_TAG "jni_player"

#define CLASS_FFMPEGPLAYER      "com/niklaus/player/FFmpegPlayer"

static JNINativeMethod playerMethods[] = {
        {"native_GetInfo",  "()Ljava/lang/String;", (void *) getInfo},
};

JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    LOGI(LOG_TAG, "JNI_OnLoad");
    jint ret = JNI_ERR;
    JNIEnv *env = nullptr;
    if (vm->GetEnv((void **) (&env), JNI_VERSION_1_6) != JNI_OK) {
        return ret;
    }


    jclass playerClazz = env->FindClass(CLASS_FFMPEGPLAYER);

    env->RegisterNatives(playerClazz,playerMethods,sizeof(playerMethods) /
                                                   sizeof(playerMethods[0]));
}

JNIEXPORT
jstring getInfo(){
    return "";
}

