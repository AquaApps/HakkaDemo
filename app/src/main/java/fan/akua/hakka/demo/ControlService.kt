package fan.akua.hakka.demo

import android.content.Intent
import android.os.Build
import android.os.IBinder
import android.os.RemoteException
import androidx.core.app.NotificationManagerCompat
import androidx.lifecycle.LifecycleService
import fan.akua.hakka.server.ipc.IPC
import fan.akua.hakka.server.ipc.IPC.ServerState


class ControlService : LifecycleService() {
    companion object {
        private const val NOTIFICATION_ID = 330771794
    }

    private var mNotificationManager: NotificationManagerCompat? = null
    private var mMusicNotification: NotificationDistribute? = null
    private var floatWindow: FloatWindow? = null

    private val binder: IClient.Stub = object : IClient.Stub() {
        override fun searchStart() {
            floatWindow?.searchStart()
        }

        override fun searchEnd(size: Int) {
            floatWindow?.searchEnd(size)
        }
    }

    override fun onCreate() {
        super.onCreate()
        mMusicNotification = NotificationDistribute.Builder(this)
        mNotificationManager = NotificationDistribute.createChannel(this)
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForeground(NOTIFICATION_ID, mMusicNotification!!.build())
        }

        floatWindow = FloatWindow(this)
        floatWindow?.showFloatWindow()
        IPC.setObserver { _, arg ->
            if (arg == ServerState.Running)
                IPC.getHakka()?.bindClient(binder)
        }
    }

    override fun onDestroy() {
        floatWindow?.hideFloatWindow()
        super.onDestroy()
    }

    override fun onBind(intent: Intent): IBinder {
        super.onBind(intent)
        return binder
    }
}