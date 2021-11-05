#include <jni.h>
#include "ALog.h"
#include "FFmpegPlayer.h"

#define LOG_TAG "jni_player"

#define CLASS_FFMPEGPLAYER      "com/niklaus/player/FFmpegPlayer"

jlong create(JNIEnv *env, jobject thiz);
jstring getInfo(JNIEnv *env, jobject thiz,jlong instance);
void setDataSource(JNIEnv *env, jobject thiz,jlong instance,jstring path);
void prepare(JNIEnv *env, jobject thiz,jlong instance);
void start(JNIEnv *env, jobject thiz,jlong instance);

int getVideoWidth(JNIEnv *env, jobject thiz,jlong instance);
int getVideoHeight(JNIEnv *env, jobject thiz,jlong instance);
int getVideoRotation(JNIEnv *env, jobject thiz,jlong instance);
long getVideoDuration(JNIEnv *env, jobject thiz,jlong instance);

static JNINativeMethod playerMethods[] = {
        {"native_create", "()J", (void *) create},
        {"native_setDataSource", "(JLjava/lang/String;)V", (void *) setDataSource},
        {"native_prepare", "(J)V", (void *) prepare},
        {"native_start", "(J)V", (void *) start},
        {"native_getVideoWidth", "(J)I", (void *) getVideoWidth},
        {"native_getVideoHeight", "(J)I", (void *) getVideoHeight},
        {"native_getVideoRotation", "(J)I", (void *) getVideoRotation},
        {"native_getVideoDuration", "(J)J", (void *) getVideoDuration},
        {"native_getInfo", "(J)Ljava/lang/String;", (void *) getInfo},
};

JNIEXPORT
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    jint loadRet = JNI_ERR;
    JNIEnv *env = nullptr;
    loadRet = vm->GetEnv((void **) (&env), JNI_VERSION_1_6);
    LOGI(LOG_TAG, "JNI_OnLoad ret = %d",loadRet);
    if (loadRet!= JNI_OK) {
        return loadRet;
    }

    int registerRet;
    jclass playerClazz = env->FindClass(CLASS_FFMPEGPLAYER);
    int nMethods = sizeof(playerMethods) / sizeof(playerMethods[0]);
    registerRet = env->RegisterNatives(playerClazz,playerMethods,nMethods);
    LOGI(LOG_TAG, "nMethods size = %d, RegisterNatives ret = %d",nMethods,registerRet);

    return JNI_VERSION_1_6;
}

JNIEXPORT
jlong create(JNIEnv *env, jobject thiz){
    auto* ffmpegPlayer = new FFmpegPlayer();
    return reinterpret_cast<jlong>(ffmpegPlayer);
}

JNIEXPORT
void setDataSource(JNIEnv *env, jobject thiz, jlong instance, jstring path){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    const char* pathChar = env->GetStringUTFChars(path,JNI_FALSE);
    ffmpegPlayer->setDataSource(pathChar);
}

JNIEXPORT
void prepare(JNIEnv *env, jobject thiz, jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    ffmpegPlayer->prepare();
}

JNIEXPORT
void start(JNIEnv *env, jobject thiz, jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    ffmpegPlayer->start();
}

JNIEXPORT
int getVideoWidth(JNIEnv *env, jobject thiz, jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    return ffmpegPlayer->getVideoWidth();
}

JNIEXPORT
int getVideoHeight(JNIEnv *env, jobject thiz, jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    return ffmpegPlayer->getVideoHeight();
}

JNIEXPORT
long getVideoDuration(JNIEnv *env, jobject thiz, jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    return ffmpegPlayer->getVideoDuration();
}

JNIEXPORT
int getVideoRotation(JNIEnv *env, jobject thiz, jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    return ffmpegPlayer->getVideoRotation();
}

JNIEXPORT
jstring getInfo(JNIEnv *env, jobject thiz,jlong instance){
    auto* ffmpegPlayer = reinterpret_cast<FFmpegPlayer *>(instance);
    return env->NewStringUTF(ffmpegPlayer->getInfo().c_str());
}

