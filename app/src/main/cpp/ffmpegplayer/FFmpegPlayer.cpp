//
// Created by 付安栋 on 2021/9/20.
//

#include "FFmpegPlayer.h"

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

void FFmpegPlayer::setFilePath(std::string filePath) {
    if(!m_FilePath.empty()){
        return;
    }
    m_FilePath = std::move(filePath);
    m_Demuxer = new Demuxer();
    m_Demuxer->init(m_FilePath);
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
