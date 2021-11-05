//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_DEMUXER_H
#define FFMPEGDEMO_DEMUXER_H

#include <string>
#include <thread>
#include <functional>

struct AVFormatContext;
struct AVStream;
struct AVPacket;

class Demuxer {
public:
    Demuxer(){}

    ~Demuxer(){}

    int init(const std::string& filePath);

    void start();

    AVFormatContext* getAVFormatContext(){
        return m_AVFormatContext;
    };

    int getVideoStreamIndex(){
        return m_VideoStreamIndex;
    };

    AVStream* getVideoStream(){
        return m_VideoStream;
    };

    int getAudioStreamIndex(){
        return m_AudioStreamIndex;
    };

    AVStream* getAudioStream(){
        return m_AudioStream;
    };

    void setDemuxOnePacketCallback(std::function<void(AVPacket*)> callback){
        m_DemuxOnePacketCallback = callback;
    }

    int getVideoWidth();
    int getVideoHeight();
    int64_t getVideoDuration();
    int getVideoRotation();

private:
    //封装格式上下文
    AVFormatContext *m_AVFormatContext = nullptr;

    int m_AudioStreamIndex = -1;

    int m_VideoStreamIndex = -1;

    AVStream *m_VideoStream;

    AVStream *m_AudioStream;

    std::thread *m_DemuxThread = nullptr;

    bool videoEof;

    bool audioEof;

    //总时长 ms
    long m_Duration = 0;
    //旋转角度
    int m_Rotation = 0;

    std::function<void(AVPacket *avPacket)> m_DemuxOnePacketCallback;

    static void demuxLoop(Demuxer *demuxer);
};


#endif //FFMPEGDEMO_DEMUXER_H
