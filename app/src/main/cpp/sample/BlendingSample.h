//
// Created by Thinkpad on 2022/8/30.
//

#ifndef OPENGLES3_0NDKDEMO_BLENDINGSAMPLE_H
#define OPENGLES3_0NDKDEMO_BLENDINGSAMPLE_H


#include <detail/type_mat4x4.hpp>
#include "GLSampleBase.h"
#include <vector>
#include <map>

#define RENDER_IMG_NUM 3

class BlendingSample : public GLSampleBase
{
public:
    BlendingSample();

    virtual ~BlendingSample();

    virtual void loadMultiImageWithIndex(int index, NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

    virtual void updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMatrix(glm::mat4 &mvpMatrix, int angleXRotate, int angleYRotate, float scale, glm::vec3 transVec3, float ratio);

private:
    GLuint m_TextureIds[RENDER_IMG_NUM];
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLuint m_VaoIds[3];
    GLuint m_VboIds[3];
    NativeImage m_RenderImages[RENDER_IMG_NUM];
    glm::mat4 m_MVPMatrix;

    std::vector<glm::vec3> windowsTrans;
    std::map<GLfloat, glm::vec3> sorted;

    int m_AngleX;
    int m_AngleY;

};


#endif //OPENGLES3_0NDKDEMO_BLENDINGSAMPLE_H
