package fan.akua.hakka.demo;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.appcompat.app.AppCompatActivity;

import android.app.ActivityManager;
import android.content.Intent;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.provider.Settings;
import android.system.ErrnoException;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.google.android.material.snackbar.Snackbar;
import com.topjohnwu.superuser.Shell;

import java.io.IOException;

import fan.akua.hakka.demo.databinding.ActivityMainBinding;
import fan.akua.hakka.server.ServerStarter;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        ActivityManager ams = (ActivityManager) getSystemService(ACTIVITY_SERVICE);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            ActivityResultLauncher<String[]> notificationPermissionRequest = registerForActivityResult(new ActivityResultContracts
                    .RequestMultiplePermissions(), result ->
                    Snackbar.make(binding.getRoot(), "已获得通知权限", Snackbar.LENGTH_SHORT).show()
            );
            notificationPermissionRequest.launch(new String[]{
                    android.Manifest.permission.POST_NOTIFICATIONS
            });
        }
        if (!Settings.canDrawOverlays(getApplicationContext())) {
            Intent intent = new Intent(Settings.ACTION_MANAGE_OVERLAY_PERMISSION, Uri.parse("package:" + getPackageName()));
            startActivityForResult(intent, 10);
        }

        binding.start.setOnClickListener(v -> {
            try {
                String command = "sh " + ServerStarter.writeFiles(MainActivity.this);
                Shell.su(command).submit(out -> {
                    out.getOut().forEach(s -> Log.d("simonServer", "out: " + s));
                    if (out.getCode() != 0) {
                        out.getErr().forEach(s -> Log.d("simonServer", "err: " + s));
                    }
                });
            } catch (IOException | ErrnoException e) {
                Log.e("simonServer", "exec error: " + e);
            }
        });
    }

}