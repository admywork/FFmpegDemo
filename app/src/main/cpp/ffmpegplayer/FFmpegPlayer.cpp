//
// Created by 付安栋 on 2021/9/20.
//

#include "FFmpegPlayer.h"
#include "Common.h"
#include "Demuxer.h"
#include "VideoDecoder.h"
#include "AudioDecoder.h"

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
#ifdef __cplusplus
}
#endif

FFmpegPlayer::FFmpegPlayer() {

}

FFmpegPlayer::~FFmpegPlayer() {

}

void FFmpegPlayer::setDataSource(std::string filePath) {
    if(!m_Path.empty()){
        return;
    }
    m_Path = std::move(filePath);
}

void FFmpegPlayer::prepare() {
    m_Demuxer = new Demuxer();
    m_Demuxer->init(m_Path);
    m_VideoDecoder = new VideoDecoder();
    m_VideoDecoder->init(m_Demuxer->getVideoStream(),DECODER_TYPE_VIDEO);
    m_AudioDecoder = new AudioDecoder();
    m_AudioDecoder->init(m_Demuxer->getAudioStream(),DECODER_TYPE_AUDIO);
}

void FFmpegPlayer::start() {
    m_VideoDecoder->startDecode();
    m_AudioDecoder->startDecode();
}

std::string FFmpegPlayer::getInfo() {
    std::string info;
    info +="libavcodec : ";
    info +=AV_STRINGIFY(LIBAVCODEC_VERSION);
    info +="\nlibavformat : ";
    info +=AV_STRINGIFY(LIBAVFORMAT_VERSION);
    info +="\nlibavdevice : ";
    info +=AV_STRINGIFY(LIBAVDEVICE_VERSION);
    info +="\nlibavutil : ";
    info +=AV_STRINGIFY(LIBAVUTIL_VERSION);
    info +="\nlibavfilter : ";
    info +=AV_STRINGIFY(LIBAVFILTER_VERSION);
    info +="\nlibpostproc : ";
    info +=AV_STRINGIFY(LIBPOSTPROC_VERSION);
    info +="\nlibswresample : ";
    info +=AV_STRINGIFY(LIBSWRESAMPLE_VERSION);
    info +="\nlibswscale : ";
    info +=AV_STRINGIFY(LIBSWSCALE_VERSION);
    return info;
}
