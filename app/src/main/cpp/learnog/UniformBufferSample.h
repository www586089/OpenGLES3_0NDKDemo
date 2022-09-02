//
// Created by Thinkpad on 2022/9/2.
//

#ifndef OPENGLES3_0NDKDEMO_UNIFORMBUFFERSAMPLE_H
#define OPENGLES3_0NDKDEMO_UNIFORMBUFFERSAMPLE_H


#include <detail/type_mat4x4.hpp>
#include <model.h>
#include "GLSampleBase.h"
#include "../utils/ImageDef.h"
#include "Shader.h"

class UniformBufferSample : public GLSampleBase {
public:
    UniformBufferSample();

    virtual ~UniformBufferSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    glm::mat4 m_MVPMatrix;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
    Shader *shaderRed, *shaderGreen, *shaderBlue, *shaderYellow;
    GLuint cubeVBO, cubeVAO;
    GLuint uboMatrices;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

};

#endif //OPENGLES3_0NDKDEMO_UNIFORMBUFFERSAMPLE_H
