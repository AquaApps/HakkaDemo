package fan.akua.hakka.demo;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.PixelFormat;
import android.os.Build;
import android.os.IBinder;
import android.util.DisplayMetrics;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;

import com.airbnb.lottie.LottieAnimationView;
import com.google.android.material.progressindicator.LinearProgressIndicator;

import java.lang.reflect.Field;
import java.util.Objects;

import fan.akua.hakka.server.ServerConstants;
import fan.akua.hakka.server.ipc.HakkaService;
import fan.akua.hakka.server.ipc.IPC;

public class FloatWindow implements IClient {
    private final Context mContext;
    private final WindowManager mWindowManager;
    private WindowManager.LayoutParams mWindowParams;

    private View mFloatLayout;
    private View animLayout;

    private Button i, S, E, A, L;
    private LinearProgressIndicator progress;
    private LottieAnimationView animView;

    public FloatWindow(Context context) {
        this.mContext = context;
        mContext.setTheme(R.style.Theme_HakkaDemo);
        mWindowManager = (WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE);
        initFloatWindow();
        initAnimWindow();
    }

    private void initAnimWindow() {
        LayoutInflater inflater = LayoutInflater.from(mContext);
        if (inflater == null)
            return;
        animLayout = inflater.inflate(R.layout.float_anim, null);
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.MATCH_PARENT,
                WindowManager.LayoutParams.MATCH_PARENT,
                (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) ?
                        WindowManager.LayoutParams.TYPE_APPLICATION_OVERLAY : WindowManager.LayoutParams.TYPE_PHONE,
                WindowManager.LayoutParams.FLAG_NOT_TOUCH_MODAL
                        | WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE
                        | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE,
                PixelFormat.TRANSLUCENT);
        mWindowManager.addView(animLayout, params);
        animView = animLayout.findViewById(R.id.fuckAnimation);
    }


    @SuppressLint("InflateParams")
    private void initFloatWindow() {
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
        i.setOnClickListener(v -> {

        });
        S.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HakkaService hakka = IPC.getHakka();
                if (hakka != null) {
                    hakka.edit1();
                }
            }
        });
        E.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                HakkaService hakka = IPC.getHakka();
                if (hakka != null) {
                    hakka.edit2();
                }
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
        if (animLayout.getParent() != null)
            mWindowManager.removeView(animLayout);
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

    @Override
    public void searchStart() {
        ServerConstants.log("client searchStart");
        progress.post(() -> progress.show());
        i.post(() -> {
            i.setClickable(false);
            S.setClickable(false);
            E.setClickable(false);
            A.setClickable(false);
            L.setClickable(false);
        });
    }

    @Override
    public void searchEnd(int size) {
        ServerConstants.log("client searchEnd");
        progress.post(() -> progress.hide());
        animView.post(() -> animView.playAnimation());
        i.post(() -> {
            i.setClickable(true);
            S.setClickable(true);
            E.setClickable(true);
            A.setClickable(true);
            L.setClickable(true);
        });
    }

    @Override
    public IBinder asBinder() {
        return null;
    }
}