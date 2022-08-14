//
// Created by Thinkpad on 2022/7/9.
//
#include <GLES3/gl3.h>
#include "../base/GLSampleBase.h"

#ifndef OPENGLES3_0NDKDEMO_TRIANGLESAMPLE_H
#define OPENGLES3_0NDKDEMO_TRIANGLESAMPLE_H
class TriangleSample: public GLSampleBase {

public:
    TriangleSample();
    virtual ~TriangleSample();
    virtual void loadImage(NativeImage *pImage);
    virtual void init();
    virtual void draw(int screenWidth, int screenHeight);
    virtual void destroy();
};
#endif //OPENGLES3_0NDKDEMO_TRIANGLESAMPLE_H
