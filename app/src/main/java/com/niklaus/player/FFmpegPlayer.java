package com.niklaus.player;

import android.content.Context;
import android.util.Log;

import com.getkeepsafe.relinker.ReLinker;

public class FFmpegPlayer {

    private long mNativeContext;

    FFmpegPlayer(Context ctx){
        ReLinker.loadLibrary(ctx, "ffmpegplayer", new ReLinker.LoadListener() {
            @Override
            public void success() {

            }

            @Override
            public void failure(Throwable t) {

            }
        });
        mNativeContext = native_create();
    }

    public String getInfo(){
        return native_getInfo();
    }

    private native String native_getInfo();
    private native long native_create();

}
