//
// Created by 付安栋 on 2021/10/22.
//

#include "AudioFilter.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavfilter/avfilter.h"
#ifdef __cplusplus
}
#endif

AudioFilter::AudioFilter() {
    m_AVFilterGraph = avfilter_graph_alloc();
}

AudioFilter::~AudioFilter() {

}

void AudioFilter::init(int channels, int sample_rate, int sample_fmt) {

}
