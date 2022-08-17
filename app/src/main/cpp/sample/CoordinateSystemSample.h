//
// Created by Thinkpad on 2022/8/17.
//

#ifndef OPENGLES3_0NDKDEMO_COORDINATESYSTEMSAMPLE_H
#define OPENGLES3_0NDKDEMO_COORDINATESYSTEMSAMPLE_H


#include <detail/type_mat4x4.hpp>
#include "../base/GLSampleBase.h"

class CoordinateSystemSample : public GLSampleBase {
public:
    CoordinateSystemSample();

    virtual ~CoordinateSystemSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX = 0;
    int m_AngleY = 0;
    float m_ScaleX;
    float m_ScaleY;

};


#endif //OPENGLES3_0NDKDEMO_COORDINATESYSTEMSAMPLE_H
