package com.ptrain.art

import com.bytedance.shadowhook.ShadowHook
import com.bytedance.shadowhook.ShadowHook.ConfigBuilder


object ARTHook {

    init {
        val code =ShadowHook.init(ConfigBuilder().setMode(ShadowHook.Mode.UNIQUE).setDebuggable(true).build())
        System.loadLibrary("art")
    }

    /**
     * A native method that is implemented by the 'art' native library,
     * which is packaged with this application.
     */
    external fun init()

}