//
// Created by Thinkpad on 2022/9/2.
//

#ifndef OPENGLES3_0NDKDEMO_GLSLSAMPLE_H
#define OPENGLES3_0NDKDEMO_GLSLSAMPLE_H

#include <detail/type_mat4x4.hpp>
#include <model.h>
#include "GLSampleBase.h"
#include "../utils/ImageDef.h"
#include "Shader.h"

class GLSLSample : public GLSampleBase {
public:
    GLSLSample();

    virtual ~GLSLSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;
    Shader *shader;
    GLuint VBO, VAO;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

};



#endif //OPENGLES3_0NDKDEMO_GLSLSAMPLE_H
