package fan.akua.hakka.server.ipc;

import android.os.IBinder;

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
}
