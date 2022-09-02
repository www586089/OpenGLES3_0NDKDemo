//
// Created by Thinkpad on 2022/9/2.
//

#include "UniformBufferSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"



//https://github.com/LearnOpenGL-CN/LearnOpenGL-CN/blob/new-theme/docs/04%20Advanced%20OpenGL/08%20Advanced%20GLSL.md

UniformBufferSample::UniformBufferSample() {
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    shader = nullptr;
}

UniformBufferSample::~UniformBufferSample() {

}

void UniformBufferSample::init() {
    if(shader != nullptr)
        return;
    // Point
//    float vertices[] = {
//            0.0f,  0.0f, 0.0f,
//            0.0f,  0.0, -2.0f,
//            0.0f,  0.0, -4.0f,
//            0.0f,  0.0, -6.0f,
//    };

    // Triangel
    float vertices[] = {
            0.0f,  0.5f, 0.0f,
            -0.5f,  0.0f, 0.0f,
            0.5f,  0.0f, 0.0f,

            0.0f,  0.5f, -0.5f,
            -0.5f,  0.0f, -0.5f,
            0.5f,  0.0f, -0.5f,

            0.0f,  0.5f, -1.0f,
            -0.5f,  0.0f, -1.0f,
            0.5f,  0.0f, -1.0f,

            0.0f,  0.5f, -1.5f,
            -0.5f,  0.0f, -1.5f,
            0.5f,  0.0f, -1.5f,

            0.0f,  0.5f, -4.5f,
            -0.5f,  0.0f, -4.5f,
            0.5f,  0.0f, -4.5f,

            0.0f,  0.5f, -6.5f,
            -0.5f,  0.0f, -6.5f,
            0.5f,  0.0f, -6.5f,
    };

    char vShaderStr[] =
            "#version 300 es                                   \n"
            "precision mediump float;                          \n"
            "layout (location = 0) in vec3 a_position;         \n"
            "uniform mat4 u_MVPMatrix;                         \n"
            "uniform mat4 u_ModelMatrix;                       \n"
            "void main()                                       \n"
            "{                                                 \n"
            "    vec4 position = vec4(a_position, 1.0);        \n"
            "    gl_Position = u_MVPMatrix * position;         \n"
            "    gl_PointSize = gl_Position.z;                 \n"
            "    vec3 fragPos = vec3(u_ModelMatrix * position);\n"
            "}";

    char fShaderStr[] =
            "#version 300 es                                   \n"
            "precision mediump float;                          \n"
            "out vec4 outColor;                                \n"
            "void main()                                       \n"
            "{                                                 \n"
            "    if (gl_FragCoord.x < 720.0) {                 \n"
            "        outColor = vec4(0.2, 0.3, 0.4, 1.0);      \n"
            "    } else {                                      \n"
            "        outColor = vec4(1.0, 0.0, 0.0, 1.0);      \n"
            "    }                                             \n"
            "                                                  \n"
            "}";

    shader = new Shader(vShaderStr, fShaderStr);

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindVertexArray(GL_NONE);
}

void UniformBufferSample::loadImage(NativeImage *pImage) {
    LOGE("UniformBufferSample::LoadImage pImage = %p", pImage->ppPlane[0]);
}

void UniformBufferSample::draw(int screenW, int screenH) {
    if(shader == nullptr) return;
    LOGE("UniformBufferSample::Draw()");
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);

    shader->use();
    shader->setMat4("u_MVPMatrix", m_MVPMatrix);
    shader->setMat4("u_ModelMatrix", m_ModelMatrix);


    glBindVertexArray(VAO);
    shader->setInt("texture_diffuse1", 0);
    glDrawArrays(GL_TRIANGLES, 0, 18);
    glBindVertexArray(GL_NONE);
}

void UniformBufferSample::destroy() {
    LOGE("UniformBufferSample::Destroy");

    if (shader != nullptr) {
        shader->Destroy();
        delete shader;
        shader = nullptr;

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void UniformBufferSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("UniformBufferSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
//    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, m_pModel->GetMaxViewDistance() * 4);
    glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    glm::vec3 eye = glm::vec3 (0.0f, 2.0f, 4.0f);
    // View matrix
    glm::mat4 View = glm::lookAt(
            eye, // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    m_ModelMatrix = Model;
    mvpMatrix = Projection * View * Model;
}

void UniformBufferSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    LOGE("UniformBufferSample::updateTransformMatrix, rotateX = %f", rotateX);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}