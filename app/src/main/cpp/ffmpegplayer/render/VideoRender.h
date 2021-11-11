//
// Created by 付安栋 on 2021/11/8.
//

#ifndef FFMPEGDEMO_VIDEORENDER_H
#define FFMPEGDEMO_VIDEORENDER_H

#include <jni.h>
#include <thread>
#include <android/native_window.h>
#include "SyncQueue.h"

struct AVFrame;

class VideoRender {
public:
    VideoRender();
    ~VideoRender();

    void init();

    void setPreview(JNIEnv *env, jobject jSurface,int width,int height);

    void putAVFrame(AVFrame *avFrame);

    void start();

    void renderFrame(AVFrame* avframe);

private:

    ANativeWindow *m_ANativeWindow;

    ANativeWindow_Buffer m_NativeWindowBuffer;

    SyncQueue<AVFrame*> *m_SyncQueue;

    int m_VideoWidth;
    int m_VideoHeight;

    int m_DstWidth;
    int m_DstHeight;

    std::thread *m_LoopThread = nullptr;

    static void renderLoop(VideoRender *audioRender);

};


#endif //FFMPEGDEMO_VIDEORENDER_H
