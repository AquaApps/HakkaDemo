package fan.akua.hakka.demo

import android.app.Notification
import android.content.Context
import android.graphics.drawable.Icon

import androidx.core.app.NotificationChannelCompat
import androidx.core.app.NotificationCompat
import androidx.core.app.NotificationManagerCompat

/**
 * MusicPlayerService的一个模块，负责创建通知。
 */
open class NotificationDistribute private constructor(
    private val context: Context,
) {
    class Builder(context: Context) :
        NotificationDistribute(context)

    private val notificationCompat = NotificationCompat.Builder(context, NOTIFICATION_CHANNEL_ID)


    init {
        notificationCompat.apply {
            setShowWhen(false)
            setCategory(Notification.CATEGORY_SERVICE)
            setLargeIcon(Icon.createWithResource(context, R.drawable.ic_akua))
            setVisibility(NotificationCompat.VISIBILITY_PUBLIC)
        }
    }

    fun build(): Notification {
        return notificationCompat.build()
    }

    companion object {

        private const val NOTIFICATION_CHANNEL_ID = "hakka_notification_channel"

        fun createChannel(context: Context): NotificationManagerCompat {
            val notificationManager = NotificationManagerCompat.from(context)
            val channel = NotificationChannelCompat.Builder(
                NOTIFICATION_CHANNEL_ID,
                NotificationManagerCompat.IMPORTANCE_LOW
            ).apply {
                setName("HakkaService")
                setDescription("A powerful memory editor.")
                setVibrationEnabled(false)
                setShowBadge(false)
                setSound(null, null)
            }
            notificationManager.createNotificationChannel(channel.build())
            return notificationManager
        }
    }
}