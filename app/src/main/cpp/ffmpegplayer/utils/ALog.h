//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_ALOG_H
#define FFMPEGDEMO_ALOG_H

#include<android/log.h>
#include <sys/time.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/log.h"
#ifdef __cplusplus
}
#endif

#define FFMPEG_LOG_TAG "FFMPEG_LOG_TAG"

#define  LOGE(LOG_TAG, ...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define  LOGV(LOG_TAG, ...)  __android_log_print(ANDROID_LOG_VERBOSE,LOG_TAG,__VA_ARGS__)
#define  LOGD(LOG_TAG, ...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGI(LOG_TAG, ...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGW(LOG_TAG, ...)  __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)

static void log_callback(void *ptr, int level, const char *fmt, va_list vl) {
    if (level > av_log_get_level()) {
        return;
    }

    char line[512] = {0};
    vsnprintf(line, sizeof(line) - 1, fmt, vl);
    if (level == AV_LOG_QUIET || level == AV_LOG_PANIC ||
        level == AV_LOG_FATAL || level == AV_LOG_ERROR) {
        LOGE(FFMPEG_LOG_TAG, "%s", line);
    } else if (level == AV_LOG_WARNING) {
        LOGW(FFMPEG_LOG_TAG, "%s", line);
    } else if (level == AV_LOG_INFO) {
        LOGI(FFMPEG_LOG_TAG, "%s", line);
    } else if (level == AV_LOG_VERBOSE) {
        LOGV(FFMPEG_LOG_TAG, "%s", line);
    } else {
        LOGD(FFMPEG_LOG_TAG, "%s", line);
    }
}


static long long GetSysCurrentTime() {
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long) (time.tv_sec)) * 1000 + time.tv_usec / 1000;
    return curTime;
}

#endif //FFMPEGDEMO_ALOG_H
