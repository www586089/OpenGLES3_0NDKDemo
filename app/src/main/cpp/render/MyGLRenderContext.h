//
// Created by Thinkpad on 2022/7/9.
//
#include "../sample/TriangleSample.h"
#include "../base/GLSampleBase.h"

#ifndef OPENGLES3_0NDKDEMO_MYGLRENDERCONTEXT_H
#define OPENGLES3_0NDKDEMO_MYGLRENDERCONTEXT_H

class MyGLRenderContext {
public :
    MyGLRenderContext();
    ~MyGLRenderContext();

    void setImageData(int format, int width, int height, unsigned char *pData);
    void setImageDataWithIndex(int index, int format, int width, int height, uint8_t *pData);

    void onSurfaceCreated();
    void onSurfaceChanged(int width, int height);
    void onDrawFrame();

    void setParamsInt(int type, int first, int second);
    void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
    void changeStatus(int type, int flag);

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
