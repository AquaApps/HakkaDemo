package fan.akua.hakka.server;

import android.os.Binder;
import android.os.IBinder;
import android.os.IInterface;

public interface IHakkaServer extends IInterface {
    String DESCRIPTION = "HakkaServer";

    int code_bindClient = 0x33;

    void bindClient(IBinder binder);

    int code_edit1 = 0x07;

    void edit1();

    int code_edit2 = 0x71;

    void edit2();

    int code_edit3 = 0x79;

    void edit3();

    int code_edit4 = 0x4;

    void edit4();
}
