//
// Created by Thinkpad on 2022/9/5.
//

#ifndef OPENGLES3_0NDKDEMO_SHADOWMAPPINGDEPTHSAMPLE_H
#define OPENGLES3_0NDKDEMO_SHADOWMAPPINGDEPTHSAMPLE_H


//https://blog.csdn.net/wangxingxing321/article/details/107688212?spm=1001.2014.3001.5502
#include <ext/matrix_float4x4.hpp>
#include "GLSampleBase.h"
#include "Shader.h"

class ShadowMappingDepthSample : public GLSampleBase {
public:
    ShadowMappingDepthSample();

    virtual ~ShadowMappingDepthSample();

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    virtual void loadImage(NativeImage *pImage);
    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);
private:
    Shader shader;
    GLuint cubeVAO, cubeVBO;
    GLuint planeVAO, planeVBO;
    GLuint transparentVAO, transparentVBO;
    GLuint cubeTexture, floorTexture, grassTexture;
    NativeImage cubeImage;
    NativeImage floorImage;
    NativeImage grassImage;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 eyePosition;
    glm::vec3 lightPos;

    glm::mat4 mvpMatrix;

    GLint m_SamplerLoc;

    GLuint m_VaoId;
    GLuint m_VboIds[1];

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX = 0.3f;
    float m_ScaleY;
};
#endif //OPENGLES3_0NDKDEMO_SHADOWMAPPINGDEPTHSAMPLE_H
