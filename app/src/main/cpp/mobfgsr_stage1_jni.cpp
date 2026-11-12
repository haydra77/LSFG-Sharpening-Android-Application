// Temporary JNI compatibility layer for Mob-FGSR Stage 1.
// This deliberately disables LSFG. It keeps the existing Kotlin app loadable
// while Stage 1 validates MediaProjection/ImageReader capture at 720p.
// Replace these stubs with the Mob-FGSR processing bridge in later stages.
#include <jni.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_lsfg_android_session_NativeBridge_nativeVersion(JNIEnv* env, jobject) {
    return env->NewStringUTF("mobfgsr-capture-stage1 (LSFG disabled)");
}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_initCrashReporter(JNIEnv*, jobject, jstring, jstring) {}
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_extractShaders(JNIEnv*, jobject, jstring, jstring, jstring) { return -1; }
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_probeShaders(JNIEnv*, jobject, jstring) { return -1; }
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_initContext(JNIEnv*, jobject, jstring, jint, jint, jint, jfloat, jboolean, jboolean, jboolean, jboolean, jboolean, jint, jint, jfloat, jfloat, jfloat, jboolean, jboolean, jint, jfloat, jfloat, jfloat, jfloat, jboolean, jint, jint, jfloat, jfloat, jfloat, jint, jfloat, jfloat, jfloat, jint) { return -90; }
extern "C" JNIEXPORT jboolean JNICALL
Java_com_lsfg_android_session_NativeBridge_isNpuAvailable(JNIEnv*, jobject) { return JNI_FALSE; }
extern "C" JNIEXPORT jboolean JNICALL
Java_com_lsfg_android_session_NativeBridge_isFramegenFp16Supported(JNIEnv*, jobject, jstring) { return JNI_FALSE; }
extern "C" JNIEXPORT jstring JNICALL
Java_com_lsfg_android_session_NativeBridge_getNpuSummary(JNIEnv* env, jobject) { return env->NewStringUTF("Disabled in Mob-FGSR Stage 1"); }
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setOutputSurface(JNIEnv*, jobject, jobject, jint, jint) {}
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_pushFrame(JNIEnv*, jobject, jobject, jlong) {}
extern "C" JNIEXPORT jlong JNICALL
Java_com_lsfg_android_session_NativeBridge_getGeneratedFrameCount(JNIEnv*, jobject) { return 0; }
extern "C" JNIEXPORT jlong JNICALL
Java_com_lsfg_android_session_NativeBridge_getPostedFrameCount(JNIEnv*, jobject) { return 0; }
extern "C" JNIEXPORT jlong JNICALL
Java_com_lsfg_android_session_NativeBridge_getUniqueCaptureCount(JNIEnv*, jobject) { return 0; }
extern "C" JNIEXPORT jdouble JNICALL
Java_com_lsfg_android_session_NativeBridge_getAverageQueueMs(JNIEnv*, jobject) { return 0.0; }
extern "C" JNIEXPORT jdouble JNICALL
Java_com_lsfg_android_session_NativeBridge_getAverageLatencyMs(JNIEnv*, jobject) { return 0.0; }
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_getRecentPostIntervalsNs(JNIEnv*, jobject, jlongArray) { return 0; }
extern "C" JNIEXPORT jint JNICALL
Java_com_lsfg_android_session_NativeBridge_getProfileWindowNs(JNIEnv*, jobject, jlongArray) { return 0; }
extern "C" JNIEXPORT void JNICALL
Java_com_lsfg_android_session_NativeBridge_setBypass(JNIEnv*, jobject, jboolean) {}
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
Java_com_lsfg_android_session_NativeBridge_destroyContext(JNIEnv*, jobject) {}
