package fan.akua.hakka.server.hiddenApi;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

public class DdmHandleAppNameApi {
    public static void setAppName(String name,int userID) throws ClassNotFoundException, NoSuchMethodException, InvocationTargetException, IllegalAccessException {
        Class<?> threadClazz = Class.forName("android.ddm.DdmHandleAppName");
        Method method = threadClazz.getMethod("setAppName", String.class,int.class);
        method.invoke(null, name,userID);
    }
}