//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_FFMPEGPLAYER_H
#define FFMPEGDEMO_FFMPEGPLAYER_H

#include <string>

class Demuxer;
class VideoDecoder;
class AudioDecoder;
class AudioFilter;

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

private:

    std::string m_Path;

    std::unique_ptr<Demuxer> m_Demuxer;
    std::unique_ptr<VideoDecoder> m_VideoDecoder;
    std::unique_ptr<AudioDecoder> m_AudioDecoder;
    std::unique_ptr<AudioFilter> m_AudioFilter;


    void demuxOnePacketCallBack(AVPacket *avPacket);

    void decodeOneVideoFrameCallBack(AVFrame *avFrame);

    void decodeOneAudioFrameCallBack(AVFrame *avFrame);

};


#endif //FFMPEGDEMO_FFMPEGPLAYER_H
