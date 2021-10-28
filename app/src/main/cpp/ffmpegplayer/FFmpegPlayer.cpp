//
// Created by 付安栋 on 2021/9/20.
//
#pragma once

#include "FFmpegPlayer.h"

#include <memory>
#include "Demuxer.h"
#include "VideoDecoder.h"
#include "AudioDecoder.h"
#include "AudioFilter.h"
#include "AudioRender.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavcodec/version.h"
#include "libavdevice/version.h"
#include "libavformat/version.h"
#include "libavutil/version.h"
#include "libavfilter/version.h"
#include "libpostproc/version.h"
#include "libswresample/version.h"
#include "libswscale/version.h"

#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif

#define LOG_TAG "FFmpegPlayer"

FFmpegPlayer::FFmpegPlayer() {

}

FFmpegPlayer::~FFmpegPlayer() {

}

std::string FFmpegPlayer::getInfo() {
    std::string info;
    info += "libavcodec : ";
    info += AV_STRINGIFY(LIBAVCODEC_VERSION);
    info += "\nlibavformat : ";
    info += AV_STRINGIFY(LIBAVFORMAT_VERSION);
    info += "\nlibavdevice : ";
    info += AV_STRINGIFY(LIBAVDEVICE_VERSION);
    info += "\nlibavutil : ";
    info += AV_STRINGIFY(LIBAVUTIL_VERSION);
    info += "\nlibavfilter : ";
    info += AV_STRINGIFY(LIBAVFILTER_VERSION);
    info += "\nlibpostproc : ";
    info += AV_STRINGIFY(LIBPOSTPROC_VERSION);
    info += "\nlibswresample : ";
    info += AV_STRINGIFY(LIBSWRESAMPLE_VERSION);
    info += "\nlibswscale : ";
    info += AV_STRINGIFY(LIBSWSCALE_VERSION);
    return info;
}

void FFmpegPlayer::setDataSource(std::string filePath) {
    if (!m_Path.empty()) {
        return;
    }
    m_Path = std::move(filePath);
}

void FFmpegPlayer::prepare() {
    m_Demuxer = std::make_unique<Demuxer>();
    m_Demuxer->init(m_Path);
    m_Demuxer->setDemuxOnePacketCallback([&](AVPacket *avPacket) {
        demuxOnePacketCallBack(avPacket);
    });

    m_VideoDecoder = std::make_unique<VideoDecoder>();
    m_VideoDecoder->init(m_Demuxer->getVideoStream(), m_Demuxer->getVideoStreamIndex());
    m_VideoDecoder->setDecodeOneFrameCallback([&](AVFrame *avFrame) {
        decodeOneVideoFrameCallBack(avFrame);
    });

    m_AudioDecoder = std::make_unique<AudioDecoder>();
    m_AudioDecoder->init(m_Demuxer->getAudioStream(), m_Demuxer->getAudioStreamIndex());
    m_AudioDecoder->setDecodeOneFrameCallback([&](AVFrame *avFrame) {
        decodeOneAudioFrameCallBack(avFrame);
    });

    m_AudioFilter = std::make_unique<AudioFilter>();

    m_AudioRender = std::make_unique<AudioRender>();
    m_AudioRender->init();

    av_log_set_callback(log_callback);
}

void FFmpegPlayer::start() {
    m_Demuxer->start();
    m_VideoDecoder->start();
    m_AudioDecoder->start();
    m_AudioRender->start();
}

void FFmpegPlayer::demuxOnePacketCallBack(AVPacket *avPacket) {
//    LOGI(LOG_TAG,"demuxOnePacketCallBack streamIndex = %d",streamIndex);
    if (avPacket->stream_index == m_Demuxer->getVideoStreamIndex()) {
        m_VideoDecoder->putAVPacket(avPacket);
    }
    if (avPacket->stream_index == m_Demuxer->getAudioStreamIndex()) {
        m_AudioDecoder->putAVPacket(avPacket);
    }
}

void FFmpegPlayer::decodeOneVideoFrameCallBack(AVFrame *avFrame) {
    av_frame_free(&avFrame);
}

void FFmpegPlayer::decodeOneAudioFrameCallBack(AVFrame *avFrame) {
    AVFrame *filteFrame = m_AudioFilter->filterFrame(avFrame);
    av_frame_free(&avFrame);
    if (filteFrame) {
        filteFrame->format = AV_SAMPLE_FMT_S16;
        filteFrame->channels = 2;
        filteFrame->channel_layout = AV_CH_LAYOUT_STEREO;
        m_AudioRender->putAVFrame(filteFrame);
    }
}


