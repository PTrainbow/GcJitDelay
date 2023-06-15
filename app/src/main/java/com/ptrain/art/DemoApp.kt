package com.ptrain.art

import android.app.Application
import android.content.Context

class DemoApp : Application() {
    override fun attachBaseContext(base: Context?) {
        super.attachBaseContext(base)
        ARTHook.init()
    }
}