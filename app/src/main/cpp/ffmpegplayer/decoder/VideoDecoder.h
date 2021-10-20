//
// Created by 付安栋 on 2021/10/20.
//

#ifndef FFMPEGDEMO_VIDEODECODER_H
#define FFMPEGDEMO_VIDEODECODER_H

#include "BaseDecoder.h"


class VideoDecoder : public BaseDecoder{

public:
    VideoDecoder();

    ~VideoDecoder();

    virtual void onDecodeReady();
};


#endif //FFMPEGDEMO_VIDEODECODER_H
