//
// Created by Thinkpad on 2022/8/31.
//

#ifndef OPENGLES3_0NDKDEMO_CUBEMAPTEXTURESAMPLE_H
#define OPENGLES3_0NDKDEMO_CUBEMAPTEXTURESAMPLE_H


#include <ext/matrix_float4x4.hpp>
#include "GLSampleBase.h"
#include "Shader.h"

class CubeMapTextureSample : public GLSampleBase {
public:
    CubeMapTextureSample();

    virtual ~CubeMapTextureSample();

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    virtual void loadImage(NativeImage *pImage);
    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);
    GLuint loadCubemap();
private:
    Shader shader;
    Shader skyBoxShader;
    GLuint cubeVAO, cubeVBO;
    GLuint skyboxVAO, skyboxVBO;
    GLuint cubeTexture;
    GLuint cubemapTexture;
    NativeImage cubeImage;
    NativeImage floorImage;
    NativeImage skyBoxRenderImg[6];
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    bool firstFrame = true;

    glm::vec3 eyePosition;
    glm::vec3 lightPos;

    glm::mat4 mvpMatrix;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX = 0.3f;
    float m_ScaleY;
};

#endif //OPENGLES3_0NDKDEMO_CUBEMAPTEXTURESAMPLE_H
