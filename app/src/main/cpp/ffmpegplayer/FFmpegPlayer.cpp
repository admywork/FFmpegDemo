//
// Created by 付安栋 on 2021/9/20.
//

#include "FFmpegPlayer.h"

#include "Demuxer.h"
#include "VideoDecoder.h"
#include "AudioDecoder.h"
#include "VideoFilter.h"
#include "AudioFilter.h"
#include "VideoRender.h"
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

    m_VideoRender = std::make_unique<VideoRender>();
    m_VideoRender->init(m_Demuxer->getVideoStream());

    m_AudioRender = std::make_unique<AudioRender>();
    m_AudioRender->init(m_Demuxer->getAudioStream());

    m_VideoRender->setTimeStampCallBack([=]()-> double{
        return m_AudioRender->getTimeStamp();
    });
    m_State = PREPARED;
    av_log_set_callback(log_callback);
}

void FFmpegPlayer::start() {
    m_Demuxer->start();
    m_VideoDecoder->start();
    m_VideoRender->start();
    m_AudioDecoder->start();
    m_AudioRender->start();
    m_State = PLAYING;
}

void FFmpegPlayer::pause() {
    m_AudioRender->pause();
    m_State = PAUSED;
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
    if(!m_VideoFilter){
        m_VideoFilter = std::make_unique<VideoFilter>();
        AVCodecContext *avCodecContext = m_VideoDecoder->getAVCodecContext();
        m_VideoFilter->initFilter(avFrame->width,avFrame->height,avFrame->format,m_Demuxer->getVideoStream()->time_base,avCodecContext->sample_aspect_ratio);
    }
//    LOGI(LOG_TAG,"video frame pkt_dts = %ld, video frame pts = %ld",avFrame->pkt_dts,avFrame->pts);
    AVFrame *filterFrame = m_VideoFilter->filterFrame(avFrame);
    av_frame_free(&avFrame);
    if(filterFrame){
        m_VideoRender->putAVFrame(filterFrame);
    }
}

void FFmpegPlayer::decodeOneAudioFrameCallBack(AVFrame *avFrame) {
    if(!m_AudioFilter){
        m_AudioFilter = std::make_unique<AudioFilter>();
        m_AudioFilter->init(avFrame->channels,avFrame->sample_rate,avFrame->format,{1,avFrame->sample_rate});
    }
//    LOGI(LOG_TAG,"audio frame pkt_dts = %ld, audio frame pts = %ld",avFrame->pkt_dts,avFrame->pts);
    AVFrame *filterFrame = m_AudioFilter->filterFrame(avFrame);
    av_frame_free(&avFrame);
    if (filterFrame) {
        filterFrame->format = AV_SAMPLE_FMT_S16;
        filterFrame->channels = 2;
        filterFrame->channel_layout = AV_CH_LAYOUT_STEREO;
        m_AudioRender->putAVFrame(filterFrame);
    }
}

int FFmpegPlayer::getVideoWidth() {
    return m_Demuxer->getVideoWidth();
}

int FFmpegPlayer::getVideoHeight() {
    return m_Demuxer->getVideoHeight();
}

long FFmpegPlayer::getVideoDuration() {
    return m_Demuxer->getVideoDuration();
}

int FFmpegPlayer::getVideoRotation() {
    return m_Demuxer->getVideoRotation();
}

long FFmpegPlayer::getCurrentPosition() {
    return m_AudioRender->getTimeStamp();
}

void FFmpegPlayer::setPreview(JNIEnv *env, jobject jobject) {
    m_VideoRender->setPreview(env,jobject,getVideoWidth(),getVideoHeight());
}

void FFmpegPlayer::seekTo(long time_ms) {
    m_VideoDecoder->clearQueue();
    m_AudioDecoder->clearQueue();
    m_VideoRender->clearQueue();
    m_AudioRender->clearQueue();
    m_Demuxer->seekTo(time_ms);
}