//
// Created by 付安栋 on 2021/10/27.
//

#ifndef FFMPEGDEMO_AUDIORENDER_H
#define FFMPEGDEMO_AUDIORENDER_H
#include "SyncQueue.h"
#include <thread>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

struct AVFrame;

class AudioRender {
public:
    AudioRender();

    ~AudioRender();

    void init();

    static void callback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

    void start();

    void putAVFrame(AVFrame *avFrame);

private:

    SLObjectItf m_EngineObj = nullptr;
    SLEngineItf m_EngineItf = nullptr;

    SLObjectItf m_OutputMixObj = nullptr;

    SLObjectItf m_AudioPlayerObj = nullptr;
    SLPlayItf m_AudioPlayerItf = nullptr;

    SLVolumeItf m_AudioPlayerVolume = nullptr;
    SLAndroidSimpleBufferQueueItf m_BufferQueueItf;

    SyncQueue<AVFrame*> *m_SyncQueue;

    std::thread *m_LoopThread = nullptr;

    void createEngine();

    void handleAudioFrameQueue();

    static void renderLoop(AudioRender *audioRender);
};


#endif //FFMPEGDEMO_AUDIORENDER_H
