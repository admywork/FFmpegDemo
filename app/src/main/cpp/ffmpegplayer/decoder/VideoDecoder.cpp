//
// Created by 付安栋 on 2021/10/20.
//

#pragma once

#include "VideoDecoder.h"
#include "ALog.h"

#define LOG_TAG "VideoDecoder"

VideoDecoder::VideoDecoder() {
    setType(DECODER_TYPE_VIDEO);
}

VideoDecoder::~VideoDecoder() {

}

void VideoDecoder::onDecodeReady() {

}

void VideoDecoder::onFrameAvailable(AVFrame *avFrame) {
    if(m_DecodeOneFrameCallback){
        m_DecodeOneFrameCallback(avFrame);
    }
}
