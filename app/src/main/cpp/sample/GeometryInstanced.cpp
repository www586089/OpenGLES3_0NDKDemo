//
// Created by Thinkpad on 2022/8/14.
//

#include "GeometryInstanced.h"
#include "../utils/GLUtils.h"
#include "../glm-0.9.9.8/ext/matrix_float4x4.hpp"


#define VERTEX_POS_SIZE        2 // x, y
#define VERTEX_COLOR_SIZE      3 // r, g, b

#define VERTEX_POS_INDEX       0 //shader layout loc
#define VERTEX_COLOR_INDEX     1 //shader layout loc

#define VERTEX_STRIDE         (sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE))

GeometryInstanced::GeometryInstanced() {
}

GeometryInstanced::~GeometryInstanced() {

}

void GeometryInstanced::loadImage(NativeImage *pImage) {
    //null implement

}

float GeometryInstanced::getRandomReal(float min, float max) {
    float fResult;

    if (min > max) {
        float temp = min;
        min = max;
        max = temp;
    }

    fResult = min + (max - min) * rand() / (RAND_MAX);

    LOGE("random, number = %f", fResult);
    return fResult;
}
/**
 * 可认为VAO简化了绘制时VBO的函数调用
 */
void GeometryInstanced::init() {
    const char vShaderStr[] =
            "#version 300 es                                                                  \n"
            "layout (location = 0) in vec2 aPos;                                              \n"
            "layout (location = 1) in vec3 aColor;                                            \n"
            "layout (location = 2) in vec2 scale;                                             \n"
            "layout (location = 3) in vec2 translation;                                       \n"
            "layout (location = 4) in vec4 rotation;                                          \n"
            "                                                                                 \n"
            "out vec3 v_color;                                                                \n"
            "                                                                                 \n"
            "void main()                                                                      \n"
            "{                                                                                \n"
            "    mat2 rotation = mat2(rotation.xy, rotation.zw);                              \n"
            "    gl_Position =  vec4(((rotation * aPos + translation) *  scale) , 0.0, 1.0);  \n"
            "                                                                                 \n"
            "    v_color = aColor;                                                            \n"
            "}";


    const char fShaderStr[] =
            "#version 300 es                                                                   \n"
            "precision mediump float;                                                          \n"
            "in vec3 v_color;                                                                  \n"
            "out vec4 o_fragColor;                                                             \n"
            "void main()                                                                       \n"
            "{                                                                                 \n"
            "    o_fragColor = vec4(v_color, 1.0);                                             \n"
            "}";

    // 4 vertices, with(x,y,z) ,(r, g, b, a) per-vertex
    GLfloat quadVertices[] = {
        -1.0f,  1.0f,
         1.0f, -1.0f,
        -1.0f, -1.0f,

        -1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f
    };

    GLfloat colors[] = {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,

        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f
    };
    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

    for (int i = 0; i < amount; i++) {
        randNumbers.push_back(getRandomReal(min, max) * amountSqrt);
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &TRANSFORM);

    glBindVertexArray(VAO);

   // config the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices) + sizeof(colors), nullptr, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quadVertices), quadVertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(quadVertices), sizeof(colors), colors);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(quadVertices)));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // config the TRANSFORM
    /**
     * 1 scale
     * 2 translation
     * 3 rotation
     */
    glBindBuffer(GL_ARRAY_BUFFER, TRANSFORM);
    GLsizeiptr size = amount * sizeof(glm::vec2) + amount * sizeof(glm::vec2) + amount * sizeof(glm::vec4);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STREAM_DRAW);

    unsigned int offset = 0;
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)offset);
    offset += (sizeof(glm::vec2) * amount);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)offset);
    offset += (sizeof(glm::vec2) * amount);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), (void*)offset);

    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);

    glVertexAttribDivisor(2, 1);
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);
}

void GeometryInstanced::draw(int screenW, int screenH) {
    if (m_ProgramObj == 0) {
        return;
    }
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_ProgramObj);

    glBindVertexArray(VAO);

    // 1. Scale
    glBindBuffer(GL_ARRAY_BUFFER, TRANSFORM);
    transformBfrPtr = glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(glm::vec2) * amount, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    for (int i = 0; i < amount; i++) {
        scale[i][0] = 1.0f / amountSqrt;
        scale[i][1] = 1.0f / amountSqrt;
    }
    LOGE("scale = %f", 1.0 / amountSqrt);
    memcpy(transformBfrPtr, scale, sizeof(scale[0]) * amount);
    glUnmapBuffer(GL_ARRAY_BUFFER);


    // 2. Translate
    transformBfrPtr = glMapBufferRange(GL_ARRAY_BUFFER, sizeof(glm::vec2) * amount, sizeof(glm::vec2) * amount, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    for (int i = 0; i < amount; i++) {
        translate[i][0] = randNumbers[i];
        translate[i][1] = randNumbers[amount - i];
    }
    std::memcpy(transformBfrPtr, translate, sizeof(translate[0]) * amount);
    glUnmapBuffer(GL_ARRAY_BUFFER);


    // 3. Rotation
    transformBfrPtr = glMapBufferRange(GL_ARRAY_BUFFER, sizeof(glm::vec2) * amount + sizeof(glm::vec2) * amount, sizeof(glm::vec4) * amount, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
    float var = clock() / 1000.0f;
    LOGE("var = %f", var);
    for (int i = 0; i < amount; i++) {
        rotate[i][0] = cos(var);
        rotate[i][1] = sin(var);
        rotate[i][2] = -sin(var);
        rotate[i][3] = cos(var);
    }
    std::memcpy(transformBfrPtr, rotate, sizeof(rotate[0]) * amount);
    glUnmapBuffer(GL_ARRAY_BUFFER);

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, amount);

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);
}

void GeometryInstanced::destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &TRANSFORM);

        m_ProgramObj = GL_NONE;
    }
}
