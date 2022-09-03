//
// Created by Thinkpad on 2022/9/3.
//

#include "InstancingArraySample.h"
#include <iosfwd>
#include <string>
#include <gtc/matrix_transform.hpp>
#include "../utils/LogUtil.h"
#include "../utils/GLUtils.h"
using namespace std;

InstancingArraySample::InstancingArraySample() = default;

InstancingArraySample::~InstancingArraySample() {

}

void InstancingArraySample::loadImage(NativeImage *pImage) {}

void InstancingArraySample::init() {
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
            "#version 300 es                                    \n"
            "layout (location = 0) in vec2 aPos;                \n"
            "layout (location = 1) in vec3 aColor;              \n"
            "layout (location = 2) in vec2 aOffset;             \n"
            "                                                   \n"
            "out vec3 fColor;                                   \n"
            "                                                   \n"
            "uniform vec2 offsets[100];                         \n"
            "uniform mat4 mvp;                                  \n"
            "                                                   \n"
            "void main()                                        \n"
            "{                                                  \n"
            "    vec4 position = vec4(aPos + aOffset, 0.0, 1.0);\n"
            "    gl_Position = mvp * position;                  \n"
            "    fColor = aColor;                               \n"
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

    // translate VBO
    glGenBuffers(1, &translateVBO);
    glBindBuffer(GL_ARRAY_BUFFER, translateVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 100, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // quad VBO
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    // quad VAO
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    GLuint offsetByte = 0;
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offsetByte * sizeof(float)));
    offsetByte += 3;
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offsetByte * sizeof(float)));

    glBindBuffer(GL_ARRAY_BUFFER, translateVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*) 0);
    /**
     * 这个函数告诉了OpenGL该什么时候更新顶点属性的内容至新一组数据。它的第一个参数是需要的顶点属性，
     * 第二个参数是属性除数(Attribute Divisor)。默认情况下，属性除数是0，告诉OpenGL我们需要在
     * 顶点着色器的每次迭代时更新顶点属性。将它设置为1时，我们告诉OpenGL我们希望在渲染一个新实例的
     * 时候更新顶点属性。而设置为2时，我们希望每2个实例更新一次属性，以此类推。我们将属性除数设置为1，
     * 是在告诉OpenGL，处于位置值2的顶点属性是一个实例化数组。
     */
    glVertexAttribDivisor(2, 1); // tell OpenGL this is an instanced vertex attribute.

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);
}

void InstancingArraySample::draw(int screenWidth, int screenHeight) {
    LOGE("InstancingArraySample::Draw");

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
    glBindVertexArray(GL_NONE);
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void InstancingArraySample::UpdateMVPMatrix(glm::mat4 &mvpMatrixVar, int angleX, int angleY, float ratio) {
    LOGE("InstancingArraySample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
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
    eyePosition = glm::vec3 (0.0f, 1.0f, 2.0f);
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

void InstancingArraySample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}

void InstancingArraySample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();
        glDeleteVertexArrays(1, &quadVAO);
        glDeleteBuffers(1, &quadVBO);
    }
}
