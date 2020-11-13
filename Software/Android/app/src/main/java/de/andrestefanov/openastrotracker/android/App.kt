package de.andrestefanov.openastrotracker.android

import android.app.Application
import android.util.Log
import de.andrestefanov.openastrotracker.meade.utils.Logger
import de.andrestefanov.openastrotracker.meade.utils.MEADE_LOGGER
import org.koin.android.ext.koin.androidContext
import org.koin.android.ext.koin.androidLogger
import org.koin.core.context.startKoin

class App : Application() {

    override fun onCreate() {
        super.onCreate()

        MEADE_LOGGER = object : Logger {
            override fun logv(tag: String, message: String) {
                Log.v(tag, message)
            }

            override fun loge(tag: String, message: String, exception: Exception?) {
                Log.e(tag, message, exception)
            }
        }

        startKoin {
            androidLogger()
            androidContext(this@App)
            modules(module)
        }
    }

}