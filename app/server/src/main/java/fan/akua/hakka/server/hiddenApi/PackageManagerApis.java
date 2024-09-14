package fan.akua.hakka.server.hiddenApi;

import android.app.ApplicationPackageManager;
import android.content.AttributionSource;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageInstallerCallback;
import android.content.pm.IPackageManager;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.pm.ParceledListSlice;
import android.content.pm.UserInfo;
import android.os.Build;
import android.os.Bundle;
import android.os.RemoteException;
import android.os.ServiceManager;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;


public class PackageManagerApis {
    private static IPackageManager iPackageManager;

    public static void init() {
        if (iPackageManager == null)
            iPackageManager = IPackageManager.Stub.asInterface(ServiceManager.getService("package"));
    }

    public static PackageManager getPackageManager() {
        init();
        return (PackageManager) iPackageManager;
    }

    public static ApplicationInfo getApplicationInfo(String packageName, long flags, int userId) throws RemoteException {
        ApplicationInfo reply;
        init();
        if (Build.VERSION.SDK_INT >= 33) {
            //安卓13
            try {
                Method method = iPackageManager.getClass().getDeclaredMethod("getApplicationInfo", String.class, long.class, int.class);
                method.setAccessible(true);
                reply = (ApplicationInfo) method.invoke(iPackageManager, packageName, flags, userId);
            } catch (Exception e) {
                throw new RuntimeException("call failed " + e.getMessage());
            }
        } else {
            reply = iPackageManager.getApplicationInfo(packageName, (int) flags, userId);
        }
        return reply;
    }

}