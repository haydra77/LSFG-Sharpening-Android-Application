// Mob-FGSR Lab: Android software frame-blend pipeline (prototype).
// This is a real intermediate-frame output path, but NOT motion-compensated
// Mob-FGSR. It blends adjacent captured frames at t=0.5 as a bring-up stage.
// Mob-FGSR-inspired motion estimation / reprojection is a later stage.
#include <jni.h>
#include <android/hardware_buffer.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <vector>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "MobFGSRNative", __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, "MobFGSRNative", __VA_ARGS__)

namespace {
std::mutex gMutex;
ANativeWindow* gWindow = nullptr;
int gWidth = 0, gHeight = 0;
bool gBypass = false;
std::vector<uint8_t> gPrevious;
bool gHasPrevious = false;
std::atomic<jlong> gGenerated{0}, gPosted{0}, gUnique{0};
std::atomic<jlong> gLastPostNs{0};

bool postPixels(const uint8_t* src, int srcW, int srcH, int srcStridePixels) {
    if (!gWindow || !src || srcW <= 0 || srcH <= 0) return false;
    ANativeWindow_Buffer out{};
    if (ANativeWindow_lock(gWindow, &out, nullptr) != 0) return false;
    auto* dst = static_cast<uint8_t*>(out.bits);
    const int dstW = out.width, dstH = out.height, dstStride = out.stride;
    if (!dst || dstW <= 0 || dstH <= 0) {
        ANativeWindow_unlockAndPost(gWindow);
        return false;
    }
    for (int y = 0; y < dstH; ++y) {
        const int sy = std::min(srcH - 1, (int)((int64_t)y * srcH / dstH));
        const uint8_t* srow = src + (size_t)sy * srcStridePixels * 4;
        uint8_t* drow = dst + (size_t)y * dstStride * 4;
        for (int x = 0; x < dstW; ++x) {
            const int sx = std::min(srcW - 1, (int)((int64_t)x * srcW / dstW));
            std::memcpy(drow + (size_t)x * 4, srow + (size_t)sx * 4, 4);
        }
    }
    if (ANativeWindow_unlockAndPost(gWindow) != 0) return false;
    gPosted.fetch_add(1, std::memory_order_relaxed);
    return true;
}

bool postBlend(const uint8_t* a, const uint8_t* b, size_t bytes, int w, int h) {
    static thread_local std::vector<uint8_t> blend;
    blend.resize(bytes);
    // Midpoint temporal blend. Alpha is opaque; RGB channels are averaged.
    const size_t pixels = (size_t)w * h;
    for (size_t i = 0; i < pixels; ++i) {
        const size_t p = i * 4;
        blend[p + 0] = (uint8_t)(((unsigned)a[p + 0] + b[p + 0] + 1) >> 1);
        blend[p + 1] = (uint8_t)(((unsigned)a[p + 1] + b[p + 1] + 1) >> 1);
        blend[p + 2] = (uint8_t)(((unsigned)a[p + 2] + b[p + 2] + 1) >> 1);
        blend[p + 3] = 255;
    }
    return postPixels(blend.data(), w, h, w);
}
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_lsfg_android_session_NativeBridge_nativeVersion(JNIEnv* env, jobject) {
    return env->NewStringUTF("mobfgsr-android-blend-prototype-1");
}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_initCrashReporter(JNIEnv*, jobject, jstring, jstring) {}
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_extractShaders(JNIEnv*, jobject, jstring, jstring, jstring) { return -1; }
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_probeShaders(JNIEnv*, jobject, jstring) { return -1; }

extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_initContext(
    JNIEnv*, jobject, jstring, jint width, jint height, jint,
    jfloat, jboolean, jboolean, jboolean, jboolean, jboolean,
    jint, jint, jfloat, jfloat, jfloat, jboolean, jboolean,
    jint, jfloat, jfloat, jfloat, jfloat, jboolean, jint, jint,
    jfloat, jfloat, jfloat, jint, jfloat, jfloat, jfloat, jint) {
    std::lock_guard<std::mutex> lock(gMutex);
    if (!gWindow || width <= 0 || height <= 0) {
        LOGW("initContext: output Surface missing or invalid dimensions");
        return -20;
    }
    gWidth = width;
    gHeight = height;
    gPrevious.clear();
    gHasPrevious = false;
    gGenerated.store(0);
    gPosted.store(0);
    gUnique.store(0);
    ANativeWindow_setBuffersGeometry(gWindow, width, height, WINDOW_FORMAT_RGBA_8888);
    LOGI("Blend prototype ready: %dx%d", width, height);
    return 0;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_lsfg_android_session_NativeBridge_isNpuAvailable(JNIEnv*, jobject) { return JNI_FALSE; }
extern "C" JNIEXPORT jboolean JNICALL
Java_com_lsfg_android_session_NativeBridge_isFramegenFp16Supported(JNIEnv*, jobject, jstring) { return JNI_FALSE; }
extern "C" JNIEXPORT jstring JNICALL
Java_com_lsfg_android_session_NativeBridge_getNpuSummary(JNIEnv* env, jobject) {
    return env->NewStringUTF("CPU midpoint blend prototype; no NPU path");
}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setOutputSurface(JNIEnv* env, jobject, jobject surface, jint w, jint h) {
    std::lock_guard<std::mutex> lock(gMutex);
    ANativeWindow* next = surface ? ANativeWindow_fromSurface(env, surface) : nullptr;
    if (gWindow) ANativeWindow_release(gWindow);
    gWindow = next;
    if (w > 0 && h > 0) {
        gWidth = w; gHeight = h;
        if (gWindow) ANativeWindow_setBuffersGeometry(gWindow, w, h, WINDOW_FORMAT_RGBA_8888);
    }
    LOGI("Output Surface %s (%dx%d)", gWindow ? "attached" : "detached", w, h);
}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_pushFrame(JNIEnv* env, jobject, jobject hardwareBuffer, jlong) {
    if (!hardwareBuffer) return;
    std::lock_guard<std::mutex> lock(gMutex);
    if (!gWindow) return;
    AHardwareBuffer* hb = AHardwareBuffer_fromHardwareBuffer(env, hardwareBuffer);
    if (!hb) return;
    AHardwareBuffer_Desc desc{};
    AHardwareBuffer_describe(hb, &desc);
    if (desc.format != AHARDWAREBUFFER_FORMAT_R8G8B8A8_UNORM &&
        desc.format != AHARDWAREBUFFER_FORMAT_R8G8B8X8_UNORM) {
        LOGW("Unsupported AHardwareBuffer format=%u", desc.format);
        return;
    }
    void* pixels = nullptr;
    const int lockRc = AHardwareBuffer_lock(
        hb, AHARDWAREBUFFER_USAGE_CPU_READ_OFTEN, -1, nullptr, &pixels);
    if (lockRc != 0 || !pixels) {
        LOGW("AHardwareBuffer CPU lock failed rc=%d (capture reader must allow CPU read)", lockRc);
        return;
    }
    const int w = (int)desc.width, h = (int)desc.height, stride = (int)desc.stride;
    const size_t bytes = (size_t)w * h * 4;
    std::vector<uint8_t> current(bytes);
    const auto* src = static_cast<const uint8_t*>(pixels);
    for (int y = 0; y < h; ++y)
        std::memcpy(current.data() + (size_t)y * w * 4, src + (size_t)y * stride * 4, (size_t)w * 4);
    AHardwareBuffer_unlock(hb, nullptr);

    if (gHasPrevious && gPrevious.size() == bytes && !gBypass) {
        if (postBlend(gPrevious.data(), current.data(), bytes, w, h))
            gGenerated.fetch_add(1, std::memory_order_relaxed);
    }
    if (postPixels(current.data(), w, h, w)) {
        bool changed = !gHasPrevious || gPrevious.size() != bytes ||
                       std::memcmp(gPrevious.data(), current.data(), bytes) != 0;
        if (changed) gUnique.fetch_add(1, std::memory_order_relaxed);
    }
    gPrevious.swap(current);
    gHasPrevious = true;
}

extern "C" JNIEXPORT jlong JNICALL
Java_com_lsfg_android_session_NativeBridge_getGeneratedFrameCount(JNIEnv*, jobject) { return gGenerated.load(); }
extern "C" JNIEXPORT jlong JNICALL
Java_com_lsfg_android_session_NativeBridge_getPostedFrameCount(JNIEnv*, jobject) { return gPosted.load(); }
extern "C" JNIEXPORT jlong JNICALL
Java_com_lsfg_android_session_NativeBridge_getUniqueCaptureCount(JNIEnv*, jobject) { return gUnique.load(); }
extern "C" JNIEXPORT jdouble JNICALL
Java_com_lsfg_android_session_NativeBridge_getAverageQueueMs(JNIEnv*, jobject) { return 0.0; }
extern "C" JNIEXPORT jdouble JNICALL
Java_com_lsfg_android_session_NativeBridge_getAverageLatencyMs(JNIEnv*, jobject) { return 0.0; }
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_getRecentPostIntervalsNs(JNIEnv*, jobject, jlongArray) { return 0; }
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_getProfileWindowNs(JNIEnv*, jobject, jlongArray) { return 0; }
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setBypass(JNIEnv*, jobject, jboolean bypass) {
    std::lock_guard<std::mutex> lock(gMutex);
    gBypass = bypass == JNI_TRUE;
}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setAntiArtifacts(JNIEnv*, jobject, jboolean) {}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setVsyncPeriodNs(JNIEnv*, jobject, jlong) {}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setPacingParams(JNIEnv*, jobject, jint, jfloat, jfloat, jfloat, jint) {}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setShizukuTimingEnabled(JNIEnv*, jobject, jboolean) {}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_reportShizukuTiming(JNIEnv*, jobject, jlong, jlong, jlong) {}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_destroyContext(JNIEnv*, jobject) {
    std::lock_guard<std::mutex> lock(gMutex);
    gPrevious.clear();
    gHasPrevious = false;
    gWidth = gHeight = 0;
}
