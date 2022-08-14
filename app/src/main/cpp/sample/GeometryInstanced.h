//
// Created by Thinkpad on 2022/8/14.
//

#ifndef OPENGLES3_0NDKDEMO_GEOMETRYINSTANCED_H
#define OPENGLES3_0NDKDEMO_GEOMETRYINSTANCED_H

#include <cmath>
#include <vector>
#include "../base//GLSampleBase.h"
#include "../glm-0.9.9.8/vec2.hpp"
#include "../glm-0.9.9.8/vec4.hpp"

using namespace std;

class GeometryInstanced : public GLSampleBase {
public:
    GeometryInstanced();

    virtual ~GeometryInstanced();
    virtual void loadImage(NativeImage *pImage);
    virtual void init();
    virtual void draw(int screenW, int screenH);
    virtual void destroy();

    virtual float getRandomReal(float min, float max);


private:
    GLuint VAO;
    GLuint VBO;
    GLuint TRANSFORM;
    float max = 1.0;
    float min = -1.0;

    void *transformBfrPtr = nullptr;

    GLsizei amount = 9;
    vector<float> randNumbers;

    float amountSqrt = sqrt(amount);
    glm::vec2 *scale = new glm::vec2[amount];
    glm::vec2 *translate = new glm::vec2[amount];
    glm::vec4 *rotate = new glm::vec4[amount];

};


#endif //OPENGLES3_0NDKDEMO_GEOMETRYINSTANCED_H
