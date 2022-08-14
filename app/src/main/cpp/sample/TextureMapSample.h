//
// Created by Thinkpad on 2022/8/12.
//

#ifndef OPENGLES3_0NDKDEMO_TEXTUREMAPSAMPLE_H
#define OPENGLES3_0NDKDEMO_TEXTUREMAPSAMPLE_H

#include "../base/GLSampleBase.h"

class TextureMapSample: public GLSampleBase {
public:
    TextureMapSample();
    virtual ~TextureMapSample();
    virtual void loadImage(NativeImage* pImage);
    virtual void init();
    virtual void draw(int screenWidth, int screenHeight);
    virtual void destroy();

private:
    GLuint mTextureId;
    GLint mSamplerLocation;
    NativeImage mRenderImage;
};

#endif //OPENGLES3_0NDKDEMO_TEXTUREMAPSAMPLE_H
