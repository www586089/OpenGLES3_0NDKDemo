//
// Created by Thinkpad on 2022/8/24.
//

#ifndef OPENGLES3_0NDKDEMO_LIGHTINGSAMPLEBASE_H
#define OPENGLES3_0NDKDEMO_LIGHTINGSAMPLEBASE_H

#include <GLSampleBase.h>
#include "Shader.h"

class LightingSampleBase: public GLSampleBase {

public:
    LightingSampleBase() {

    }
    virtual ~LightingSampleBase() {

    }

protected:
    Shader lightingShader;
    Shader lightCubeShader;
    GLuint VBO, cubeVAO;
    GLuint lightCubeVAO;
    GLuint m_TransFeedbackObjId;
    GLuint m_TransFeedbackBufId;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::vec3 eyePosition;
    glm::vec3 lightPos;

    glm::mat4 mvpMatrix;

    NativeImage diffuseImage;
    NativeImage specularImage;
    GLuint textureDiffuse;  //漫反射贴图
    GLuint textureSpecular;//高光贴图
};


#endif //OPENGLES3_0NDKDEMO_LIGHTINGSAMPLEBASE_H
