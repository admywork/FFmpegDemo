//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_BASEDECODER_H
#define FFMPEGDEMO_BASEDECODER_H

#include <thread>
#include "SyncQueue.h"

struct AVFormatContext;
struct AVCodecParameters;
struct AVCodec;
struct AVCodecContext;
struct AVStream;
struct AVPacket;
struct AVFrame;

enum DecoderType {
    DECODER_TYPE_VIDEO,
    DECODER_TYPE_AUDIO
};

class BaseDecoder {
public:
    BaseDecoder();

    ~BaseDecoder();

    void start();

    int init(AVStream *avStream, int streamIndex);

    void setDecodeOneFrameCallback(std::function<void(AVFrame *avFrame)> callBack){
        m_DecodeOneFrameCallback = callBack;
    };

    static void decodeLoop(BaseDecoder *decoder);

    virtual void onDecodeReady() = 0;

    void putAVPacket(AVPacket* avPacket);

    void clearQueue();

    virtual void onFrameAvailable(AVFrame *avFrame) = 0;

    AVCodecContext* getAVCodecContext(){
        return m_AVCodecContext;
    }

protected:

    DecoderType m_Type;

    int m_StreamIndex = -1;

    AVStream *m_AVStream = nullptr;
    //解码器
    AVCodec *m_AVCodec = nullptr;
    //解码器上下文
    AVCodecContext *m_AVCodecContext = nullptr;

    std::mutex m_Mutex;

    std::thread *m_DecodeThread = nullptr;

    void setType(DecoderType decoderType) {
        m_Type = decoderType;
    }

    SyncQueue<AVPacket*> *m_SyncQueue;

    std::function<void(AVFrame *avFrame)> m_DecodeOneFrameCallback;

    struct timeval m_lastTimeval;

private:

    double m_CurTimeStamp;

    char *getLogTag() {
        return const_cast<char *>(m_Type == DECODER_TYPE_VIDEO ? "VideoDecoder" : "AudioDecoder");
    }

    int decodeOnePacket(AVPacket *avPacket);

    void updateTimeStamp(AVFrame *avFrame);

};


#endif //FFMPEGDEMO_BASEDECODER_H
