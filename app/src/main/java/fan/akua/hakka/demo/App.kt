package fan.akua.hakka.demo

import android.app.Application
import android.content.Intent
import android.os.Build

class App : Application() {

    override fun onCreate() {
        super.onCreate()
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            startForegroundService(Intent(this, ControlService::class.java))
        } else {
            startService(Intent(this, ControlService::class.java))
        }
    }
}