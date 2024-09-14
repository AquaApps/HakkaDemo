package fan.akua.hakka.server.hiddenApi;


import android.annotation.SuppressLint;
import android.app.ActivityManager;
import android.app.ActivityManagerNative;
import android.app.ContentProviderHolder;
import android.app.IActivityManager;
import android.app.IProcessObserver;
import android.app.IUidObserver;
import android.content.Context;
import android.content.IContentProvider;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.IPackageManager;
import android.os.Binder;
import android.os.Build;
import android.os.IBinder;
import android.os.RemoteException;
import android.os.ServiceManager;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;


@SuppressLint("SoonBlockedPrivateApi")
public class ActivityManagerApis {
    private static IActivityManager iActivityManager;

    public static void init() {
        if (iActivityManager == null)
//            iActivityManager = IActivityManager.Stub.asInterface(ServiceManager.getService(Context.ACTIVITY_SERVICE));
            iActivityManager = (IActivityManager) ActivityManagerNative.getDefault();
    }

    public static IBinder peekService(Intent service, String resolvedType, String callingPackage) throws RemoteException {
        init();
        return iActivityManager.peekService(service, resolvedType, callingPackage);
    }

    public static void registerProcessObserver(IProcessObserver observer) throws RemoteException {
        init();
        iActivityManager.registerProcessObserver(observer);
    }

    public static void registerUidObserver(IUidObserver observer, int which, int cutpoint, String callingPackage) throws RemoteException {
        init();
        iActivityManager.registerUidObserver(observer, which, cutpoint, callingPackage);
    }

    @SuppressLint("BlockedPrivateApi")
    public static IContentProvider getContentProviderExternal(String auth, int userId, IBinder token, String tag) throws RemoteException {
        init();
        ContentProviderHolder holder = null;
        if (Build.VERSION.SDK_INT > 28) {
            holder = iActivityManager.getContentProviderExternal(auth, userId, token, tag);
        } else {
            Method method = null;
            try {
                method = iActivityManager.getClass().getDeclaredMethod("getContentProviderExternal", String.class, int.class, IBinder.class);
                holder = (ContentProviderHolder) method.invoke(iActivityManager, auth, userId, token);
            } catch (NoSuchMethodException | InvocationTargetException | IllegalAccessException e) {
                e.printStackTrace();
            }
        }
        if (holder == null)
            throw new RuntimeException("getContentProviderExternal fail");
        return holder.provider;
    }

    public static void forceStopPackage(String packageName, int userId) throws RemoteException {
        init();
        iActivityManager.forceStopPackage(packageName, userId);
    }

    public static void removeContentProviderExternal(String name, IBinder token) throws RemoteException {
        init();
        iActivityManager.removeContentProviderExternal(name, token);
    }

    public static final int UID_OBSERVER_PROCSTATE = 1;

    public static final int UID_OBSERVER_GONE = 1 << 1;

    public static final int UID_OBSERVER_IDLE = 1 << 2;

    public static final int UID_OBSERVER_ACTIVE = 1 << 3;
    public static final int UID_OBSERVER_CACHED = 1 << 4;

    public static final int PROCESS_STATE_UNKNOWN = -1;

}