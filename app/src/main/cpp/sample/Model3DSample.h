//
// Created by Thinkpad on 2022/8/28.
//

#ifndef OPENGLES3_0NDKDEMO_MODEL3DSAMPLE_H
#define OPENGLES3_0NDKDEMO_MODEL3DSAMPLE_H


#include <detail/type_mat4x4.hpp>
#include <shader.h>
#include <model.h>
#include "GLSampleBase.h"
#include "../utils/ImageDef.h"

class Model3DSample : public GLSampleBase {
public:
    Model3DSample();

    virtual ~Model3DSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;
    Shader *m_pShader;
    Model *m_pModel;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

};


#endif //OPENGLES3_0NDKDEMO_MODEL3DSAMPLE_H
