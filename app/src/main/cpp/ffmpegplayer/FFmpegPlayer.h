//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_FFMPEGPLAYER_H
#define FFMPEGDEMO_FFMPEGPLAYER_H

#include <string>

class Demuxer;
class VideoDecoder;
class AudioDecoder;
struct AVPacket;

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

    Demuxer *m_Demuxer;
    VideoDecoder *m_VideoDecoder;
    AudioDecoder *m_AudioDecoder;


    void demuxOnePacketCallBack(AVPacket *avPacket);

};


#endif //FFMPEGDEMO_FFMPEGPLAYER_H
