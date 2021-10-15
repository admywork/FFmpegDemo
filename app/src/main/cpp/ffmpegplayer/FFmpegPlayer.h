//
// Created by 付安栋 on 2021/9/20.
//

#ifndef FFMPEGDEMO_FFMPEGPLAYER_H
#define FFMPEGDEMO_FFMPEGPLAYER_H

#include <string>
#include "Demuxer.h"


class FFmpegPlayer {

public:

    FFmpegPlayer();

    ~FFmpegPlayer();

    std::string getInfo();

    void setFilePath(std::string filePath);


private:

    std::string m_FilePath;

    //解封装
    Demuxer *m_Demuxer;

};


#endif //FFMPEGDEMO_FFMPEGPLAYER_H
