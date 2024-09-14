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

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.Date;

import fan.akua.hakka.Hakka;
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

    public static HakkaServer Instance;

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
        new Handler().post(ManagerListenServer::postToManager);
        
//        while (true){
//            String s = Hakka.attachGame();
//            ServerConstants.log("attachGame " +s);
//            Thread.sleep(4_000);
//        }
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
//        switch (code) {
//            case getHandledNum_code:
//                String getHandledNum_result = getHandledNum();
//                reply.writeNoException();
//                reply.writeString(getHandledNum_result);
//                return true;
//            case killServer_code:
//                reply.writeNoException();
//                killServer();
//                return true;
//            case getVersion_code:
//                reply.writeNoException();
//                reply.writeString(getVersion());
//                return true;
//            case getStrategy_code:
//                ParcelableListSlice<VirtualStrategy> getStrategy_result = getStrategy();
//                reply.writeNoException();
//                reply.writeTypedObject(getStrategy_result, 0);
//                return true;
//            case addStrategy_code:
//                VirtualStrategy addStrategy_arg1 = data.readParcelable(VirtualStrategy.class.getClassLoader());
//                addStrategy(addStrategy_arg1);
//                reply.writeNoException();
//                return true;
//            case removeStrategy_code:
//                String _arg1 = data.readString();
//                removeStrategy(_arg1);
//                reply.writeNoException();
//                return true;
//            case getAllPackageInfo_code:
//                ParcelableListSlice<PackageInfoWarp> getPackageInfo_result = getAllPackageInfo();
//                reply.writeNoException();
//                reply.writeTypedObject(getPackageInfo_result, 0);
//                return true;
//            case bindPackage_code:
//                String[] bind_arg1 = new String[2];
//                boolean bind_arg2;
//                data.readStringArray(bind_arg1);
//                bind_arg2 = data.readInt() == 1;
//                int result = bindPackage(bind_arg1[0], bind_arg1[1], bind_arg2);
//                reply.writeInt(result);
//                reply.writeNoException();
//                return true;
//            case unbindPackage_code:
//                String unbind_arg1 = data.readString();
//                unbindPackage(unbind_arg1);
//                reply.writeNoException();
//                return true;
//            case exportStrategy_code:
//                String export_arg1 = data.readString();
//                reply.writeString(exportStrategy(export_arg1));
//                reply.writeNoException();
//                return true;
//            case getTruePath_code:
//                reply.writeString(getTruePath());
//                reply.writeNoException();
//                return true;
//            case setTruePath_code:
//                String setTruePath_arg1 = data.readString();
//                setTruePath(setTruePath_arg1);
//                reply.writeNoException();
//                return true;
//            case getStandardFS_code:
//                reply.writeString(getStandardFS());
//                reply.writeNoException();
//                return true;
//            case setStandardFS_code:
//                String setStandardFS_arg1 = data.readString();
//                setStandardFS(setStandardFS_arg1);
//                reply.writeNoException();
//                return true;
//            case getOpenNum_code:
//                reply.writeInt(getOpenNum());
//                reply.writeNoException();
//                return true;
//            case setAutoCreate_code:
//                boolean value = data.readInt() == 1;
//                setAutoCreate(value);
//                reply.writeNoException();
//                return true;
//            case isAutoCreate_code:
//                reply.writeInt(isAutoCreate() ? 1 : 0);
//                reply.writeNoException();
//                return true;
//            case verify_code:
//                String verify_data = data.readString();
//                int verify_result = verify(verify_data);
//                reply.writeInt(verify_result);
//                reply.writeNoException();
//                return true;
//            case printLog_code: {
//                reply.writeString(printLog());
//                reply.writeNoException();
//                return true;
//            }
//        }
        return super.onTransact(code, data, reply, flags);
    }

}
