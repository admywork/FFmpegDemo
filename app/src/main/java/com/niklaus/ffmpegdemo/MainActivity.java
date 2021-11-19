package com.niklaus.ffmpegdemo;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.res.ResourcesCompat;

import android.content.Intent;
import android.content.res.Resources;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.ViewGroup;
import android.widget.SeekBar;
import android.widget.TextView;

import com.getkeepsafe.relinker.ReLinker;
import com.hjq.permissions.OnPermissionCallback;
import com.hjq.permissions.Permission;
import com.hjq.permissions.XXPermissions;
import com.niklaus.ffmpegdemo.databinding.ActivityMainBinding;
import com.niklaus.player.FFmpegPlayer;
import com.orhanobut.logger.AndroidLogAdapter;
import com.orhanobut.logger.Logger;

import java.util.List;

public class MainActivity extends AppCompatActivity {



    private String filePath = Environment.getExternalStorageDirectory().getPath()+"/bluetooth/heng_10.mp4";
//    private String filePath = Environment.getExternalStorageDirectory().getPath()+"/哆啦A梦：伴我同行2.mp4";

    private ActivityMainBinding binding;
    private FFmpegPlayer mFFmpegPlayer;

    public static final int GET_POSITION = 0;

    private Handler timeHandler = new Handler(){

        @Override
        public void handleMessage(@NonNull Message msg) {
            if(mFFmpegPlayer==null){
                return;
            }
            switch (msg.what){
                case GET_POSITION:
                    upDateTime();
                    timeHandler.sendEmptyMessageDelayed(GET_POSITION,200);
                  break;
            }
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        Logger.addLogAdapter(new AndroidLogAdapter());
        ReLinker.loadLibrary(this, "ffmpegplayer");

        requestAppPermissions();

        binding.btnPause.setOnClickListener(new View.OnClickListener() {
            boolean shouldPause;
            @Override
            public void onClick(View v) {
                shouldPause = !shouldPause;
                if(shouldPause){
                    mFFmpegPlayer.pause();
                }else{
                    mFFmpegPlayer.start();
                }
                binding.btnPause.setText(shouldPause?"播放":"暂停");
            }
        });
    }

    private void initPlayer(){
        mFFmpegPlayer = new FFmpegPlayer();
        binding.sampleText.setText(mFFmpegPlayer.getInfo());
        mFFmpegPlayer.setDataSource(filePath);
        mFFmpegPlayer.prepare();
        Logger.i("videoWidth = "+ mFFmpegPlayer.getVideoWidth() + " videoHeight = "+ mFFmpegPlayer.getVideoHeight() +" videoDuration = "+ mFFmpegPlayer.getVideoDuration() +" videoRotation = "+ mFFmpegPlayer.getVideoRotation());


        int screenWidth = Resources.getSystem().getDisplayMetrics().widthPixels;
        ViewGroup.LayoutParams layoutParams = binding.surfaceview.getLayoutParams();
        layoutParams.width = screenWidth;
        layoutParams.height = (int) (1.000f * layoutParams.width * mFFmpegPlayer.getVideoHeight() / mFFmpegPlayer.getVideoWidth());
        binding.surfaceview.setLayoutParams(layoutParams);

        SurfaceHolder holder = binding.surfaceview.getHolder();
        holder.addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(@NonNull SurfaceHolder holder) {
                mFFmpegPlayer.setPreview(holder.getSurface());
                mFFmpegPlayer.start();
            }

            @Override
            public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

            }
        });
        binding.seekbar.setMax((int) (mFFmpegPlayer.getVideoDuration()));
        timeHandler.sendEmptyMessageDelayed(GET_POSITION,100);
        binding.tvTotalDuration.setText(String.format("%.1f",mFFmpegPlayer.getVideoDuration() * 1.0 / 1000));

        binding.seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {

            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
                mFFmpegPlayer.seekTo(seekBar.getProgress());
            }
        });
    }

    private void upDateTime() {
        int currentTime = (int) (mFFmpegPlayer.getCurrentPosition());
        binding.seekbar.setProgress(currentTime);
        binding.tvCurPosition.setText(String.format("%.1f",currentTime * 1.0 / 1000));
    }

    private void requestAppPermissions() {
        XXPermissions.with(this)
                .permission(Permission.MANAGE_EXTERNAL_STORAGE)
                .permission(Permission.RECORD_AUDIO)
                .permission(Permission.CAMERA)
                .request(new OnPermissionCallback() {

                    @Override
                    public void onGranted(List<String> permissions, boolean all) {
                        initPlayer();
                    }

                    @Override
                    public void onDenied(List<String> permissions, boolean never) {
                        if (never) {
                            XXPermissions.startPermissionActivity(MainActivity.this, permissions);
                        } else {

                        }
                    }
                });
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == XXPermissions.REQUEST_CODE) {
            if (XXPermissions.isGranted(this, Permission.RECORD_AUDIO) &&
                    XXPermissions.isGranted(this, Permission.Group.CALENDAR)) {
            } else {

            }
        }
    }

}