//
// Created by Thinkpad on 2022/9/2.
//

#ifndef OPENGLES3_0NDKDEMO_INSTANCINGIDSAMPLE_H
#define OPENGLES3_0NDKDEMO_INSTANCINGIDSAMPLE_H

#include <GLES3/gl3.h>
#include <vec2.hpp>
#include "../base/GLSampleBase.h"
#include "Shader.h"

class InstancingIDSample: public GLSampleBase {

public:
    InstancingIDSample();
    virtual ~InstancingIDSample();
    virtual void loadImage(NativeImage *pImage);
    virtual void init();
    virtual void draw(int screenWidth, int screenHeight);
    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);
private:
    Shader shader;
    glm::vec2 translations[100];
    GLuint quadVAO, quadVBO;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 eyePosition;

    glm::mat4 mvpMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX = 0.3f;
    float m_ScaleY;
};


#endif //OPENGLES3_0NDKDEMO_INSTANCINGIDSAMPLE_H
