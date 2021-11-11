//
// Created by 付安栋 on 2021/10/22.
//

#ifndef FFMPEGDEMO_AUDIOFILTER_H
#define FFMPEGDEMO_AUDIOFILTER_H

#include <iostream>
#include <fstream>

struct AVFrame;
struct AVFilterGraph;
struct AVRational;
struct AVFilterContext;

class AudioFilter {

public:
    AudioFilter();

    ~AudioFilter();

    int init(int channels,int sample_rate,int sample_fmt,AVRational time_base);

    AVFrame* filterFrame(AVFrame *srcFrame);

    FILE* fd;

    void writePCM(AVFrame *avFrame);

private:

    AVFilterContext *m_Buffersink_ctx = nullptr;

    AVFilterContext *m_Buffersrc_ctx = nullptr;

    AVFilterGraph *m_AVFilterGraph = nullptr;

};


#endif //FFMPEGDEMO_AUDIOFILTER_H
