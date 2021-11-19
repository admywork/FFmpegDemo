//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_FFMPEGPLAYER_H
#define FFMPEGDEMO_FFMPEGPLAYER_H

#include <string>
#include <jni.h>

class Demuxer;
class VideoDecoder;
class AudioDecoder;
class AudioFilter;
class VideoFilter;
class VideoRender;
class AudioRender;

struct AVPacket;
struct AVFrame;

class FFmpegPlayer {

public:

    FFmpegPlayer();

    ~FFmpegPlayer();

    std::string getInfo();

    void setDataSource(std::string path);

    void prepare();

    void start();

    void pause();

    int getVideoWidth();

    int getVideoHeight();

    long getVideoDuration();

    int getVideoRotation();

    long getCurrentPosition();

    void setPreview(JNIEnv *pEnv, jobject jSurface);

private:

    std::string m_Path;

    std::unique_ptr<Demuxer> m_Demuxer;
    std::unique_ptr<VideoDecoder> m_VideoDecoder;
    std::unique_ptr<AudioDecoder> m_AudioDecoder;
    std::unique_ptr<VideoFilter> m_VideoFilter;
    std::unique_ptr<AudioFilter> m_AudioFilter;
    std::unique_ptr<VideoRender> m_VideoRender;
    std::unique_ptr<AudioRender> m_AudioRender;


    void demuxOnePacketCallBack(AVPacket *avPacket);

    void decodeOneVideoFrameCallBack(AVFrame *avFrame);

    void decodeOneAudioFrameCallBack(AVFrame *avFrame);

};


#endif //FFMPEGDEMO_FFMPEGPLAYER_H
