//
// Created by Thinkpad on 2022/9/2.
//

#include <iosfwd>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "InstancingIDSample.h"
#include "../utils/LogUtil.h"
#include "../utils/GLUtils.h"

InstancingIDSample::InstancingIDSample() = default;

InstancingIDSample::~InstancingIDSample() {

}

void InstancingIDSample::loadImage(NativeImage *pImage) {}

void InstancingIDSample::init() {
    float quadVertices[] = {
             // 位置          // 颜色
            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
            -0.05f, -0.05f,  0.0f, 0.0f, 1.0f,

            -0.05f,  0.05f,  1.0f, 0.0f, 0.0f,
             0.05f, -0.05f,  0.0f, 1.0f, 0.0f,
             0.05f,  0.05f,  0.0f, 1.0f, 1.0f
    };
    char vShaderStr[] =
            "#version 300 es                                   \n"
            "layout (location = 0) in vec2 aPos;               \n"
            "layout (location = 1) in vec3 aColor;             \n"
            "                                                  \n"
            "out vec3 fColor;                                  \n"
            "                                                  \n"
            "uniform vec2 offsets[100];                        \n"
            "uniform mat4 mvp;                                 \n"
            "                                                  \n"
            "void main()                                       \n"
            "{                                                 \n"
            "    vec2 offset = offsets[gl_InstanceID];         \n"
            "    vec4 position = vec4(aPos + offset, 0.0, 1.0);\n"
            "    gl_Position = mvp * position;                 \n"
            "    fColor = aColor;                              \n"
            "}";

    char fShaderStr[] =
            "#version 300 es                        \n"
            "out vec4 FragColor;                    \n"
            "                                       \n"
            "in vec3 fColor;                        \n"
            "                                       \n"
            "void main()                            \n"
            "{                                      \n"
            "    FragColor = vec4(fColor, 1.0);     \n"
            "}";

    shader = Shader(vShaderStr, fShaderStr);


    int index = 0;
    float offset = 0.1f;
    for (int y = -10; y < 10; y += 2) {
        for (int x = -10; x < 10; x += 2) {
            glm::vec2 translation;
            translation.x = (float) x / 10.0f + offset;
            translation.y = (float) y / 10.0f + offset;
            translations[index++] = translation;
        }
    }

    // quad VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);

    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    GLuint offsetByte = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offsetByte * sizeof(float)));

    offsetByte += 3;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offsetByte * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);
}

void InstancingIDSample::draw(int screenWidth, int screenHeight) {
    LOGE("InstancingIDSample::Draw");

    if (!shader.isAvailable()) {
        return;
    }

    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenWidth / (float) screenHeight);
    shader.use();
    shader.setMat4("mvp", mvpMatrix);
    for (unsigned int i = 0; i < 100; i++) {
        std::stringstream ss;
        std::string index;
        ss << i;
        index = ss.str();
        shader.setVec2("offsets[" + index + "]", translations[i]);
    }

    glBindVertexArray(quadVAO);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 100);
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void InstancingIDSample::UpdateMVPMatrix(glm::mat4 &mvpMatrixVar, int angleX, int angleY, float ratio) {
    LOGE("InstancingIDSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    // View matrix
    eyePosition = glm::vec3 (0.0f, 1.0f, 4.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );

    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrixVar = projection * view * model;
}

void InstancingIDSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}

void InstancingIDSample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}