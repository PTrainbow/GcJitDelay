package com.ptrain.art

import com.bytedance.shadowhook.ShadowHook
import com.bytedance.shadowhook.ShadowHook.ConfigBuilder


object ARTHook {

    init {

//        // 此处要考虑架构问题，shadowhook 只支持两种架构，示例工程先不管了
//        if (!Build.SUPPORTED_ABIS.contains("arm64-v8a") && !Build.SUPPORTED_ABIS.contains("armeabi-v7a")) {
//            Log.e("ARTHook", "unSupport ABI!")
//        }
        ShadowHook.init(
            ConfigBuilder().setMode(ShadowHook.Mode.UNIQUE).setDebuggable(true).build()
        )
        System.loadLibrary("art")
    }

    external fun init()

}