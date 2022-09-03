//
// Created by Thinkpad on 2022/9/3.
//

#ifndef OPENGLES3_0NDKDEMO_INSTANCINGASTEROIDARRAYSAMPLE_H
#define OPENGLES3_0NDKDEMO_INSTANCINGASTEROIDARRAYSAMPLE_H


#include <detail/type_mat4x4.hpp>
#include <AssimpShader.h>
#include <model.h>
#include "GLSampleBase.h"
#include "../utils/ImageDef.h"

class InstancingAsteroidArraySample : public GLSampleBase {
public:
    InstancingAsteroidArraySample();

    virtual ~InstancingAsteroidArraySample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    int amount = 10000;

    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;
    glm::mat4 *modelMatrices;
    glm::mat4 view;
    glm::mat4 projection;
    AssimpShader *m_pShader;
    AssimpShader *asteroidsShader;
    Model *planet;
    Model *rock;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

};

#endif //OPENGLES3_0NDKDEMO_INSTANCINGASTEROIDARRAYSAMPLE_H
