cmake_minimum_required(VERSION 3.10.2)

project("ffmpegdemo")

set(jnilibs ${CMAKE_SOURCE_DIR}/../jniLibs)

link_directories(
        ${jnilibs}/${ANDROID_ABI}
)

include_directories(
        includes/ffmpeg
        includes/fdkaac
        includes/lame
        includes/x264

        ffmpegplayer
        ffmpegplayer/demuxer
        ffmpegplayer/decoder
        ffmpegplayer/filter
        ffmpegplayer/render
        ffmpegplayer/utils
)

file(
        GLOB_RECURSE
        ffmpegplayer_src
        *.c
        *.cpp
)

add_library(
        ffmpegplayer

        SHARED

        ${ffmpegplayer_src})


#find_library(
#        log-lib
#
#        log)

set(ffmpeg-libs
        avcodec
        avformat
        avdevice
        avutil
        avfilter
        postproc
        swresample
        swscale
        fdk-aac
        mp3lame
        x264
        )

set(native-libs
        android
        EGL
        GLESv3
        OpenSLES
        log
        m
        z
        )


target_link_libraries(
        ffmpegplayer

        ${native-libs}
        ${ffmpeg-libs})