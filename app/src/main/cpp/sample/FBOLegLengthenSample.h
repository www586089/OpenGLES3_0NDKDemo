//
// Created by Thinkpad on 2022/8/16.
//

#ifndef OPENGLES3_0NDKDEMO_FBOLEGLENGTHENSAMPLE_H
#define OPENGLES3_0NDKDEMO_FBOLEGLENGTHENSAMPLE_H



#include "../glm-0.9.9.8/detail/type_mat4x4.hpp"
#include "../base/GLSampleBase.h"
#include "../utils/ImageDef.h"

#define VERTICAL_STRETCH_8_POINTS          0x10
#define VERTICAL_STRETCH_TOP_6_POINTS      0x11
#define VERTICAL_STRETCH_BOTTOM_6_POINTS   0x12
#define VERTICAL_STRETCH_4_POINTS          0x13

#define HORIZONTAL_STRETCH_8_POINTS        0x20
#define HORIZONTAL_STRETCH_LEFT_6_POINTS   0x21
#define HORIZONTAL_STRETCH_RIGHT_6_POINTS  0x22
#define HORIZONTAL_STRETCH_4_POINTS        0x23

class FBOLegLengthenSample : public GLSampleBase {
public:
    FBOLegLengthenSample();

    virtual ~FBOLegLengthenSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    bool CreateFrameBufferObj();

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2] = {GL_NONE};;
    GLuint m_VboIds[6] = {GL_NONE};;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;
    GLuint m_FboProgramObj;
    GLuint m_FboVertexShader;
    GLuint m_FboFragmentShader;
    GLint m_FboSamplerLoc;

    glm::mat4 m_FboMVPMatrix;
    GLint m_FboMVPMatLoc;

    glm::mat4 m_MVPMatrix;
    GLint m_MVPMatLoc;

    GLfloat m_dt;
    bool    m_isgo;

    RectF m_StretchRect;
    int   m_StretchMode;

    bool  m_bIsVerticalMode;
};


#endif //OPENGLES3_0NDKDEMO_FBOLEGLENGTHENSAMPLE_H
