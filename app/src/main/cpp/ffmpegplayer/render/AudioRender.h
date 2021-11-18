//
// Created by 付安栋 on 2021/10/27.
//

#ifndef FFMPEGDEMO_AUDIORENDER_H
#define FFMPEGDEMO_AUDIORENDER_H
#include "SyncQueue.h"
#include <thread>
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

struct AVStream;
struct AVFrame;

class AudioRender {
public:
    AudioRender();

    ~AudioRender();

    void init(AVStream* stream);

    static void callback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context);

    void start();

    void putAVFrame(AVFrame *avFrame);

    double getTimeStamp(){
        return m_CurTimeStamp;
    }

private:

    SLObjectItf m_EngineObj = nullptr;
    SLEngineItf m_EngineItf = nullptr;

    SLObjectItf m_OutputMixObj = nullptr;

    SLObjectItf m_AudioPlayerObj = nullptr;
    SLPlayItf m_AudioPlayerItf = nullptr;

//    SLVolumeItf m_AudioPlayerVolume = nullptr;
    SLAndroidSimpleBufferQueueItf m_BufferQueueItf;

    SyncQueue<AVFrame*> *m_SyncQueue;

    uint8_t *m_Buffer = nullptr;

    std::thread *m_LoopThread = nullptr;

    void createEngine();

    void handleAudioFrameQueue();

    void updateTimeStamp(AVFrame *avFrame);

    double m_CurTimeStamp;

    AVStream *m_Stream;
};


#endif //FFMPEGDEMO_AUDIORENDER_H
