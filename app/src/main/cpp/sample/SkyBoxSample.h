//
// Created by Thinkpad on 2022/9/1.
//

#ifndef OPENGLES3_0NDKDEMO_SKYBOXSAMPLE_H
#define OPENGLES3_0NDKDEMO_SKYBOXSAMPLE_H


#include <detail/type_mat4x4.hpp>
#include "../base/GLSampleBase.h"
#include "../utils/ImageDef.h"

class SkyBoxSample : public GLSampleBase
{
public:
    SkyBoxSample();

    virtual ~SkyBoxSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void init();

    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float scale, float ratio);

private:
    GLuint m_TextureId;

    GLuint m_CubeProgramObj;

    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLint m_CubeSamplerLoc;
    GLint m_CubeMVPMatLoc;
    GLint m_CubeModelMatLoc;
    GLint m_ViewPosLoc;

    GLuint m_CubeVaoId;
    GLuint m_CubeVboId;
    GLuint m_SkyBoxVaoId;
    GLuint m_SkyBoxVboId;

    NativeImage m_RenderImage;
    NativeImage m_pSkyBoxRenderImg[6];

    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLES3_0NDKDEMO_SKYBOXSAMPLE_H
