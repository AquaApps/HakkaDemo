package fan.akua.hakka.server.hiddenApi;

import android.annotation.SuppressLint;
import android.content.AttributionSource;
import android.content.IContentProvider;
import android.os.Binder;
import android.os.Build;
import android.os.Bundle;
import android.os.RemoteException;

import java.lang.reflect.Method;

public class ContentProviderApis {

    public static Bundle call(IContentProvider provider, String attributeTag, String callingPkg, String authority, String method, String arg, Bundle extras) throws RemoteException {
        Bundle reply;
        if (Build.VERSION.SDK_INT >= 31) {
            AttributionSource build = new AttributionSource.Builder(Binder.getCallingUid()).setPackageName(callingPkg).build();
            try {
                @SuppressLint("BlockedPrivateApi") Method method1 = provider.getClass().getDeclaredMethod("call", AttributionSource.class, String.class, String.class, String.class, Bundle.class);
                reply = (Bundle) method1.invoke(provider, build, authority, method, arg, extras);
            } catch (Exception e) {
                throw new RuntimeException("call failed " + e.getMessage());
            }
        } else if (Build.VERSION.SDK_INT >= 30) {
            reply = provider.call(callingPkg, attributeTag, authority, method, arg, extras);
        } else if (Build.VERSION.SDK_INT >= 29) {
            try {
                Method method1 = provider.getClass().getDeclaredMethod("call", String.class, String.class, String.class, String.class, Bundle.class);
                reply = (Bundle) method1.invoke(provider, callingPkg, authority, method, arg, extras);
            } catch (Exception e) {
                throw new RuntimeException("call failed " + e.getMessage());
            }
        } else {
            reply = provider.call(callingPkg, method, arg, extras);
        }

        return reply;
    }
}
