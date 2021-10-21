//
// Created by 付安栋 on 2021/10/20.
//

#include "AudioDecoder.h"
#include "ALog.h"
#include <mutex>

#define LOG_TAG "AudioDecoder"

AudioDecoder::AudioDecoder() {
    setType(DECODER_TYPE_AUDIO);
}

AudioDecoder::~AudioDecoder() {

}

void AudioDecoder::onDecodeReady() {

}

void AudioDecoder::onFrameAvailable(AVFrame *avFrame) {
    LOGI(LOG_TAG,"onFrameAvailable");
}
