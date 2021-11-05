//
// Created by 付安栋 on 2021/10/20.
//
#pragma once

#include "AudioDecoder.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/frame.h"
#ifdef __cplusplus
}
#endif

#define LOG_TAG "AudioDecoder"

AudioDecoder::AudioDecoder() {
    setType(DECODER_TYPE_AUDIO);
    fd = fopen("sdcard/123_d.pcm", "wb");
}

AudioDecoder::~AudioDecoder() {

}

void AudioDecoder::onDecodeReady() {

}

void AudioDecoder::onFrameAvailable(AVFrame *avFrame) {
//    writePCM(avFrame);
    if(m_DecodeOneFrameCallback){
        m_DecodeOneFrameCallback(avFrame);
    }
}

void AudioDecoder::writePCM(AVFrame *avFrame) {
    auto avSampleFormat = static_cast<AVSampleFormat>(avFrame->format);
    int bytePerSample = av_get_bytes_per_sample(avSampleFormat);
    if(av_sample_fmt_is_planar(avSampleFormat)) {
        for (int i = 0; i < avFrame->nb_samples; i++) {//拷贝每个采样的数据。
            fwrite((avFrame->data[0] + i * bytePerSample), 1, bytePerSample, fd);
            fwrite((avFrame->data[1] + i * bytePerSample), 1, bytePerSample, fd);
        }
    } else {
        fwrite(avFrame->data[0], 2, bytePerSample * avFrame->nb_samples, fd);
    }
}
