//
// Created by Thinkpad on 2022/8/24.
//

#ifndef OPENGLES3_0NDKDEMO_LIGHTINGSAMPLEBASE_H
#define OPENGLES3_0NDKDEMO_LIGHTINGSAMPLEBASE_H

#include <GLSampleBase.h>

class LightingSampleBase: public GLSampleBase {

protected:
    GLuint lightingShader;
    GLuint lightCubeShader;
    unsigned int VBO, cubeVAO;
    unsigned int lightCubeVAO;

    GLint modelLocation;
    GLint viewLocation;
    GLint projectionLocation;
};


#endif //OPENGLES3_0NDKDEMO_LIGHTINGSAMPLEBASE_H
