//
// Created by Thinkpad on 2022/9/8.
//

#ifndef OPENGLES3_0NDKDEMO_PARALLAXMAPPINGSAMPLE_H
#define OPENGLES3_0NDKDEMO_PARALLAXMAPPINGSAMPLE_H


#include <ext/matrix_float4x4.hpp>
#include "LightingSampleBase.h"
#include "ImageDef.h"



//https://learnopengl.com/Advanced-Lighting/Parallax-Mapping
class ParallaxMappingSample : public LightingSampleBase {
public:
    ParallaxMappingSample();

    virtual ~ParallaxMappingSample();

    virtual void init();
    virtual void loadImage(NativeImage *pImage);
    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);
    virtual void draw(int screenW, int screenH);
    virtual void renderQuad();

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
    virtual void changeStatus(int type, int flag);
    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    Shader *pShader = NULL;
    bool normalMapping = true;
    glm::vec3 lightPos = glm::vec3(0.5f, 1.0f, 0.3f);
    GLuint diffuseMap, normalMap, depthMap;
    GLuint quadVAO = 0, quadVBO = 0;
    int m_AngleX;
    int m_AngleY;
    NativeImage normalImage, depthMapImage;
    Shader noNormalMappingShader;

    float m_ScaleX = 0.3f;
    float m_ScaleY;
};


#endif //OPENGLES3_0NDKDEMO_PARALLAXMAPPINGSAMPLE_H
