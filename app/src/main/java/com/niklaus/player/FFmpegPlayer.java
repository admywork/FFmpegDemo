package com.niklaus.player;

import android.content.Context;
import android.util.Log;

import com.getkeepsafe.relinker.ReLinker;

public class FFmpegPlayer {

    FFmpegPlayer(Context ctx){
        ReLinker.loadLibrary(ctx, "ffmpegplayer", new ReLinker.LoadListener() {
            @Override
            public void success() {

            }

            @Override
            public void failure(Throwable t) {

            }
        });
    }

}
