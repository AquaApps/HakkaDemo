package fan.akua.hakka.demo;

import android.content.Context;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.ArrayList;
import java.util.List;
import java.util.function.Consumer;
import java.util.function.Function;

import fan.akua.hakka.PlayerEntry;

public class PlayerHead extends View {
    private List<PlayerEntry> playerEntries;
    private Paint paint;

    public PlayerHead(Context context) {
        super(context);
        init();
    }

    public PlayerHead(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public PlayerHead(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        init();
    }

    private void init() {
        paint = new Paint();
        paint.setColor(Color.RED); // 点的颜色
        paint.setStyle(Paint.Style.FILL); // 填充样式
        paint.setStrokeWidth(10);
    }

    @Override
    protected void onDraw(Canvas canvas) {
        super.onDraw(canvas);
        if (playerEntries != null) {
            for (PlayerEntry entry : playerEntries) {
                entry.mapRange(190, 170);
                Log.e("simonServer", "show " + entry.getX() + " " + entry.getY());
                canvas.drawPoint(entry.getX(), entry.getY(), paint);
            }
        }
    }

    public void updatePlayerEntries(List<PlayerEntry> entries) {
        this.playerEntries = entries;
        invalidate(); // 请求重绘
    }
}
