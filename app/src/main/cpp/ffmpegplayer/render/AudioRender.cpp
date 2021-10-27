//
// Created by 付安栋 on 2021/10/27.
//

#include "AudioRender.h"
#include "ALog.h"

#define LOG_TAG "AudioRender"

AudioRender::AudioRender() {

}

AudioRender::~AudioRender() {

}

void AudioRender::init() {
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

    ret = (*m_AudioPlayerObj)->GetInterface(m_AudioPlayerObj, SL_IID_VOLUME,
                                               &m_AudioPlayerVolume);
    if (ret != SL_RESULT_SUCCESS) {
        LOGE(LOG_TAG,"createAudioPlayer GetInterface SL_IID_VOLUME fail. ret=%d", ret);
        return;
    }
}

void AudioRender::callback(SLAndroidSimpleBufferQueueItf bufferQueue, void *context) {
    auto *render = static_cast<AudioRender *>(context);
    render->HandleAudioFrameQueue();
}

void AudioRender::HandleAudioFrameQueue() {
    if (m_AudioPlayerItf == nullptr) return;

}
