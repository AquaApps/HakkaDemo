package fan.akua.hakka.demo;

import android.os.Parcel;
import android.os.Parcelable;

import androidx.annotation.NonNull;

public class Test implements Parcelable {
    protected Test(Parcel in) {
    }

    public static final Creator<Test> CREATOR = new Creator<Test>() {
        @Override
        public Test createFromParcel(Parcel in) {
            return new Test(in);
        }

        @Override
        public Test[] newArray(int size) {
            return new Test[size];
        }
    };

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(@NonNull Parcel dest, int flags) {
    }
}
