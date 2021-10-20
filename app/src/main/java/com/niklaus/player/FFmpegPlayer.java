package com.niklaus.player;


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



    /************************native方法*****************************/
    private native String native_getInfo(long nativeRef);
    private native long native_create();
    private native void native_setDataSource(long nativeRef,String path);
    private native void native_prepare(long nativeRef);
    private native void native_start(long nativeRef);

}
