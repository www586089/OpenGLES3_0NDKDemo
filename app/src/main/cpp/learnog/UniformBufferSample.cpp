//
// Created by Thinkpad on 2022/9/2.
//

#include "UniformBufferSample.h"
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.inl>
#include "../utils/GLUtils.h"



//https://github.com/LearnOpenGL-CN/LearnOpenGL-CN/blob/new-theme/docs/04%20Advanced%20OpenGL/08%20Advanced%20GLSL.md

UniformBufferSample::UniformBufferSample() {
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    shaderRed = nullptr;
}

UniformBufferSample::~UniformBufferSample() {

}

void UniformBufferSample::init() {
    if(shaderRed != nullptr)
        return;
    float cubeVertices[] = {
            // positions         
            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,

            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f, -0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,

             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,

            -0.5f, -0.5f, -0.5f,
             0.5f, -0.5f, -0.5f,
             0.5f, -0.5f,  0.5f,
             0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f,  0.5f,
            -0.5f, -0.5f, -0.5f,

            -0.5f,  0.5f, -0.5f,
             0.5f,  0.5f, -0.5f,
             0.5f,  0.5f,  0.5f,
             0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f,  0.5f,
            -0.5f,  0.5f, -0.5f,
    };

    char vShaderStr[] =
            "#version 300 es                                 \n"
            "layout (location = 0) in vec3 aPos;             \n"
            "                                                \n"
            "layout (std140) uniform Matrices                \n"
            "{                                               \n"
            "    mat4 projection;                            \n"
            "    mat4 view;                                  \n"
            "};                                              \n"
            "uniform mat4 model;                             \n"
            "                                                \n"
            "void main()                                     \n"
            "{                                               \n"
            "    mat4 mvpMatrix = projection * view * model; \n"
            "    gl_Position = mvpMatrix * vec4(aPos, 1.0);  \n"
            "}";

    char fShaderRedStr[] =
            "#version 300 es                                 \n"
            "precision mediump float;                        \n"
            "out vec4 FragColor;                             \n"
            "                                                \n"
            "void main()                                     \n"
            "{                                               \n"
            "    FragColor = vec4(1.0, 0.0, 0.0, 1.0);       \n"
            "}";
    char fShaderGreenStr[] =
            "#version 300 es                                 \n"
            "precision mediump float;                        \n"
            "out vec4 FragColor;                             \n"
            "                                                \n"
            "void main()                                     \n"
            "{                                               \n"
            "    FragColor = vec4(0.0, 1.0, 0.0, 1.0);       \n"
            "}";
    char fShaderBlueStr[] =
            "#version 300 es                                 \n"
            "precision mediump float;                        \n"
            "out vec4 FragColor;                             \n"
            "                                                \n"
            "void main()                                     \n"
            "{                                               \n"
            "    FragColor = vec4(0.0, 0.0, 1.0, 1.0);       \n"
            "}";
    char fShaderYellowStr[] =
            "#version 300 es                                 \n"
            "precision mediump float;                        \n"
            "out vec4 FragColor;                             \n"
            "                                                \n"
            "void main()                                     \n"
            "{                                               \n"
            "    FragColor = vec4(1.0, 1.0, 0.0, 1.0);       \n"
            "}";

    shaderRed = new Shader(vShaderStr, fShaderRedStr);
    shaderGreen = new Shader(vShaderStr, fShaderGreenStr);
    shaderBlue = new Shader(vShaderStr, fShaderBlueStr);
    shaderYellow = new Shader(vShaderStr, fShaderYellowStr);

    // cube VAO
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // configure a uniform buffer object
    // ---------------------------------
    // first. We get the relevant block indices
    GLuint uniformBlockIndexRed = glGetUniformBlockIndex(shaderRed->getProgram(), "Matrices");
    GLuint uniformBlockIndexGreen = glGetUniformBlockIndex(shaderGreen->getProgram(), "Matrices");
    GLuint uniformBlockIndexBlue = glGetUniformBlockIndex(shaderBlue->getProgram(), "Matrices");
    GLuint uniformBlockIndexYellow = glGetUniformBlockIndex(shaderYellow->getProgram(), "Matrices");

    // then we link each shader's uniform block to this uniform binding point
    glUniformBlockBinding(shaderRed->getProgram(), uniformBlockIndexRed, 0);
    glUniformBlockBinding(shaderGreen->getProgram(), uniformBlockIndexGreen, 0);
    glUniformBlockBinding(shaderBlue->getProgram(), uniformBlockIndexBlue, 0);
    glUniformBlockBinding(shaderYellow->getProgram(), uniformBlockIndexYellow, 0);

    // Now actually create the buffer
    glGenBuffers(1, &uboMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    // define the range of the buffer that links to a uniform binding point
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

}

void UniformBufferSample::loadImage(NativeImage *pImage) {
    LOGE("UniformBufferSample::LoadImage pImage = %p", pImage->ppPlane[0]);
}

void UniformBufferSample::draw(int screenW, int screenH) {
    if(shaderRed == nullptr) return;
    LOGE("UniformBufferSample::Draw()");
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);
    float angleX = m_AngleX % 360;
    float angleY = m_AngleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    // draw 4 cubes
    // RED
    glBindVertexArray(cubeVAO);
    shaderRed->use();
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-0.75f, 0.75f, 0.0f)); // move top-left
    shaderRed->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // GREEN
    shaderGreen->use();
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.75f, 0.75f, 0.0f)); // move top-right
    shaderGreen->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // YELLOW
    shaderYellow->use();
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-0.75f, -0.75f, 0.0f)); // move bottom-left
    shaderYellow->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    // BLUE
    shaderBlue->use();
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.75f, -0.75f, 0.0f)); // move bottom-right
    shaderBlue->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void UniformBufferSample::destroy() {
    LOGE("UniformBufferSample::Destroy");

    if (shaderRed != nullptr) {
        shaderRed->Destroy();
        delete shaderRed;
        shaderRed = nullptr;

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
    }
}

void UniformBufferSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("UniformBufferSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);

    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
//    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, m_pModel->GetMaxViewDistance() * 4);
    projection = glm::perspective(45.0f, ratio, 0.1f,100.f);

    glm::vec3 eye = glm::vec3 (0.0f, 2.0f, 4.0f);
    // View matrix
    view = glm::lookAt(
            eye, // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    model = glm::mat4(1.0f);
    mvpMatrix = projection * view * model;

    // store the projection matrix (we only do this once now) (note: we're not using zoom anymore by changing the FoV)
    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBufferSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    LOGE("UniformBufferSample::updateTransformMatrix, rotateX = %f", rotateX);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}