//
// Created by 付安栋 on 2021/9/20.
//

#include "Demuxer.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C"{
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif


#define LOG_TAG "Demuxer"

int Demuxer::init(std::string filePath) {
    int ret;
    m_AVFormatContext = avformat_alloc_context();

    ret = avformat_open_input(&m_AVFormatContext,filePath.c_str(), nullptr, nullptr);
    LOGI(LOG_TAG,"avformat_open_input ret = %d",ret);

    ret = avformat_find_stream_info(m_AVFormatContext, nullptr);
    LOGI(LOG_TAG,"avformat_find_stream_info ret = %d",ret);

    for (int i = 0;i<m_AVFormatContext->nb_streams;i++){
        if(m_AVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            m_VideoStreamIndex = i;
            break;
        }
        if(m_AVFormatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            m_AudioStreamIndex = i;
            break;
        }
    }

    m_Duration = m_AVFormatContext->duration / AV_TIME_BASE * 1000;//us to ms

    return ret;
}

AVFormatContext *Demuxer::getAVFormatContext() {
    return m_AVFormatContext;
}

int Demuxer::getVideoStreamIndex() {
    return m_VideoStreamIndex;
}

int Demuxer::getAudioStreamIndex() {
    return m_AudioStreamIndex;
}
