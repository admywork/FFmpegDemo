//
// Created by 付安栋 on 2021/10/20.
//

#ifndef FFMPEGDEMO_AUDIODECODER_H
#define FFMPEGDEMO_AUDIODECODER_H

#include "BaseDecoder.h"

#include <iostream>
#include <fstream>


class AudioDecoder : public BaseDecoder{

public:

    AudioDecoder();

    ~AudioDecoder();

    virtual void onDecodeReady();

    virtual void onFrameAvailable(AVFrame *avFrame);

private:
    std::fstream fstream;

    void writePCM(AVFrame *avFrame);

};


#endif //FFMPEGDEMO_AUDIODECODER_H
