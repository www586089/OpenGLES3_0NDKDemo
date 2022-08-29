//
// Created by Thinkpad on 2022/8/29.
//

#include "DepthBufferTestSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"
#include "Shader.h"

DepthBufferTestSample::DepthBufferTestSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

DepthBufferTestSample::~DepthBufferTestSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void DepthBufferTestSample::init() {
    if (lightingShader.isAvailable()) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float r = 0.4f;
    GLfloat vertices[] = {
            -r, -r, -r,
             r, -r, -r,
             r,  r, -r,
             r,  r, -r,
            -r,  r, -r,
            -r, -r, -r,

            -r, -r,  r,
             r, -r,  r,
             r,  r,  r,
             r,  r,  r,
            -r,  r,  r,
            -r, -r,  r,

            -r,  r,  r,
            -r,  r, -r,
            -r, -r, -r,
            -r, -r, -r,
            -r, -r,  r,
            -r,  r,  r,

             r,  r,  r,
             r,  r, -r,
             r, -r, -r,
             r, -r, -r,
             r, -r,  r,
             r,  r,  r,

            -r, -r, -r,
             r, -r, -r,
             r, -r,  r,
             r, -r,  r,
            -r, -r,  r,
            -r, -r, -r,

            -r,  r, -r,
             r,  r, -r,
             r,  r,  r,
             r,  r,  r,
            -r,  r,  r,
            -r,  r, -r,
    };

    char lightingVShaderStr[] =
            "#version 300 es                                                   \n"
            "layout (location = 0) in vec3 aPos;                               \n"
            "                                                                  \n"
            "uniform mat4 model;                                               \n"
            "uniform mat4 view;                                                \n"
            "uniform mat4 projection;                                          \n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);    \n"
            "}";

    char lightingFShaderStr[] =
            "#version 300 es                                         \n"
            "out vec4 FragColor;                                     \n"
            "                                                        \n"
            "uniform vec3 objectColor;                               \n"
            "uniform vec3 lightColor;                                \n"
            "                                                        \n"
            "void main()                                             \n"
            "{                                                       \n"
            "    FragColor = vec4(lightColor * objectColor, 1.0);    \n"
            "}";
    char lightingCubeVShaderStr[] =
            "#version 300 es                                                   \n"
            "layout (location = 0) in vec3 aPos;                               \n"
            "                                                                  \n"
            "uniform mat4 model;                                               \n"
            "uniform mat4 view;                                                \n"
            "uniform mat4 projection;                                          \n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);    \n"
            "}";
    char lightingCubeFShaderStr[] =
            "#version 300 es                                                  \n"
            "out vec4 FragColor;                                              \n"
            "                                                                 \n"
            "void main()                                                      \n"
            "{                                                                \n"
            "    FragColor = vec4(1.0); // set alle 4 vector values to 1.0    \n"
            "}";
    lightingShader = Shader(lightingVShaderStr, lightingFShaderStr);
    lightCubeShader = Shader(lightingCubeVShaderStr, lightingCubeFShaderStr);

    if (lightingShader.isAvailable() && lightCubeShader.isAvailable()) {
        // first, configure the cube's VAO (and VBO)
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &VBO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindVertexArray(cubeVAO);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
        glGenVertexArrays(1, &lightCubeVAO);
        glBindVertexArray(lightCubeVAO);
        // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it;
        // the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
    } else {
        LOGE("DepthBufferTestSample::Init create program fail");
        return;
    }
}



void DepthBufferTestSample::draw(int screenW, int screenH) {
    LOGE("DepthBufferTestSample::Draw()");

    if (!lightingShader.isAvailable() || !lightCubeShader.isAvailable()) {
        LOGE("DepthBufferTestSample::Draw() return");
        return;
    }
    LOGE("DepthBufferTestSample::Do Draw()");
    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, .0f));
    // lighting
    glm::vec3 lightPos(2.8f, 2.8f, 1.0f);


    int angleX = m_AngleX;
    int angleY = m_AngleY;
    angleX = angleX % 360;
    angleY = angleY % 360;
    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // view/projection transformations
    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);
    // be sure to activate shader when setting uniforms/drawing objects
    lightingShader.use();
    lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    lightingShader.setMat4("model", model);
    // view/projection transformations
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);


    // also draw the lamp object
    lightCubeShader.use();
    // lighting
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f)); //绕Y轴旋转
    model = glm::translate(model, lightPos);
    lightCubeShader.setMat4("model", model);
    lightCubeShader.setMat4("view", view);
    lightCubeShader.setMat4("projection", projection);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
}

void DepthBufferTestSample::destroy() {
    if (lightingShader.isAvailable()) {
        lightingShader.deleteProgram();
        lightCubeShader.deleteProgram();

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightCubeVAO);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void DepthBufferTestSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("DepthBufferTestSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    // View matrix
    glm::vec3 eyePosition = glm::vec3 (1.0f, 2.5f, 3.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void DepthBufferTestSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}