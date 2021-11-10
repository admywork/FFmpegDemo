//
// Created by 付安栋 on 2021/11/6.
//

#include "VideoFilter.h"
#include "ALog.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
#include "libavfilter/avfilter.h"
#include "libavfilter/buffersink.h"
#include "libavfilter/buffersrc.h"
#ifdef __cplusplus
}
#endif

#define LOG_TAG "AudioFilter"

VideoFilter::VideoFilter() {

}

VideoFilter::~VideoFilter() {

}

int VideoFilter::init(int width, int height, int pix_fmt, AVRational sample_aspect_ratio,AVRational time_base) {

    char *filters_descr = "format=pix_fmts=rgb";
    char args[512];
    int ret = 0;
    const AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    const AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_RGB24, AV_PIX_FMT_NONE };

    m_AVFilterGraph = avfilter_graph_alloc();
    if (!outputs || !inputs || !m_AVFilterGraph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             width, height, pix_fmt,
             time_base.num, time_base.den,
             sample_aspect_ratio.num, sample_aspect_ratio.den);

    ret = avfilter_graph_create_filter(&m_Buffersrc_ctx, buffersrc, "in",
                                       args, NULL, m_AVFilterGraph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer source\n");
        goto end;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&m_Buffersink_ctx, buffersink, "out",
                                       NULL, NULL, m_AVFilterGraph);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot create buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(m_Buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }

    /*
     * Set the endpoints for the filter graph. The filter_graph will
     * be linked to the graph described by filters_descr.
     */

    /*
     * The buffer source output must be connected to the input pad of
     * the first filter described by filters_descr; since the first
     * filter input label is not specified, it is set to "in" by
     * default.
     */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = m_Buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = nullptr;

    /*
     * The buffer sink input must be connected to the output pad of
     * the last filter described by filters_descr; since the last
     * filter output label is not specified, it is set to "out" by
     * default.
     */
    inputs->name       = av_strdup("out");
    inputs->filter_ctx = m_Buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = nullptr;

    if ((ret = avfilter_graph_parse_ptr(m_AVFilterGraph, filters_descr,
                                        &inputs, &outputs, nullptr)) < 0)
        goto end;

    if ((ret = avfilter_graph_config(m_AVFilterGraph, nullptr)) < 0)
        goto end;

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

AVFrame *VideoFilter::filterFrame(AVFrame *srcFrame) {
    if (!m_AVFilterGraph) {
        initFilter(srcFrame->width, srcFrame->height, srcFrame->sample_aspect_ratio,
             srcFrame.time);
    }
    if (av_buffersrc_add_frame_flags(m_Buffersrc_ctx, avFrame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
        LOGE(LOG_TAG, "Error while feeding the audio filtergraph\n");
    }
    AVFrame *filtFrame = av_frame_alloc();
    int ret = av_buffersink_get_frame(m_Buffersink_ctx, filtFrame);
    if (ret < 0) {
        av_frame_free(&filtFrame);
        if (ret != AVERROR(EAGAIN)) {
            LOGE(LOG_TAG, "av_buffersink_get_frame ret = %d", ret);
        }
        return nullptr;
    }
//    writePCM(filtFrame);
    return filtFrame;
}
