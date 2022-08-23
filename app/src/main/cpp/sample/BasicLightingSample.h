//
// Created by Thinkpad on 2022/8/23.
//

#ifndef OPENGLES3_0NDKDEMO_BASICLIGHTINGSAMPLE_H
#define OPENGLES3_0NDKDEMO_BASICLIGHTINGSAMPLE_H


#include <detail/type_mat4x4.hpp>
#include "GLSampleBase.h"
#include "../utils/ImageDef.h"

class BasicLightingSample : public GLSampleBase
{
public:
    BasicLightingSample();

    virtual ~BasicLightingSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLint m_ModelMatrixLoc;
    GLint m_LightPosLoc;
    GLint m_LightColorLoc;
    GLint m_ViewPosLoc;

    GLuint m_VaoId;
    GLuint m_VboIds[1];
    GLuint m_TfoId;
    GLuint m_TfoBufId;
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLES3_0NDKDEMO_BASICLIGHTINGSAMPLE_H
