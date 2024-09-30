package fan.akua.hakka;

import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.NonNull;

public class PlayerEntry implements Parcelable {
    private final long index;
    private long x;
    private long y;

    public PlayerEntry(long index, long x, long y) {
        this.index = index;
        this.x = x;
        this.y = y;
    }

    protected PlayerEntry(Parcel in) {
        index = in.readLong();
        x = in.readLong();
        y = in.readLong();
    }



    public PlayerEntry mapRange(int dis, int base) {
        this.x = mapRange(this.x, dis) + base;
        this.y =  - mapRange(this.y, dis) + base;
        return this;
    }

    private static long mapRange(long value, long dis) {
        double scale = (double) dis / 58900;
        return (long) (value * scale);
    }

    public static final Creator<PlayerEntry> CREATOR = new Creator<PlayerEntry>() {
        @Override
        public PlayerEntry createFromParcel(Parcel in) {
            return new PlayerEntry(in);
        }

        @Override
        public PlayerEntry[] newArray(int size) {
            return new PlayerEntry[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(@NonNull Parcel dest, int flags) {
        dest.writeLong(index);
        dest.writeLong(x);
        dest.writeLong(y);
    }

    public long getX() {
        return x;
    }

    public long getY() {
        return y;
    }

    public long getIndex() {
        return index;
    }
}
