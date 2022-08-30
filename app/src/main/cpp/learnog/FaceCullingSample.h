//
// Created by Thinkpad on 2022/8/30.
//

#ifndef OPENGLES3_0NDKDEMO_FACECULLINGSAMPLE_H
#define OPENGLES3_0NDKDEMO_FACECULLINGSAMPLE_H

#include <detail/type_mat4x4.hpp>
#include <AssimpShader.h>
#include <model.h>
#include "GLSampleBase.h"
#include "../utils/ImageDef.h"
#include "Shader.h"

class FaceCullingSample : public GLSampleBase {
public:
    FaceCullingSample();

    virtual ~FaceCullingSample();

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
    NativeImage cubeImage;
    GLuint cubeTextureId;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

};

#endif //OPENGLES3_0NDKDEMO_FACECULLINGSAMPLE_H
