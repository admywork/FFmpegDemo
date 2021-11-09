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

    public void prepare(){
        native_prepare(mNativeRef);
    }

    public void start(){
        native_start(mNativeRef);
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

    public void setPreview(Surface surface) {
        native_setPreview(mNativeRef,surface);
    }



    /************************native方法*****************************/
    private native String native_getInfo(long nativeRef);
    private native long native_create();
    private native void native_setDataSource(long nativeRef,String path);
    private native void native_prepare(long nativeRef);
    private native void native_start(long nativeRef);

    private native int native_getVideoWidth(long nativeRef);
    private native int native_getVideoHeight(long nativeRef);
    private native int native_getVideoRotation(long nativeRef);
    private native long native_getVideoDuration(long nativeRef);
    private native void native_setPreview(long nativeRef,Surface surface);
}
