//
// Created by 付安栋 on 2021/9/20.
//

#include "Demuxer.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif


#define LOG_TAG "Demuxer"

int Demuxer::init(const std::string &filePath) {
    int ret;
    m_AVFormatContext = avformat_alloc_context();

    ret = avformat_open_input(&m_AVFormatContext, filePath.c_str(), nullptr, nullptr);
    LOGI(LOG_TAG, "avformat_open_input ret = %d", ret);

    ret = avformat_find_stream_info(m_AVFormatContext, nullptr);
    LOGI(LOG_TAG, "avformat_find_stream_info ret = %d", ret);

    m_AudioStreamIndex = av_find_best_stream(m_AVFormatContext, AVMEDIA_TYPE_AUDIO, -1, -1, nullptr,
                                             0);
    m_VideoStreamIndex = av_find_best_stream(m_AVFormatContext, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr,
                                             0);
    m_VideoStream = m_AVFormatContext->streams[m_VideoStreamIndex];
    m_AudioStream = m_AVFormatContext->streams[m_AudioStreamIndex];

    m_Duration = m_AVFormatContext->duration * 1000 / AV_TIME_BASE;//us to ms

    LOGI(LOG_TAG, "audioStreamIndex = %d, videoStreamIndex = %d,m_Duration = %ld ms",
         m_AudioStreamIndex, m_VideoStreamIndex, m_Duration);

    return ret;
}

void Demuxer::start() {
    if (!m_DemuxThread) {
        m_DemuxThread = new std::thread(demuxLoop, this);
    }
}

void Demuxer::demuxLoop(Demuxer *demuxer) {
    int ret;
    do {
        AVPacket* avPacket = av_packet_alloc();
        ret = av_read_frame(demuxer->m_AVFormatContext, avPacket);
        if (ret == AVERROR_EOF) {
            LOGI(LOG_TAG, "av_read_frame eof!");
        } else if (ret == 0) {
            //创建 AVPacket 存放编码数据
            demuxer->m_DemuxOnePacketCallback(avPacket);
        }
    } while (ret >= 0);
    LOGI(LOG_TAG, "demuxLoop end!");
}

int Demuxer::getVideoWidth() {
    return m_VideoStream->codecpar->width;
}

int Demuxer::getVideoHeight() {
    return m_VideoStream->codecpar->height;
}

int64_t Demuxer::getVideoDuration() {
    return m_Duration;
}

int Demuxer::getVideoRotation() {
    AVDictionaryEntry *tag = nullptr;
    int rotate;
    tag = av_dict_get(m_VideoStream->metadata, "rotate", tag, 0);
    if (tag == nullptr) {
        rotate = 0;
    } else {
        rotate = atoi(tag->value);
        rotate %= 360;
    }
    return rotate;
}
