//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_DEMUXER_H
#define FFMPEGDEMO_DEMUXER_H

struct AVFormatContext;
struct AVStream;

#include <string>
class Demuxer {
public:
    Demuxer(){}

    ~Demuxer(){}

    int init(const std::string& filePath);

    AVFormatContext* getAVFormatContext();

    AVStream* getVideoStream();

    AVStream* getAudioStream();

private:
    //封装格式上下文
    AVFormatContext *m_AVFormatContext = nullptr;

    int m_AudioStreamIndex = -1;

    int m_VideoStreamIndex = -1;

    AVStream* m_VideoStream;

    AVStream* m_AudioStream;

    //总时长 ms
    long m_Duration = 0;
};


#endif //FFMPEGDEMO_DEMUXER_H
