package fan.akua.hakka.demo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.PixelFormat;
import android.os.Build;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.TextView;

import com.google.android.material.progressindicator.LinearProgressIndicator;

import java.lang.reflect.Field;
import java.util.Objects;

public class FloatWindow {
    private final Context mContext;
    private final WindowManager mWindowManager;
    private WindowManager.LayoutParams mWindowParams;

    private View mFloatLayout;

    private Button i, S, E, A, L;
    private LinearProgressIndicator progress;
    private TextView logWindow;

    public FloatWindow(Context context) {
        this.mContext = context;
        mWindowManager = (WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE);
        initFloatWindow();
        initLogWindow();
    }

    private void initLogWindow() {
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT,
                (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) ?
                        WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY : WindowManager.LayoutParams.TYPE_PHONE,
                WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                        | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                        | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                PixelFormat.TRANSLUCENT);

        logWindow = new TextView(mContext);
        params.gravity = Gravity.TOP | Gravity.START;
        mWindowManager.addView(logWindow, params);
    }

    @SuppressLint("InflateParams")
    private void initFloatWindow() {
        mContext.setTheme(R.style.Theme_HakkaDemo);
        LayoutInflater inflater = LayoutInflater.from(mContext);
        if (inflater == null)
            return;
        mFloatLayout = inflater.inflate(R.layout.float_view, null);

        i = mFloatLayout.findViewById(R.id.btnI);
        S = mFloatLayout.findViewById(R.id.btnS);
        E = mFloatLayout.findViewById(R.id.btnE);
        A = mFloatLayout.findViewById(R.id.btnA);
        L = mFloatLayout.findViewById(R.id.btnL);
        progress = mFloatLayout.findViewById(R.id.progress);

        mWindowParams = new WindowManager.LayoutParams();
        //8.0新特性
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            mWindowParams.type = WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY;
        }
        mWindowParams.format = PixelFormat.RGBA_8888;
        mWindowParams.flags = WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE;
        mWindowParams.gravity = Gravity.START | Gravity.TOP;
        mWindowParams.width = WindowManager.LayoutParams.WRAP_CONTENT;
        mWindowParams.height = WindowManager.LayoutParams.WRAP_CONTENT;

        assert i != null;
        assert S != null;
        assert E != null;
        assert A != null;
        assert L != null;
        assert progress != null;
        progress.hide();
        i.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                progress.show();
            }
        });
    }

    public void showFloatWindow() {
        if (mFloatLayout.getParent() == null) {
            DisplayMetrics metrics = new DisplayMetrics();
            mWindowManager.getDefaultDisplay().getMetrics(metrics);
            mWindowParams.x = metrics.widthPixels;
            mWindowParams.y = getSysBarHeight(mContext);
            mWindowManager.addView(mFloatLayout, mWindowParams);
        }
    }

    public void hideFloatWindow() {
        if (mFloatLayout.getParent() != null)
            mWindowManager.removeView(mFloatLayout);
        if (logWindow.getParent() != null)
            mWindowManager.removeView(logWindow);
    }


    // 获取系统状态栏高度
    @SuppressLint("PrivateApi")
    public static int getSysBarHeight(Context contex) {
        Class<?> c;
        Object obj;
        Field field;
        int x;
        int sbar = 0;
        try {
            c = Class.forName("com.android.internal.R$dimen");
            obj = c.newInstance();
            field = c.getField("status_bar_height");
            x = Integer.parseInt(Objects.requireNonNull(field.get(obj)).toString());
            sbar = contex.getResources().getDimensionPixelSize(x);
        } catch (Exception ignored) {

        }
        return sbar;
    }
}