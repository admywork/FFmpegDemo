//
// Created by 付安栋 on 2021/11/6.
//

#ifndef FFMPEGDEMO_VIDEOFILTER_H
#define FFMPEGDEMO_VIDEOFILTER_H

struct AVFrame;
struct AVFilterGraph;
struct AVRational;
struct AVFilterContext;

class VideoFilter {
public:
    VideoFilter();

    ~VideoFilter();

    int initFilter(int width, int height, int pix_fmt, AVRational time_base,AVRational sample_aspect_ratio);

    AVFrame* filterFrame(AVFrame *srcFrame);

private:
    AVFilterContext *m_Buffersink_ctx = nullptr;

    AVFilterContext *m_Buffersrc_ctx = nullptr;

    AVFilterGraph *m_AVFilterGraph = nullptr;
};


#endif //FFMPEGDEMO_VIDEOFILTER_H
