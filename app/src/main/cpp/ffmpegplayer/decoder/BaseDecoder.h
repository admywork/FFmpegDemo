//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_BASEDECODER_H
#define FFMPEGDEMO_BASEDECODER_H

#include <mutex>
#include <thread>

#include "Common.h"

struct AVFormatContext;
struct AVCodecParameters;
struct AVCodec;
struct AVCodecContext;
struct AVStream;
struct AVPacket;
struct AVFrame;

class BaseDecoder {
public:
    BaseDecoder();

    ~BaseDecoder();

    void start();

    int init(AVStream *avStream,int streamIndex,DecoderType type);

    static void decodeLoop(BaseDecoder *decoder);

    virtual void onDecodeReady() = 0;

protected:

    DecoderType mType;

    int m_StreamIndex = -1;

    AVStream *m_AVStream = nullptr;
    //解码器
    AVCodec *m_AVCodec = nullptr;
    //解码器上下文
    AVCodecContext *m_AVCodecContext = nullptr;
    //编码的数据包
    AVPacket        *m_Packet = nullptr;
    //解码的帧
    AVFrame         *m_Frame = nullptr;

    std::mutex m_Mutex;

    std::thread *m_DecodeThread = nullptr;

private:
    char* getLogTag(){
        return const_cast<char *>(mType == DECODER_TYPE_VIDEO ? "VideoDecoder" : "AudioDecoder");
    }

    int decodeOnePacket();
};


#endif //FFMPEGDEMO_BASEDECODER_H
