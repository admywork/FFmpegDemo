package com.niklaus.player;


import android.view.Surface;

public class FFmpegPlayer {

    private long mNativeRef;

    public FFmpegPlayer(){
        mNativeRef = native_create();
    }

    public String getInfo(){
        return native_getInfo(mNativeRef);
    }

    public void setDataSource(String path){
        native_setDataSource(mNativeRef,path);
    }

    public void setPreview(Surface surface) {
        native_setPreview(mNativeRef,surface);
    }

    public void prepare(){
        native_prepare(mNativeRef);
    }

    public void start(){
        native_start(mNativeRef);
    }

    public void pause(){
        native_pause(mNativeRef);
    }

    public void seekTo(long time_ms){
        native_seekTo(mNativeRef,time_ms);
    }

    public int getVideoWidth(){
        return native_getVideoWidth(mNativeRef);
    }

    public int getVideoHeight(){
        return native_getVideoHeight(mNativeRef);
    }

    public long getVideoDuration(){
        return native_getVideoDuration(mNativeRef);
    }

    public int getVideoRotation(){
        return native_getVideoRotation(mNativeRef);
    }

    public long getCurrentPosition() {
        return native_getCurrentPosition(mNativeRef);
    }



    /************************native方法*****************************/
    private native String native_getInfo(long nativeRef);
    private native long native_create();
    private native void native_setDataSource(long nativeRef,String path);
    private native void native_setPreview(long nativeRef,Surface surface);
    private native void native_prepare(long nativeRef);
    private native void native_start(long nativeRef);
    private native void native_pause(long nativeRef);
    private native void native_seekTo(long nativeRef,long time_ms);

    private native int native_getVideoWidth(long nativeRef);
    private native int native_getVideoHeight(long nativeRef);
    private native int native_getVideoRotation(long nativeRef);
    private native long native_getVideoDuration(long nativeRef);
    private native long native_getCurrentPosition(long nativeRef);
}
