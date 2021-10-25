//
// Created by 付安栋 on 2021/10/22.
//

#ifndef FFMPEGDEMO_AUDIOFILTER_H
#define FFMPEGDEMO_AUDIOFILTER_H

struct AVFilterGraph;
struct AVRational;
struct AVFilterContext;

class AudioFilter {

public:
    AudioFilter();

    ~AudioFilter();

    int init(int channels,int sample_rate,int sample_fmt,AVRational time_base);

private:

    AVFilterContext *m_Buffersink_ctx;

    AVFilterContext *m_Buffersrc_ctx;

    AVFilterGraph *m_AVFilterGraph;

};


#endif //FFMPEGDEMO_AUDIOFILTER_H
