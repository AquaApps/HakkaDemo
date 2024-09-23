package fan.akua.hakka;

import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.NonNull;

public class PlayerEntry implements Parcelable {
    private final long index;
    private final long x;
    private final long y;

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
}
