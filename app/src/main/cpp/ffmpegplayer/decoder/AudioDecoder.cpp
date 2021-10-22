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
    fstream.open("sdcard/123.pcm", std::ios::out);
}

AudioDecoder::~AudioDecoder() {

}

void AudioDecoder::onDecodeReady() {

}

void AudioDecoder::onFrameAvailable(AVFrame *avFrame) {
    LOGI(LOG_TAG, "onFrameAvailable avFrame format = %d, channels = %d, sample_rate = %d",avFrame->format,avFrame->channels,avFrame->sample_rate);
    auto avSampleFormat = static_cast<AVSampleFormat>(avFrame->format);
    int bytePerSample = av_get_bytes_per_sample(avSampleFormat);
    if(av_sample_fmt_is_planar(avSampleFormat)) {
        for (int i = 0; i < avFrame->nb_samples; i++) {//拷贝每个采样的数据。
            fstream.write(
                    reinterpret_cast<const char *>(avFrame->data[0] + i * bytePerSample),
                    bytePerSample);
            fstream.write(
                    reinterpret_cast<const char *>(avFrame->data[1] + i * bytePerSample),
                    bytePerSample);
        }
    } else {
        fstream.write(reinterpret_cast<const char *>(avFrame->data[0]),
                 avFrame->linesize[0]);
    }
}
