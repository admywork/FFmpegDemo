//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_AUDIODECODER_H
#define FFMPEGDEMO_AUDIODECODER_H

struct AVFormatContext;
struct AVCodecParameters;
struct AVCodec;
struct AVCodecContext;
struct AVPacket;
struct AVFrame;

class AudioDecoder {
public:
    AudioDecoder();
    ~AudioDecoder();

    int init(AVFormatContext *avFormatContext,int streamIndex);

private:
    //封装格式上下文
    AVFormatContext *m_AVFormatContext = nullptr;
    //解码器
    AVCodec *m_AVCodec = nullptr;
    //解码器上下文
    AVCodecContext *m_AVCodecContext = nullptr;
    //总时长 ms
    long m_Duration = 0;
    //编码的数据包
    AVPacket        *m_Packet = nullptr;
    //解码的帧
    AVFrame         *m_Frame = nullptr;

};


#endif //FFMPEGDEMO_AUDIODECODER_H
