package fan.akua.hakka.server;

import android.annotation.SuppressLint;
import android.content.pm.ApplicationInfo;
import android.os.Looper;
import android.os.RemoteException;
import android.system.Os;

import java.io.File;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.Date;

import fan.akua.hakka.server.hiddenApi.DdmHandleAppNameApi;
import fan.akua.hakka.server.hiddenApi.PackageManagerApis;

public class HakkaServer {

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

    public HakkaServer() throws RemoteException {
        ApplicationInfo managerApplicationInfo = getManagerApplicationInfo();
        assert managerApplicationInfo != null;
        ApkChangedObservers.start(managerApplicationInfo.sourceDir, () -> {
            // 这里的重新获取很关键
            if (getManagerApplicationInfo() == null) {
                ServerConstants.log("manager app is uninstalled in user 0, exiting...");
                ServerConstants.log("killServer");
                System.exit(0);
            }
        });
    }
}
