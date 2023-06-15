#include <jni.h>
#include <android/log.h>
#include <unistd.h>
#include "shadowhook.h"

#define LOG_TAG            "art-hook"
#define LOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##__VA_ARGS__)

// GC
void *originGC = nullptr;
void *gcStub = nullptr;

// JIT
void *originJit = nullptr;
void *jitStub = nullptr;

typedef void (*TaskRunType)(void *, void *);

void newJit(void *task, void *thread) {
    if (jitStub != nullptr) {
        LOGI("unhook jit task");
        shadowhook_unhook(jitStub);
    }
    LOGI("jit task sleep");
    sleep(5);
    LOGI("jit task wake up");
    ((TaskRunType) originJit)(task, thread);
    LOGI("jit task done");
}

void newGCDelay(void *task, void *thread) {
    if (gcStub != nullptr) {
        LOGI("unhook gc task");
        shadowhook_unhook(gcStub);
    }
    LOGI("gc task sleep");
    sleep(5);
    LOGI("gc task wake up");
    ((TaskRunType) originGC)(task, thread);
    LOGI("gc task done");
}

void jitDelay() {
    const char *jit = "_ZN3art3jit14JitCompileTask3RunEPNS_6ThreadE";
    jitStub = shadowhook_hook_sym_name("libart.so", jit,
                                       (void *) newJit,
                                       (void **) &originJit);
    if (jitStub != nullptr) {
        LOGI("hook JitCompileTask success");
    } else {
        LOGE("hook JitCompileTask failed");
    }
}

void gcDelay5X() {
    // Android 5.X 没有 ConcurrentGCTask, hook ConcurrentGC
    gcStub = shadowhook_hook_sym_name("libart.so",
                                      "_ZN3art2gc4Heap12ConcurrentGCEPNS_6ThreadE",
                                      (void *) newGCDelay,
                                      (void **) &originGC);
    if (gcStub != nullptr) {
        LOGI("hook ConcurrentGC success");
    } else {
        LOGE("hook ConcurrentGC failed");
    }
}

void gcDelay() {
    // Android 5.X 没有 ConcurrentGCTask, hook ConcurrentGC
    gcStub = shadowhook_hook_sym_name("libart.so",
                                      "_ZN3art2gc4Heap16ConcurrentGCTask3RunEPNS_6ThreadE",
                                      (void *) newGCDelay,
                                      (void **) &originGC);
    if (gcStub != nullptr) {
        LOGI("hook ConcurrentGCTask success");
    } else {
        LOGE("hook ConcurrentGCTask failed");
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_ptrain_art_ARTHook_init(JNIEnv *env, jobject thiz) {
    int androidApi = android_get_device_api_level();
    if (androidApi < __ANDROID_API_M__) { // Android 5.x 版本
        gcDelay5X();
    } else if (androidApi < 34) { // Android 6 - 13
        gcDelay();
    } else {
        LOGE("android api > 33, return!");
    }


    if (androidApi < __ANDROID_API_N__ || androidApi >= 34) { // Android  < 7  > 13 先不管
        LOGE("android api > 33 || < 24, return!");
        return;
    } else { // Android 7 - 13
        jitDelay();
    }
}