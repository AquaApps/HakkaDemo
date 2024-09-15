package fan.akua.hakka.demo

import android.content.Intent
import android.os.Build
import androidx.core.app.NotificationManagerCompat
import androidx.lifecycle.LifecycleService

class ControlService : LifecycleService() {
    companion object {
        private const val NOTIFICATION_ID = 330771794
    }

    private var mNotificationManager: NotificationManagerCompat? = null
    private var mMusicNotification: NotificationDistribute? = null
    private var floatWindow: FloatWindow? = null

    override fun onStartCommand(intent: Intent?, flags: Int, startId: Int): Int {
        mMusicNotification = NotificationDistribute.Builder(this)
        if (mNotificationManager == null) {
            mNotificationManager = NotificationDistribute.createChannel(this)
        }
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForeground(NOTIFICATION_ID, mMusicNotification!!.build())
        }
        return super.onStartCommand(intent, flags, startId)
    }

    override fun onCreate() {
        super.onCreate()
        floatWindow = FloatWindow(this)
        floatWindow?.showFloatWindow()
    }

    override fun onDestroy() {
        floatWindow?.hideFloatWindow()
        super.onDestroy()
    }
}