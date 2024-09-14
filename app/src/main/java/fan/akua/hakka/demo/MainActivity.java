package fan.akua.hakka.demo;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.system.ErrnoException;
import android.util.Log;
import android.view.View;
import android.widget.TextView;

import com.topjohnwu.superuser.Shell;

import java.io.IOException;

import fan.akua.hakka.demo.databinding.ActivityMainBinding;
import fan.akua.hakka.server.ServerStarter;

public class MainActivity extends AppCompatActivity {

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // todo 自动将hakka这个execute放到/data/local下
        // todo server加载so
        // todo manager启动一个悬浮窗，用binder和server通信
        binding.start.setOnClickListener(v -> {
            try {
                String command = "sh " + ServerStarter.writeFiles(MainActivity.this);
                Shell.su(command).submit(out -> {
                    out.getOut().forEach(s -> Log.d("simon", "out: " + s));
                    if (out.getCode() != 0) {
                        out.getErr().forEach(s -> Log.d("simon", "err: " + s));
                    }
                });
            } catch (IOException | ErrnoException e) {
                throw new RuntimeException(e);
            }
        });
    }

}