//
// Created by Thinkpad on 2022/8/27.
//

#ifndef OPENGLES3_0NDKDEMO_PHONGLIGHTTEXTURESAMPLE_H
#define OPENGLES3_0NDKDEMO_PHONGLIGHTTEXTURESAMPLE_H


#include <ext/matrix_float4x4.hpp>
#include "LightingSampleBase.h"

class PhongLightTextureSample : public LightingSampleBase {
public:
    PhongLightTextureSample();

    virtual ~PhongLightTextureSample();

    virtual void init();
    virtual void loadImage(NativeImage *pImage);
    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX = 0.3f;
    float m_ScaleY;
};



#endif //OPENGLES3_0NDKDEMO_PHONGLIGHTTEXTURESAMPLE_H
