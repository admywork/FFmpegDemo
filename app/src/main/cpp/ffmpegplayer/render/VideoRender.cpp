//
// Created by 付安栋 on 2021/11/8.
//

#include "VideoRender.h"
#include "ALog.h"

#include <android/native_window.h>
#include <android/native_window_jni.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/frame.h"
#include "libavutil/time.h"
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif

#define LOG_TAG "VideoRender"

VideoRender::VideoRender() {
    m_SyncQueue = new SyncQueue<AVFrame*>(10);
}

VideoRender::~VideoRender() {

}

void VideoRender::init(AVStream* stream) {
    m_AVStream = stream;
}

void VideoRender::setPreview(JNIEnv *env, jobject jSurface,int videoWidth,int videoHeight) {
    m_VideoWidth = videoWidth;
    m_VideoHeight = videoHeight;
    m_ANativeWindow = ANativeWindow_fromSurface(env, jSurface);

    ANativeWindow_setBuffersGeometry(m_ANativeWindow, m_VideoWidth,
                                     m_VideoHeight, WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer m_NativeWindowBuffer;
}

void VideoRender::renderFrame(AVFrame *avFrame) {
    //锁定当前 Window ，获取屏幕缓冲区 Buffer 的指针
    ANativeWindow_lock(m_ANativeWindow, &m_NativeWindowBuffer, nullptr);
    auto *dstBuffer = static_cast<uint8_t *>(m_NativeWindowBuffer.bits);

    int srcLineSize = avFrame->linesize[0];//输入图的步长（一行像素有多少字节）
    int dstLineSize = m_NativeWindowBuffer.stride * 4;//RGBA 缓冲区步长

    for (int i = 0; i < m_VideoHeight; ++i) {
        //一行一行地拷贝图像数据
        memcpy(dstBuffer + i * dstLineSize, avFrame->data[0] + i * srcLineSize, srcLineSize);
    }
    //解锁当前 Window ，渲染缓冲区数据
    ANativeWindow_unlockAndPost(m_ANativeWindow);
}

void VideoRender::putAVFrame(AVFrame *avFrame) {
    m_SyncQueue->put(avFrame);
}

void VideoRender::start() {
    if(!m_LoopThread){
        m_LoopThread = new std::thread(renderLoop, this);
    }
}

void VideoRender::renderLoop(VideoRender *videoRender) {
    AVFrame *avFrame;
    videoRender->m_SyncQueue->take(avFrame);
    do {
//        LOGI(LOG_TAG,"renderLoop avFrame dts = %ld",avFrame->pkt_dts);
        double audioTimeStamp = videoRender->m_TimeStampCallBack();
//        LOGI(LOG_TAG,"renderLoop audioTimeStamp = %f",audioTimeStamp);
        double videoTimeStamp = updateTimeStamp(avFrame,videoRender->m_AVStream);
        while (videoTimeStamp >audioTimeStamp){
            int sleepTime = (videoTimeStamp - audioTimeStamp) * 1000;
//            LOGI(LOG_TAG,"renderLoop sleepTime = %d",sleepTime);
            av_usleep(sleepTime);
            audioTimeStamp = videoRender->m_TimeStampCallBack();
        }
        videoRender->renderFrame(avFrame);
        av_frame_free(&avFrame);
        videoRender->m_SyncQueue->take(avFrame);
    } while (avFrame);
}

double VideoRender::updateTimeStamp(AVFrame *avFrame,AVStream *stream) {
    double curTimeStamp;
    if (avFrame->pkt_dts != AV_NOPTS_VALUE) {
        curTimeStamp = avFrame->pkt_dts;
    } else if (avFrame->pts != AV_NOPTS_VALUE) {
        curTimeStamp = avFrame->pts;
    } else {
        curTimeStamp = 0;
    }
    curTimeStamp =  (curTimeStamp * av_q2d(stream->time_base)) * 1000;
    return curTimeStamp;
}
