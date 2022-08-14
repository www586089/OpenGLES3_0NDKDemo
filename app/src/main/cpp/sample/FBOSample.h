//
// Created by Thinkpad on 2022/8/14.
//

#ifndef OPENGLES3_0NDKDEMO_FBOSAMPLE_H
#define OPENGLES3_0NDKDEMO_FBOSAMPLE_H

#include "../base/GLSampleBase.h"
#include "../utils/ImageDef.h"

class FBOSample: public GLSampleBase {
public:
    FBOSample();

    virtual ~FBOSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    bool CreateFrameBufferObj();
    void GO_CHECK_GL_ERROR() {}

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2];
    GLuint m_VboIds[4];
    GLint m_SamplerLoc;
    NativeImage mRenderImage;
    GLuint m_FboProgramObj;
    GLuint m_FboVertexShader;
    GLuint m_FboFragmentShader;
    GLint m_FboSamplerLoc;
};


#endif //OPENGLES3_0NDKDEMO_FBOSAMPLE_H
