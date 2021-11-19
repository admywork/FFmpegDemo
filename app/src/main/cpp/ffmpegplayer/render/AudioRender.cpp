//
// Created by 付安栋 on 2021/10/27.
//

#pragma once

#include "AudioRender.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C"{
#endif
#include "libavutil/frame.h"
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif

#define LOG_TAG "AudioRender"

AudioRender::AudioRender() {

}

AudioRender::~AudioRender() {

}

void AudioRender::init(AVStream* stream) {
    m_Stream = stream;
    m_SyncQueue = new SyncQueue<AVFrame*>(20);
    createEngine();
}

void AudioRender::createEngine() {
    SLresult ret = SL_RESULT_SUCCESS;
    ret = slCreateEngine(&m_EngineObj, 0, nullptr, 0, nullptr, nullptr);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG, "createEngine slCreateEngine fail. ret=%d", ret);
        return;
    }
    ret = (*m_EngineObj)->Realize(m_EngineObj, SL_BOOLEAN_FALSE);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG, "createEngine Realize fail. ret=%d", ret);
        return;
    }
    ret = (*m_EngineObj)->GetInterface(m_EngineObj, SL_IID_ENGINE, &m_EngineItf);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG, "createEngine GetInterface fail. ret=%d", ret);
        return;
    }
    /**************************/
    const SLInterfaceID mids[1] = {SL_IID_ENVIRONMENTALREVERB};
    const SLboolean mreq[1] = {SL_BOOLEAN_FALSE};

    ret = (*m_EngineItf)->CreateOutputMix(m_EngineItf, &m_OutputMixObj, 1, mids, mreq);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG, "createOutputMix fail. ret=%d", ret);
        return;
    }

    ret = (*m_OutputMixObj)->Realize(m_OutputMixObj, SL_BOOLEAN_FALSE);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG, "CreateOutputMix Realize fail. ret=%d", ret);
        return;
    }
    /***************************/
    SLDataLocator_AndroidSimpleBufferQueue android_queue = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE,
                                                            2};
    SLDataFormat_PCM pcm = {
            SL_DATAFORMAT_PCM,//format type
            (SLuint32) 2,//channel count
            SL_SAMPLINGRATE_44_1,//44100hz
            SL_PCMSAMPLEFORMAT_FIXED_16,// bits per sample
            SL_PCMSAMPLEFORMAT_FIXED_16,// container size
            SL_SPEAKER_FRONT_LEFT | SL_SPEAKER_FRONT_RIGHT,// channel mask
            SL_BYTEORDER_LITTLEENDIAN // endianness
    };
    SLDataSource slDataSource = {&android_queue, &pcm};

    SLDataLocator_OutputMix outputMix = {SL_DATALOCATOR_OUTPUTMIX, m_OutputMixObj};
    SLDataSink slDataSink = {&outputMix, nullptr};

    const SLInterfaceID ids[3] = {SL_IID_BUFFERQUEUE, SL_IID_EFFECTSEND, SL_IID_VOLUME};
    const SLboolean req[3] = {SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE, SL_BOOLEAN_TRUE};

    ret = (*m_EngineItf)->CreateAudioPlayer(m_EngineItf, &m_AudioPlayerObj, &slDataSource,
                                                  &slDataSink, 3, ids, req);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG,"createAudioPlayer fail. ret=%d", ret);
        return;
    }

    ret = (*m_AudioPlayerObj)->Realize(m_AudioPlayerObj, SL_BOOLEAN_FALSE);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG,"createAudioPlayer Realize fail. ret=%d", ret);
        return;
    }

    ret = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_PLAY, &m_AudioPlayerItf);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG,"createAudioPlayer GetInterface SL_IID_PLAY fail. ret=%d", ret);
        return;
    }

    ret = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_BUFFERQUEUE,
                                               &m_BufferQueueItf);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG,"createAudioPlayer GetInterface SL_IID_BUFFERQUEUE fail. ret=%d", ret);
        return;
    }

    ret = (*m_BufferQueueItf)->RegisterCallback(m_BufferQueueItf, callback, this);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG,"createAudioPlayer RegisterCallback fail. ret=%d", ret);
        return;
    }
}

void AudioRender::start() {
    (*m_AudioPlayerItf)->SetPlayState(m_AudioPlayerItf, SL_PLAYSTATE_PLAYING);
    if(!hasStart){
        hasStart = true;
        callback(m_BufferQueueItf, this);
    }
}

void AudioRender::pause() {
    (*m_AudioPlayerItf)->SetPlayState(m_AudioPlayerItf, SL_PLAYSTATE_PAUSED);
}

void AudioRender::callback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    auto *render = static_cast<AudioRender *>(context);
    render->handleAudioFrameQueue();
}

void AudioRender::handleAudioFrameQueue() {
    if (m_AudioPlayerItf == nullptr) return;
    AVFrame *avFrame;
    m_SyncQueue->take(avFrame);
    int bufferSize = av_samples_get_buffer_size(avFrame->linesize, 2, avFrame->nb_samples,
                               static_cast<AVSampleFormat>(avFrame->format), 1);
    delete m_Buffer;
    m_Buffer = new uint8_t[bufferSize]{0};
    memcpy(m_Buffer, avFrame->data[0], bufferSize);
    updateTimeStamp(avFrame);
    SLresult result = (*m_BufferQueueItf)->Enqueue(m_BufferQueueItf, m_Buffer, (SLuint32) bufferSize);
    if (result != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG,"createAudioPlayer Enqueue ret=%d", result);
    }
    av_frame_free(&avFrame);
}

void AudioRender::updateTimeStamp(AVFrame *avFrame) {
    if (avFrame->pkt_dts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = avFrame->pkt_dts;
    } else if (avFrame->pts != AV_NOPTS_VALUE) {
        m_CurTimeStamp = avFrame->pts;
    } else {
        m_CurTimeStamp = 0;
    }
    m_CurTimeStamp = (int64_t) ((m_CurTimeStamp * av_q2d(m_Stream->time_base)) * 1000);
}

void AudioRender::putAVFrame(AVFrame *avFrame) {
    m_SyncQueue->put(avFrame);
}

void AudioRender::clearQueue() {
    m_SyncQueue->clear();
}
