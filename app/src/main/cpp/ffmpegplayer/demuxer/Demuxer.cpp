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

int Demuxer::init(const std::string& filePath) {
    int ret;
    m_AVFormatContext = avformat_alloc_context();

    ret = avformat_open_input(&m_AVFormatContext,filePath.c_str(), nullptr, nullptr);
    LOGI(LOG_TAG,"avformat_open_input ret = %d",ret);

    ret = avformat_find_stream_info(m_AVFormatContext, nullptr);
    LOGI(LOG_TAG,"avformat_find_stream_info ret = %d",ret);

    m_AudioStreamIndex = av_find_best_stream(m_AVFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr, 0);
    m_VideoStreamIndex = av_find_best_stream(m_AVFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
    m_VideoStream = m_AVFormatContext->streams[m_VideoStreamIndex];
    m_AudioStream = m_AVFormatContext->streams[m_AudioStreamIndex];
    m_Duration = m_AVFormatContext->duration * 1000 / AV_TIME_BASE ;//us to ms
    LOGI(LOG_TAG,"audioStreamIndex = %d, videoStreamIndex = %d",m_AudioStreamIndex,m_VideoStreamIndex);
    LOGI(LOG_TAG,"m_Duration = %ld ms",m_Duration);

    return ret;
}

AVFormatContext *Demuxer::getAVFormatContext() {
    return m_AVFormatContext;
}

AVStream* Demuxer::getVideoStream() {
    return m_VideoStream;
}

AVStream* Demuxer::getAudioStream() {
    return m_AudioStream;
}
