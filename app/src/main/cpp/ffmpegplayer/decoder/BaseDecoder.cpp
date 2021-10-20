//
// Created by 付安栋 on 2021/9/20.
//

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

#define LOG_TAG "BaseDecoder"

BaseDecoder::BaseDecoder() {

}

BaseDecoder::~BaseDecoder() {

}

int BaseDecoder::init(AVStream *avStream,DecoderType type) {
    mType = type;
    m_AVStream = avStream;
    int ret = -1;
    //获取解码器参数
    AVCodecParameters *codecParameters = avStream->codecpar;
    //获取解码器
    m_AVCodec = avcodec_find_decoder(codecParameters->codec_id);
    //创建解码器上下文
    m_AVCodecContext = avcodec_alloc_context3(m_AVCodec);

    //打开解码器
    ret = avcodec_open2(m_AVCodecContext, m_AVCodec, nullptr);
    LOGI(getLogTag(),"avcodec_open2 ret = %d",ret);

    //创建 AVPacket 存放编码数据
    m_Packet = av_packet_alloc();
    //创建 AVFrame 存放解码后的数据
    m_Frame = av_frame_alloc();
    return 0;
}

void BaseDecoder::startDecode() {
    std::lock_guard<std::mutex> lck (m_Mutex);
    LOGI(getLogTag(),"startDecode");
    if(!m_DecodeThread){
        m_DecodeThread = new std::thread(decodeLoop, this);
    }
}

void BaseDecoder::decodeLoop(BaseDecoder *decoder) {
    decoder->onDecodeReady();
    LOGI(decoder->getLogTag(),"decodeLoop");
    while (true){
        if(decoder->decodeOnePacket() != 0) {

        }
        break;
    }
}

void BaseDecoder::onDecodeReady() {
    LOGI(getLogTag(),"onDecodeReady");
}

int BaseDecoder::decodeOnePacket() {
    return 1;
}
