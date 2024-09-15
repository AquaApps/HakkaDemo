package fan.akua.hakka.server.ipc;

import android.content.ContentProvider;
import android.content.ContentValues;
import android.content.IContentProvider;
import android.content.pm.UserInfo;
import android.database.Cursor;
import android.net.Uri;
import android.os.Binder;
import android.os.Bundle;
import android.os.IBinder;
import android.os.RemoteException;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import java.util.List;
import java.util.Observer;

import fan.akua.hakka.server.ServerConstants;
import fan.akua.hakka.server.hiddenApi.ActivityManagerApis;
import fan.akua.hakka.server.hiddenApi.ContentProviderApis;
import fan.akua.hakka.server.hiddenApi.UserManagerApis;

public class IPC extends ContentProvider {
    public static enum ServerState {
        Stop,
        Starting,
        Running,
    }

    public static void sendBinderToAllManager(Binder binder) throws RemoteException {
        sendBinderToApp(binder, ServerConstants.PACKAGE_NAME_MANAGER, "blog.akua.fan", "IPC", 0, true);
//        List<UserInfo> users = UserManagerApis.getUsers(true, true, true);
//        users.forEach(userInfo -> sendBinderToApp(binder, ServerConstants.PACKAGE_NAME_MANAGER, "blog.akua.fan", "IPC", userInfo.id, true));
    }

    private static void sendBinderToApp(Binder binder, String packageName, String authorization, String body, int userId, boolean retry) {
        IContentProvider provider = null;

        IBinder token = null;

        try {
            provider = ActivityManagerApis.getContentProviderExternal(authorization, userId, token, null);
            if (provider == null) {
                return;
            }
            if (!provider.asBinder().pingBinder()) {
                if (retry) {
                    ActivityManagerApis.forceStopPackage(packageName, userId);
                    Thread.sleep(1000);
                    // 仅尝试一次
                    sendBinderToApp(binder, packageName, authorization, body, userId, false);
                }
                return;
            }
            if (!retry) {
                ServerConstants.log("retry works");
            }

            Bundle extra = new Bundle();
            extra.putParcelable(IPCConstants.EXTRA_BINDER, new BinderContainer(binder));
            Bundle reply = ContentProviderApis.call(provider, IPCConstants.PROVIDER_HEAD + body, null, authorization, IPCConstants.METHOD_SEND_BINDER, null, extra);
            if (reply.getInt(IPCConstants.METHOD_SEND_BINDER_BAK) == IPCConstants.SuccessCode) {
                ServerConstants.log("send binder success");
            }
        } catch (Throwable tr) {
            ServerConstants.log("failed send binder to user app " + packageName + " in user " + userId);
        } finally {
            if (provider != null) {
                try {
                    ActivityManagerApis.removeContentProviderExternal(authorization, token);
                } catch (Throwable tr) {
                    ServerConstants.log("removeContentProviderExternal");
                }
            }
        }
    }

    private volatile static HakkaService binder;

    private volatile static Observer observer;

    public static void setObserver(Observer observer) {
        IPC.observer = observer;
    }

    public static HakkaService getHakka() {
        return binder;
    }

    @Override
    public Bundle call(String method, String arg, Bundle extras) {
        return this.call(null, method, arg, extras);
    }

    @Override
    public Bundle call(String authority, String method, String arg, Bundle extras) {
        if (extras == null) {
            return null;
        }

        extras.setClassLoader(BinderContainer.class.getClassLoader());

        Bundle reply = new Bundle();
        if (IPCConstants.METHOD_SEND_BINDER.equals(method)) {
            handleSendBinder(extras);
        }
        reply.putInt(IPCConstants.METHOD_SEND_BINDER_BAK, IPCConstants.SuccessCode);
        return reply;
    }

    private void handleSendBinder(Bundle extras) {
        BinderContainer container = extras.getParcelable(IPCConstants.EXTRA_BINDER);
        if (container != null && container.binder != null) {
            IBinder remote = container.binder;
            binder = new HakkaService(remote);
            if (observer == null) return;
            observer.update(null, ServerState.Running);

            try {
                remote.linkToDeath(() -> observer.update(null, ServerState.Stop), 0);
            } catch (RemoteException e) {
                e.printStackTrace();
                observer.update(null, ServerState.Stop);
            }
        }
    }

    @Override
    public boolean onCreate() {
        return true;
    }

    @Nullable
    @Override
    public Cursor query(@NonNull Uri uri, @Nullable String[] projection, @Nullable String selection, @Nullable String[] selectionArgs, @Nullable String sortOrder) {
        return null;
    }

    @Nullable
    @Override
    public String getType(@NonNull Uri uri) {
        return "";
    }

    @Nullable
    @Override
    public Uri insert(@NonNull Uri uri, @Nullable ContentValues values) {
        return null;
    }

    @Override
    public int delete(@NonNull Uri uri, @Nullable String selection, @Nullable String[] selectionArgs) {
        return 0;
    }

    @Override
    public int update(@NonNull Uri uri, @Nullable ContentValues values, @Nullable String selection, @Nullable String[] selectionArgs) {
        return 0;
    }
}
