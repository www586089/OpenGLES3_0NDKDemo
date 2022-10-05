//
// Created by Thinkpad on 2022/10/5.
//

#ifndef OPENGLES3_0NDKDEMO_DEFERREDSHADINGSAMPLE_H
#define OPENGLES3_0NDKDEMO_DEFERREDSHADINGSAMPLE_H

#include <ext/matrix_float4x4.hpp>
#include "LightingSampleBase.h"
#include "ImageDef.h"

class DeferredShadingSample : public LightingSampleBase {
public:
    DeferredShadingSample();

    virtual ~DeferredShadingSample();

    virtual void init();
    virtual void loadImage(NativeImage *pImage);
    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);
    virtual void draw(int screenW, int screenH);
    virtual void renderQuad();
    virtual void renderCube();

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
    virtual void changeStatus(int type, int flag);
    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    Shader shader;
    Shader shaderLight;
    Shader shaderBlur;
    Shader shaderBloomFinal;
    bool firstFrame = true;
    GLuint woodTexture = -1, containerTexture = -1;
    GLuint quadVAO = 0, quadVBO = 0;
    GLuint cubeVAO = 0, cubeVBO = 0;

    GLuint hdrFBO = 0;
    GLuint rboDepth = 0;
    GLuint colorBuffers[2] = {0};

    GLuint pingpongFBO[2] = {0};
    GLuint pingpongColorBuffers[2] = {0};
    GLuint rboDepthBuffer = 0;
    int m_AngleX = 0;
    int m_AngleY = 0;
    NativeImage woodImage;
    NativeImage containerImage;

    float farthestLightPosZ = 20.0f;
    float m_ScaleX = 0.3f;
    float m_ScaleY;
};


#endif //OPENGLES3_0NDKDEMO_DEFERREDSHADINGSAMPLE_H
