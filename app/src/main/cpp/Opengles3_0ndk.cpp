#include <jni.h>
#include <stdio.h>
#include <EGLRender.h>
#include "render/MyGLRenderContext.h"

// Write C++ code here.
//
// Do not forget to dynamically load the C++ library into your application.
//
// For instance,
//
// In MainActivity.java:
//    static {
//       System.loadLibrary("opengles3_0ndk");
//    }
//
// Or, in MainActivity.kt:
//    companion object {
//      init {
//         System.loadLibrary("opengles3_0ndk")
//      }
//    }
extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_MyNativeRender_native_1OnInit(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::GetInstance();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_MyNativeRender_native_1OnUnInit(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::DestroyInstance();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_MyNativeRender_native_1SetImageData(JNIEnv *env, jobject thiz,
                                                                   jint format, jint width,
                                                                   jint height, jbyteArray imageData) {
    int len = env->GetArrayLength (imageData);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(imageData, 0, len, reinterpret_cast<jbyte*>(buf));
    printf("stx-----start");
    MyGLRenderContext::GetInstance()->setImageData(format, width, height, buf);
    printf("stx-----end");
    delete[] buf;
    env->DeleteLocalRef(imageData);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_MyNativeRender_native_1OnSurfaceCreated(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::GetInstance()->onSurfaceCreated();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_MyNativeRender_native_1OnSurfaceChanged(JNIEnv *env, jobject thiz,
                                                                       jint width, jint height) {
    MyGLRenderContext::GetInstance()->onSurfaceChanged(width, height);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_MyNativeRender_native_1OnDrawFrame(JNIEnv *env, jobject thiz) {
    MyGLRenderContext::GetInstance()->onDrawFrame();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_MyNativeRender_nativeSetParamsInt(JNIEnv *env, jobject thiz,
                                                                 jint type, jint first,
                                                                 jint second) {
    MyGLRenderContext::GetInstance()->setParamsInt(type, first, second);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_egl_NativeEglRender_nativeEglRenderInit(JNIEnv *env, jobject thiz) {
    EGLRender::GetInstance()->Init();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_egl_NativeEglRender_nativeEglRenderSetImageData(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jbyteArray data,
                                                                               jint width,
                                                                               jint height) {
    int len = env->GetArrayLength (data);
    uint8_t* buf = new uint8_t[len];
    env->GetByteArrayRegion(data, 0, len, reinterpret_cast<jbyte*>(buf));
    EGLRender::GetInstance()->SetImageData(buf, width, height);
    delete[] buf;
    env->DeleteLocalRef(data);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_egl_NativeEglRender_nativeEglRenderSetIntParams(JNIEnv *env,
                                                                               jobject thiz,
                                                                               jint type,
                                                                               jint param) {
    EGLRender::GetInstance()->SetIntParams(type, param);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_egl_NativeEglRender_nativeEglRenderDraw(JNIEnv *env, jobject thiz) {
    EGLRender::GetInstance()->Draw();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_zfang_opengles3_10ndk_egl_NativeEglRender_nativeEglRenderUnInit(JNIEnv *env,
                                                                         jobject thiz) {
    EGLRender::GetInstance()->UnInit();
}