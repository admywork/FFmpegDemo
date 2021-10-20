//
// Created by 付安栋 on 2021/10/20.
//

#ifndef FFMPEGDEMO_AUDIODECODER_H
#define FFMPEGDEMO_AUDIODECODER_H

#include "BaseDecoder.h"


class AudioDecoder : public BaseDecoder{

public:

    AudioDecoder();

    ~AudioDecoder();

    virtual void onDecodeReady();

};


#endif //FFMPEGDEMO_AUDIODECODER_H
