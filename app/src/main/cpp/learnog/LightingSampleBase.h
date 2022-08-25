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
    unsigned int VBO, cubeVAO;
    unsigned int lightCubeVAO;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    glm::mat4 mvpMatrix;
};


#endif //OPENGLES3_0NDKDEMO_LIGHTINGSAMPLEBASE_H
