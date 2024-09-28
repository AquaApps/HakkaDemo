package fan.akua.hakka.server;

import android.annotation.SuppressLint;
import android.content.pm.ApplicationInfo;
import android.os.Binder;
import android.os.Handler;
import android.os.IBinder;
import android.os.Looper;
import android.os.Parcel;
import android.os.RemoteException;
import android.system.Os;
import android.util.Log;

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

import fan.akua.hakka.Hakka;
import fan.akua.hakka.PlayerEntry;
import fan.akua.hakka.demo.IClient;
import fan.akua.hakka.server.hiddenApi.DdmHandleAppNameApi;
import fan.akua.hakka.server.hiddenApi.PackageManagerApis;

public class HakkaServer extends Binder implements IHakkaServer {

    public static void main(String[] strArr) {
        try {
            DdmHandleAppNameApi.setAppName(ServerConstants.SERVER_NAME, 0);
        } catch (Exception e) {
            throw new RuntimeException("DdmHandleAppNameApi" + e.getMessage());
        }

        Looper.prepare();

        try {
            int uid = Os.getuid();
            int gid = Os.getgid();
            int pid = Os.getpid();
            ServerConstants.log(String.format("uid %s gid %s pid %s", uid, gid, pid));

            Thread.setDefaultUncaughtExceptionHandler((thread, exception) -> {
                StringWriter sw = new StringWriter();
                PrintWriter pw = new PrintWriter(sw);
                exception.printStackTrace(pw);
                String stackTrace = sw.toString();
                ServerConstants.log(String.format("%s: RunningError", thread.getName()));
                ServerConstants.log(stackTrace);
                @SuppressLint("SimpleDateFormat") SimpleDateFormat dateFormat = new SimpleDateFormat("MM-dd-HH-mm");
                String time = dateFormat.format(new Date());
                File logFile = new File(ServerConstants.LOG_PATH, String.format("serverErrorLog%s.txt", time));
                ServerConstants.printLog(logFile);
                System.exit(400);
            });

            Instance = new HakkaServer();
        } catch (RemoteException e) {
            throw new RuntimeException("Something wrong when new HakkaServer");
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        Looper.loop();

        throw new RuntimeException("Main thread loop unexpectedly exited");
    }

    private static final ExecutorService executor = Executors.newSingleThreadExecutor();
    public static HakkaServer Instance;
    private static IClient client;
    private static Handler handler;


    private ApplicationInfo getManagerApplicationInfo() {
        try {
            return PackageManagerApis.getApplicationInfo(ServerConstants.PACKAGE_NAME_MANAGER, 0, 0);
        } catch (RemoteException e) {
            ServerConstants.log("getManagerApplicationInfo fail " + e.getMessage());
            return null;
        }
    }

    public HakkaServer() throws RemoteException, InterruptedException {
        ApplicationInfo managerApplicationInfo = getManagerApplicationInfo();
        assert managerApplicationInfo != null;
        ApkChangedObservers.start(managerApplicationInfo.sourceDir, () -> {
            // 这里的重新获取很关键
            if (getManagerApplicationInfo() == null) {
                ServerConstants.log("manager app is uninstalled in user 0, exiting...");
                killServer();
            }
        });
        System.load("/data/misc/hakka_lib.so");
        attachInterface(this, DESCRIPTION);
        new Thread(new ManagerListenServer()).start();
        handler = new Handler();
        handler.post(ManagerListenServer::postToManager);
    }

    public void killServer() {
        ServerConstants.log("killServer");
        System.exit(0);
    }

    @Override
    public IBinder asBinder() {
        return this;
    }

    @Override
    protected boolean onTransact(int code, Parcel data, Parcel reply, int flags) throws RemoteException {
        data.enforceInterface(DESCRIPTION);
        switch (code) {
            case code_bindClient:
                IBinder iBinder = data.readStrongBinder();
                bindClient(iBinder);
                reply.writeNoException();
                return true;
            case code_edit1:
                executor.submit(this::edit1);
                reply.writeNoException();
                return true;
            case code_edit2:
                executor.submit(this::edit2);
                reply.writeNoException();
                return true;
            case code_edit3:
                executor.submit(this::edit3);
                reply.writeNoException();
                return true;
            case code_wallHack:
                executor.submit(this::wallHack);
                reply.writeNoException();
                return true;
        }
        return super.onTransact(code, data, reply, flags);
    }

    @Override
    public void bindClient(IBinder binder) {
        try {
            binder.linkToDeath(() -> client = null, 0);
            client = IClient.Stub.asInterface(binder);
        } catch (RemoteException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    public void edit1() {
        if (client != null) {
            handler.post(() -> {
                try {
                    if (client != null) client.searchStart();
                } catch (RemoteException ignored) {

                }
            });
            int size = Hakka.edit1();
            handler.post(() -> {
                try {
                    if (client != null) client.searchEnd(size);
                } catch (RemoteException ignored) {

                }
            });
        }
    }

    @Override
    public void edit2() {
        if (client != null) {
            handler.post(() -> {
                try {
                    if (client != null) client.searchStart();
                } catch (RemoteException ignored) {

                }
            });
            int size = Hakka.edit2();
            handler.post(() -> {
                try {
                    if (client != null) client.searchEnd(size);
                } catch (RemoteException ignored) {

                }
            });
        }
    }

    @Override
    public void edit3() {
        if (client != null) {
            handler.post(() -> {
                try {
                    if (client != null) client.searchStart();
                } catch (RemoteException ignored) {

                }
            });
            int size = Hakka.edit3();
            handler.post(() -> {
                try {
                    if (client != null) client.searchEnd(size);
                } catch (RemoteException ignored) {

                }
            });
        }
    }

    @Override
    public void wallHack() {
        if (client != null) {
            handler.post(() -> {
                try {
                    if (client != null) client.wallHackStart();
                } catch (RemoteException ignored) {

                }
            });

            new Thread(() -> {
                final long address = Hakka.wallHack();
                handler.post(() -> {
                    try {
                        if (client != null) client.wallHackEnd();
                    } catch (RemoteException ignored) {

                    }
                });
                while (true) {
                    try {
                        if (client != null) {
                            List<PlayerEntry> playerEntries = new ArrayList<>();
                            for (int i = 0; i < 10; i++)
                                playerEntries.add(Hakka.readEntry(address + i * 0x20L));
                            client.wallHackLoop(playerEntries);
                        }
                    } catch (RemoteException e) {
                        ServerConstants.log("readEntry error " + e);
                    }
                    try {
                        Thread.sleep(1000);
                    } catch (InterruptedException ignored) {
                    }
                }
            }).start();
        }
    }
}
