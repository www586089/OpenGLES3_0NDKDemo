//
// Created by Thinkpad on 2022/8/12.
//

#ifndef OPENGLES3_0NDKDEMO_VBOSAMPLE_H
#define OPENGLES3_0NDKDEMO_VBOSAMPLE_H

#include "../base/GLSampleBase.h"
#include "../utils/ImageDef.h"

class VBOSample: public GLSampleBase {
public:
    VBOSample();

    virtual ~VBOSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);
    virtual void destroy();

private:
    GLuint  mVBOIds[2];
};


#endif //OPENGLES3_0NDKDEMO_VBOSAMPLE_H
