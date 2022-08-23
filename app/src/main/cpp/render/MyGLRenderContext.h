//
// Created by Thinkpad on 2022/7/9.
//
#include "../sample/TriangleSample.h"
#include "../base/GLSampleBase.h"

#ifndef OPENGLES3_0NDKDEMO_MYGLRENDERCONTEXT_H
#define OPENGLES3_0NDKDEMO_MYGLRENDERCONTEXT_H

class MyGLRenderContext {
public :
    TriangleSample mSample;
    //TextureMapSample* m_TextureSample;
    NativeImage m_NativeImage;

//    static int const IMAGE_FORMAT_RGBA = 0x01;
//    static int const IMAGE_FORMAT_NV21 = 0x02;
//    static int const IMAGE_FORMAT_NV12 = 0x03;
//    static int const IMAGE_FORMAT_I420 = 0x04;

    MyGLRenderContext();
    ~MyGLRenderContext();

    void setImageData(int format, int width, int height, unsigned char *pData);

    void onSurfaceCreated();
    void onSurfaceChanged(int width, int height);
    void onDrawFrame();

    void setParamsInt(int type, int first, int second);
    void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    static MyGLRenderContext *GetInstance();
    static void DestroyInstance();

private:
    static MyGLRenderContext *m_pContext;
    GLSampleBase *pPreviousSample = nullptr;
    GLSampleBase *pCurrentSample = nullptr;
    int mScreenWidth;
    int mScreenHeight;
};
#endif //OPENGLES3_0NDKDEMO_MYGLRENDERCONTEXT_H
