//
// Created by 付安栋 on 2021/10/22.
//



#include "ALog.h"
#include "AudioFilter.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavfilter/avfilter.h"
#include "libavutil/channel_layout.h"
#include "libavutil/opt.h"
#ifdef __cplusplus
}
#endif

#define LOG_TAG "AudioFilter"

AudioFilter::AudioFilter() {
    m_AVFilterGraph = avfilter_graph_alloc();
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
        LOGE(LOG_TAG, "avfilter_graph_parse_ptr ret = %d \n",ret);
        goto end;
    }


    if ((ret = avfilter_graph_config(m_AVFilterGraph, nullptr)) < 0) {
        LOGE(LOG_TAG, "avfilter_graph_config ret = %d \n",ret);
        goto end;
    }

    end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;

}
