//
// Created by Thinkpad on 2022/8/26.
//

#ifndef OPENGLES3_0NDKDEMO_PHONGMATERIALSSAMPLE_H
#define OPENGLES3_0NDKDEMO_PHONGMATERIALSSAMPLE_H

//https://blog.csdn.net/wangxingxing321/article/details/107636902?spm=1001.2014.3001.5502
#include <ext/matrix_float4x4.hpp>
#include "LightingSampleBase.h"

class PhongMaterialsSample : public LightingSampleBase {
public:
    PhongMaterialsSample();

    virtual ~PhongMaterialsSample();

    virtual void init();
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



#endif //OPENGLES3_0NDKDEMO_PHONGMATERIALSSAMPLE_H
