package fan.akua.hakka.server.ipc;

import android.os.Binder;
import android.os.Parcel;
import android.os.IBinder;
import android.os.RemoteException;

import fan.akua.hakka.server.IHakkaServer;

public final class HakkaService implements IHakkaServer {
    private final IBinder remote;

    public HakkaService(IBinder remote) {
        this.remote = remote;
    }

    @Override
    public IBinder asBinder() {
        return remote;
    }

    @Override
    public void bindClient(IBinder binder) {
        Parcel _data = Parcel.obtain();
        Parcel _reply = Parcel.obtain();
        try {
            _data.writeInterfaceToken(DESCRIPTION);
            _data.writeStrongBinder(binder);
            boolean _status = remote.transact(code_bindClient, _data, _reply, 0);
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        } finally {
            _reply.recycle();
            _data.recycle();
        }
    }

    @Override
    public void edit1() {
        Parcel _data = Parcel.obtain();
        Parcel _reply = Parcel.obtain();
        try {
            _data.writeInterfaceToken(DESCRIPTION);
            boolean _status = remote.transact(code_edit1, _data, _reply, 0);
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        } finally {
            _reply.recycle();
            _data.recycle();
        }
    }

    @Override
    public void edit2() {
        Parcel _data = Parcel.obtain();
        Parcel _reply = Parcel.obtain();
        try {
            _data.writeInterfaceToken(DESCRIPTION);
            boolean _status = remote.transact(code_edit2, _data, _reply, 0);
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        } finally {
            _reply.recycle();
            _data.recycle();
        }
    }

    @Override
    public void edit3() {
        Parcel _data = Parcel.obtain();
        Parcel _reply = Parcel.obtain();
        try {
            _data.writeInterfaceToken(DESCRIPTION);
            boolean _status = remote.transact(code_edit3, _data, _reply, 0);
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        } finally {
            _reply.recycle();
            _data.recycle();
        }
    }

    @Override
    public void wallHack() {
        Parcel _data = Parcel.obtain();
        Parcel _reply = Parcel.obtain();
        try {
            _data.writeInterfaceToken(DESCRIPTION);
            boolean _status = remote.transact(code_wallHack, _data, _reply, 0);
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        } finally {
            _reply.recycle();
            _data.recycle();
        }
    }
}
