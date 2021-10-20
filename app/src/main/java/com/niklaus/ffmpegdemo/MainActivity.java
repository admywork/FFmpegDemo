package com.niklaus.ffmpegdemo;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.os.Environment;
import android.widget.TextView;

import com.getkeepsafe.relinker.ReLinker;
import com.hjq.permissions.OnPermissionCallback;
import com.hjq.permissions.Permission;
import com.hjq.permissions.XXPermissions;
import com.niklaus.ffmpegdemo.databinding.ActivityMainBinding;
import com.niklaus.player.FFmpegPlayer;

import java.util.List;

public class MainActivity extends AppCompatActivity {



    private String filePath = Environment.getExternalStorageDirectory().getPath()+"/bluetooth/heng_10.mp4";

    private ActivityMainBinding binding;
    private FFmpegPlayer mFFmpegPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        requestAppPermissions();

        ReLinker.loadLibrary(this, "ffmpegplayer");

        initPlayer();
    }

    private void initPlayer(){
        mFFmpegPlayer = new FFmpegPlayer();
        binding.sampleText.setText(mFFmpegPlayer.getInfo());
        mFFmpegPlayer.setDataSource(filePath);
        mFFmpegPlayer.prepare();
        mFFmpegPlayer.start();
    }

    private void requestAppPermissions() {
        XXPermissions.with(this)
                .permission(Permission.MANAGE_EXTERNAL_STORAGE)
                .permission(Permission.RECORD_AUDIO)
                .permission(Permission.CAMERA)
                .request(new OnPermissionCallback() {

                    @Override
                    public void onGranted(List<String> permissions, boolean all) {

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