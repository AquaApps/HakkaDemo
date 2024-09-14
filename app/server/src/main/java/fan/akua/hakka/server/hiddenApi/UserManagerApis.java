package fan.akua.hakka.server.hiddenApi;


import android.app.IActivityManager;
import android.content.Context;
import android.content.pm.UserInfo;
import android.os.IUserManager;
import android.os.RemoteException;
import android.os.ServiceManager;

import java.util.List;

public class UserManagerApis {
    private static IUserManager iUserManager;

    public static void init() {
        if (iUserManager == null)
            iUserManager = IUserManager.Stub.asInterface(ServiceManager.getService(Context.USER_SERVICE));
    }

    public static List<UserInfo> getUsers(boolean excludePartial, boolean excludeDying, boolean excludePreCreated) throws RemoteException{
        init();
        return iUserManager.getUsers(excludePartial,excludeDying,excludePreCreated);
    }
}
