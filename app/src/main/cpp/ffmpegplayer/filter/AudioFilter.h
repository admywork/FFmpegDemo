//
// Created by 付安栋 on 2021/10/22.
//

#ifndef FFMPEGDEMO_AUDIOFILTER_H
#define FFMPEGDEMO_AUDIOFILTER_H

struct AVFilterGraph;

class AudioFilter {

public:
    AudioFilter();

    ~AudioFilter();

    void init(int channels,int sample_rate,int sample_fmt);

private:
    AVFilterGraph *m_AVFilterGraph;

};


#endif //FFMPEGDEMO_AUDIOFILTER_H
