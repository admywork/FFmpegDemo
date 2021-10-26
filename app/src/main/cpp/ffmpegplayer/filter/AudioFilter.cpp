//
// Created by 付安栋 on 2021/10/22.
//
#pragma once

#include "ALog.h"
#include "AudioFilter.h"

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

AudioFilter::AudioFilter() {

}

AudioFilter::~AudioFilter() {

}

int AudioFilter::init(int channels, int sample_rate, int sample_fmt, AVRational time_base) {

    const char *filter_descr = "aresample=44100,aformat=sample_fmts=s16:channel_layouts=mono";

    char args[512];
    int ret = 0;
    const AVFilter *abuffersrc = avfilter_get_by_name("abuffer");
    const AVFilter *abuffersink = avfilter_get_by_name("abuffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs = avfilter_inout_alloc();
    static const enum AVSampleFormat out_sample_fmts[] = {AV_SAMPLE_FMT_S16, AV_SAMPLE_FMT_NONE};
    static const int64_t out_channel_layouts[] = {AV_CH_LAYOUT_MONO, -1};
    static const int out_sample_rates[] = {44100, -1};
    const AVFilterLink *outlink;

    m_AVFilterGraph = avfilter_graph_alloc();
    if (!outputs || !inputs || !m_AVFilterGraph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    snprintf(args, sizeof(args),
             "time_base=%d/%d:sample_rate=%d:sample_fmt=%s:channel_layout=0x%" PRIx64,
             time_base.num, time_base.den, sample_rate,
             av_get_sample_fmt_name(static_cast<AVSampleFormat>(sample_fmt)),
             av_get_default_channel_layout(channels));
    ret = avfilter_graph_create_filter(&m_Buffersrc_ctx, abuffersrc, "in",
                                       args, nullptr, m_AVFilterGraph);

    if (ret < 0) {
        LOGE(LOG_TAG, "Cannot create audio buffer source\n");
        goto end;
    }

    /* buffer audio sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&m_Buffersink_ctx, abuffersink, "out",
                                       nullptr, nullptr, m_AVFilterGraph);
    if (ret < 0) {
        LOGE(LOG_TAG, "Cannot create audio buffer sink\n");
        goto end;
    }

    ret = av_opt_set_int_list(m_Buffersink_ctx, "sample_fmts", out_sample_fmts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE(LOG_TAG, "Cannot set output sample format\n");
        goto end;
    }

    ret = av_opt_set_int_list(m_Buffersink_ctx, "channel_layouts", out_channel_layouts, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE(LOG_TAG, "Cannot set output channel layout\n");
        goto end;
    }

    ret = av_opt_set_int_list(m_Buffersink_ctx, "sample_rates", out_sample_rates, -1,
                              AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        LOGE(LOG_TAG, "Cannot set output sample rate\n");
        goto end;
    }

    outputs->name = av_strdup("in");
    outputs->filter_ctx = m_Buffersrc_ctx;
    outputs->pad_idx = 0;
    outputs->next = nullptr;

    inputs->name = av_strdup("out");
    inputs->filter_ctx = m_Buffersink_ctx;
    inputs->pad_idx = 0;
    inputs->next = nullptr;

    if ((ret = avfilter_graph_parse_ptr(m_AVFilterGraph, filter_descr, &inputs, &outputs,
                                        nullptr)) < 0) {
        LOGE(LOG_TAG, "avfilter_graph_parse_ptr ret = %d \n", ret);
        goto end;
    }


    if ((ret = avfilter_graph_config(m_AVFilterGraph, nullptr)) < 0) {
        LOGE(LOG_TAG, "avfilter_graph_config ret = %d \n", ret);
        goto end;
    }

    outlink = m_Buffersink_ctx->inputs[0];
    av_get_channel_layout_string(args, sizeof(args), -1, outlink->channel_layout);
    LOGI(LOG_TAG, "Output: srate:%dHz fmt:%s chlayout:%s\n",
         (int) outlink->sample_rate,
         (char *) av_x_if_null(av_get_sample_fmt_name(static_cast<AVSampleFormat>(outlink->format)),
                               "?"),
         args);

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;

}

AVFrame* AudioFilter::filterFrame(AVFrame *avFrame) {
    if (!m_AVFilterGraph) {
        init(avFrame->channels, avFrame->sample_rate, avFrame->format,
             {1, avFrame->sample_rate});
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
    return filtFrame;
}
