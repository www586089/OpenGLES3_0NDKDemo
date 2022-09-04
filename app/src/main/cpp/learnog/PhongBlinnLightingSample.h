//
// Created by Thinkpad on 2022/9/4.
//

#ifndef OPENGLES3_0NDKDEMO_PHONGBLINNLIGHTINGSAMPLE_H
#define OPENGLES3_0NDKDEMO_PHONGBLINNLIGHTINGSAMPLE_H


#include <ext/matrix_float4x4.hpp>
#include "LightingSampleBase.h"

//https://blog.csdn.net/wangxingxing321/article/details/107687019?spm=1001.2014.3001.5502

class PhongBlinnLightingSample : public LightingSampleBase {
public:
    PhongBlinnLightingSample();

    virtual ~PhongBlinnLightingSample();

    virtual void init();
    virtual void loadImage(NativeImage *pImage);
    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);
    virtual void changeStatus(int type, int flag);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    bool blinn = false;
    GLuint planeVAO, planeVBO;
    GLuint floorTexture;
    NativeImage floorImage;
    int m_AngleX;
    int m_AngleY;

    float m_ScaleX = 0.3f;
    float m_ScaleY;
};

#endif //OPENGLES3_0NDKDEMO_PHONGBLINNLIGHTINGSAMPLE_H
