//
// Created by Thinkpad on 2022/8/12.
//

#ifndef OPENGLES3_0NDKDEMO_NV21TEXTUREMAPSAMPLE_H
#define OPENGLES3_0NDKDEMO_NV21TEXTUREMAPSAMPLE_H

#include "../base/GLSampleBase.h"
#include "../utils/ImageDef.h"

class NV21TextureMapSample : public GLSampleBase {
public:
    NV21TextureMapSample() {
        m_yTextureId = GL_NONE;
        m_uvTextureId = GL_NONE;

        m_ySamplerLoc = GL_NONE;
        m_uvSamplerLoc = GL_NONE;
    }

    virtual ~NV21TextureMapSample() {
        NativeImageUtil::FreeNativeImage(&mRenderImage);
    }

    virtual void loadImage(NativeImage *pImage);
    virtual void init();
    virtual void draw(int screenWidth, int screenHeight);
    virtual void destroy();

private:
    GLuint m_yTextureId;
    GLuint m_uvTextureId;

    GLint m_ySamplerLoc;
    GLint m_uvSamplerLoc;

    NativeImage mRenderImage;
};

#endif //OPENGLES3_0NDKDEMO_NV21TEXTUREMAPSAMPLE_H
