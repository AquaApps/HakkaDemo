package fan.akua.hakka.server;

import android.os.RemoteException;

import java.io.IOException;
import java.net.ServerSocket;
import java.net.Socket;

import fan.akua.hakka.server.ipc.IPC;

public class ManagerListenServer implements Runnable {
    @Override
    public void run() {
        try {
            ServerSocket serverSocket = new ServerSocket(ServerConstants.PORT_LISTEN);
            while (true) {
                Socket accept = serverSocket.accept();
                accept.close();
                //当新的manager启动时，发送给他binder
                postToManager();
            }
        } catch (IOException e) {
            ServerConstants.log("ManagerListenServer " + e);
        }
        HakkaServer.Instance.killServer();
    }

    public static void postToManager() {
        try {
            ServerConstants.log("postToManager");
            IPC.sendBinderToAllManager(HakkaServer.Instance);
        } catch (RemoteException e) {
            e.printStackTrace();
        }
    }
}