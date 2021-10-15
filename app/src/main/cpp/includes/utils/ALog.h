//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_ALOG_H
#define FFMPEGDEMO_ALOG_H

#include<android/log.h>
#include <sys/time.h>

#define  LOGE(LOG_TAG,...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGV(LOG_TAG,...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define  LOGD(LOG_TAG,...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(LOG_TAG,...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)


static long long GetSysCurrentTime(){
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
    return curTime;
}

#endif //FFMPEGDEMO_ALOG_H
