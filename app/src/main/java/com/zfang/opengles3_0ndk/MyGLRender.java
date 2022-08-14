package com.zfang.opengles3_0ndk;

import static com.zfang.opengles3_0ndk.MyNativeRenderKt.SAMPLE_TYPE;

import android.opengl.GLSurfaceView;
import android.util.Log;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MyGLRender implements GLSurfaceView.Renderer {
    private static final String TAG = "MyGLRender";
    private MyNativeRender mNativeRender;
    private int mSampleType;

    MyGLRender() {
        mNativeRender = new MyNativeRender();
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
        mNativeRender.native_OnSurfaceCreated();
        Log.e(TAG, "onSurfaceCreated() called with: GL_VERSION = [" + gl.glGetString(GL10.GL_VERSION) + "]");
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        mNativeRender.native_OnSurfaceChanged(width, height);

    }

    @Override
    public void onDrawFrame(GL10 gl) {
        mNativeRender.native_OnDrawFrame();

    }

    public void init() {
        mNativeRender.native_OnInit();
    }

    public void unInit() {
        mNativeRender.native_OnUnInit();
    }

    public void setParamsInt(int paramType, int first, int second) {
        if (paramType == SAMPLE_TYPE) {
            mSampleType = first;
        }
        mNativeRender.nativeSetParamsInt(paramType, first, second);
    }

    public void setTouchLoc(float x, float y) {
//        mNativeRender.native_SetParamsFloat(SAMPLE_TYPE_SET_TOUCH_LOC, x, y);
    }

    public void setGravityXY(float x, float y) {
//        mNativeRender.native_SetParamsFloat(SAMPLE_TYPE_SET_GRAVITY_XY, x, y);
    }

    public void setImageData(int format, int width, int height, byte[] bytes) {
        mNativeRender.native_SetImageData(format, width, height, bytes);
    }

    public void setImageDataWithIndex(int index, int format, int width, int height, byte[] bytes) {
//        mNativeRender.native_SetImageDataWithIndex(index, format, width, height, bytes);
    }

    public void setAudioData(short[] audioData) {
//        mNativeRender.native_SetAudioData(audioData);
    }

    public int getSampleType() {
        return mSampleType;
    }

    public void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
//        mNativeRender.native_UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    }

}
