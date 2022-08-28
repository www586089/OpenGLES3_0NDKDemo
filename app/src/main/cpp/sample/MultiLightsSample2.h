//
// Created by Thinkpad on 2022/8/28.
//

#ifndef OPENGLES3_0NDKDEMO_MULTILIGHTSSAMPLE2_H
#define OPENGLES3_0NDKDEMO_MULTILIGHTSSAMPLE2_H

#include <detail/type_mat4x4.hpp>
#include "GLSampleBase.h"
#include "../utils/ImageDef.h"

class MultiLightsSample2 : public GLSampleBase {
public:
    MultiLightsSample2();

    virtual ~MultiLightsSample2();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

    void UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix, int angleXRotate, int angleYRotate, float scale, glm::vec3 transVec3, float ratio);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLint m_ModelMatrixLoc;
    GLint m_ViewPosLoc;

    GLuint m_VaoId;
    GLuint m_VboIds[1];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;

    int m_AngleX;
    int m_AngleY;
};


#endif //OPENGLES3_0NDKDEMO_MULTILIGHTSSAMPLE2_H
