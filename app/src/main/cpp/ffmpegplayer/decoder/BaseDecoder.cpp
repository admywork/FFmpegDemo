//
// Created by 付安栋 on 2021/9/20.
//

#pragma once

#include "BaseDecoder.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C"{
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

BaseDecoder::BaseDecoder() {
    m_SyncQueue = new SyncQueue<AVPacket*>(10);
}

BaseDecoder::~BaseDecoder() {

}

int BaseDecoder::init(AVStream *avStream,int streamIndex) {
    m_AVStream = avStream;
    m_StreamIndex = streamIndex;
    LOGI(getLogTag(),"m_StreamIndex = %d",streamIndex);
    int ret = -1;
    //获取解码器参数
    AVCodecParameters *codecParameters = m_AVStream->codecpar;
    //获取解码器
    m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
    //创建解码器上下文
    m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);

    ret = avcodec_parameters_to_context(m_AVCodecContext, codecParameters);
    LOGI(getLogTag(),"avcodec_parameters_to_context ret = %d",ret);

    m_AVCodecContext->thread_count = 0;

    //打开解码器
    ret = avcodec_open2(m_AVCodecContext, m_AVCodec, nullptr);
    LOGI(getLogTag(),"avcodec_open2 ret = %d",ret);

    return 0;
}

void BaseDecoder::start() {
    std::lock_guard<std::mutex> lck (m_Mutex);
    LOGI(getLogTag(),"startDecode");
    if(!m_DecodeThread){
        m_DecodeThread = new std::thread(decodeLoop, this);
    }
}

void BaseDecoder::decodeLoop(BaseDecoder *decoder) {
    decoder->onDecodeReady();
    while (true){
        AVPacket *avPacket;
        decoder->m_SyncQueue->take(avPacket);
        decoder->decodeOnePacket(avPacket);
    }
}

int BaseDecoder::decodeOnePacket(AVPacket *avPacket) {
    int ret =avcodec_send_packet(m_AVCodecContext, avPacket);
    if(ret != 0) {
        return ret;
    }
    AVFrame *avFrame = av_frame_alloc();
    while ((ret = avcodec_receive_frame(m_AVCodecContext, avFrame)) == 0) {
        onFrameAvailable(avFrame);
    }
    av_packet_free(&avPacket);
    return ret;
}

void BaseDecoder::putAVPacket(AVPacket *avPacket) {
    m_SyncQueue->put(avPacket);
//    LOGI(getLogTag(),"putAVPacket after");
}
