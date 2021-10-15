//
// Created by 付安栋 on 2021/9/20.
//

#include "AudioDecoder.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C"{
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

#define LOG_TAG "AudioDecoder"

AudioDecoder::AudioDecoder() {

}

AudioDecoder::~AudioDecoder() {

}

int AudioDecoder::init(AVFormatContext *avFormatContext, int streamIndex) {
    int ret = -1;
    m_AVFormatContext = avFormatContext;
    //获取解码器参数
    AVCodecParameters *codecParameters = m_AVFormatContext->streams[streamIndex]->codecpar;
    //获取解码器
    m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
    //创建解码器上下文
    m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);

    AVDictionary *pAVDictionary = nullptr;
    av_dict_set(&pAVDictionary, "buffer_size", "1024000", 0);
    av_dict_set(&pAVDictionary, "stimeout", "20000000", 0);
    av_dict_set(&pAVDictionary, "max_delay", "30000000", 0);
    av_dict_set(&pAVDictionary, "rtsp_transport", "tcp", 0);

    //打开解码器
    ret = avcodec_open2(m_AVCodecContext, m_AVCodec, &pAVDictionary);
    LOGI(LOG_TAG,"avcodec_open2 ret = %d",ret);

    m_Duration = m_AVFormatContext->duration / AV_TIME_BASE * 1000;//us to ms
    //创建 AVPacket 存放编码数据
    m_Packet = av_packet_alloc();
    //创建 AVFrame 存放解码后的数据
    m_Frame = av_frame_alloc();
    return 0;
}
