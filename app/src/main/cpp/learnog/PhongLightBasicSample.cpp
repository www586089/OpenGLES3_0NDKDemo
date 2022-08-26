//
// Created by Thinkpad on 2022/8/25.
//

#include "PhongLightBasicSample.h"

#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"
#include "Shader.h"

PhongLightBasicSample::PhongLightBasicSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

PhongLightBasicSample::~PhongLightBasicSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void PhongLightBasicSample::init() {
    if (lightingShader.isAvailable()) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float r = 0.5f;
    GLfloat vertices[] = {
            //vertex     normal
            -r, -r, -r,  0.0f,  0.0f, -1.0f,
             r, -r, -r,  0.0f,  0.0f, -1.0f,
             r,  r, -r,  0.0f,  0.0f, -1.0f,
             r,  r, -r,  0.0f,  0.0f, -1.0f,
            -r,  r, -r,  0.0f,  0.0f, -1.0f,
            -r, -r, -r,  0.0f,  0.0f, -1.0f,

            -r, -r,  r,  0.0f,  0.0f,  1.0f,
             r, -r,  r,  0.0f,  0.0f,  1.0f,
             r,  r,  r,  0.0f,  0.0f,  1.0f,
             r,  r,  r,  0.0f,  0.0f,  1.0f,
            -r,  r,  r,  0.0f,  0.0f,  1.0f,
            -r, -r,  r,  0.0f,  0.0f,  1.0f,

            -r,  r,  r, -1.0f,  0.0f,  0.0f,
            -r,  r, -r, -1.0f,  0.0f,  0.0f,
            -r, -r, -r, -1.0f,  0.0f,  0.0f,
            -r, -r, -r, -1.0f,  0.0f,  0.0f,
            -r, -r,  r, -1.0f,  0.0f,  0.0f,
            -r,  r,  r, -1.0f,  0.0f,  0.0f,

             r,  r,  r,  1.0f,  0.0f,  0.0f,
             r,  r, -r,  1.0f,  0.0f,  0.0f,
             r, -r, -r,  1.0f,  0.0f,  0.0f,
             r, -r, -r,  1.0f,  0.0f,  0.0f,
             r, -r,  r,  1.0f,  0.0f,  0.0f,
             r,  r,  r,  1.0f,  0.0f,  0.0f,

            -r, -r, -r,  0.0f, -1.0f,  0.0f,
             r, -r, -r,  0.0f, -1.0f,  0.0f,
             r, -r,  r,  0.0f, -1.0f,  0.0f,
             r, -r,  r,  0.0f, -1.0f,  0.0f,
            -r, -r,  r,  0.0f, -1.0f,  0.0f,
            -r, -r, -r,  0.0f, -1.0f,  0.0f,

            -r,  r, -r,  0.0f,  1.0f,  0.0f,
             r,  r, -r,  0.0f,  1.0f,  0.0f,
             r,  r,  r,  0.0f,  1.0f,  0.0f,
             r,  r,  r,  0.0f,  1.0f,  0.0f,
            -r,  r,  r,  0.0f,  1.0f,  0.0f,
            -r,  r, -r,  0.0f,  1.0f,  0.0f,
    };

    char lightingVShaderStr[] =
            "#version 300 es                                                   \n"
            "layout (location = 0) in vec3 aPos;                               \n"
            "layout (location = 1) in vec3 aNormal;                            \n"
            "                                                                  \n"
            "uniform mat4 model;                                               \n"
            "uniform mat4 view;                                                \n"
            "uniform mat4 projection;                                          \n"
            "                                                                  \n"
            "out vec3 FragPos;                                                 \n"
            "out vec3 Normal;                                                  \n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    FragPos = vec3(model * vec4(aPos, 1.0));                      \n"
            "    gl_Position = projection * view * vec4(FragPos, 1.0);         \n"
            "    Normal = aNormal;                                             \n"
            "}";

    char lightingFShaderStr[] =
            "#version 300 es                                         \n"
            "out vec4 FragColor;                                     \n"
            "                                                        \n"
            "uniform vec3 objectColor;                               \n"
            "uniform vec3 lightColor;                                \n"
            "uniform vec3 lightPos;                                  \n"
            "uniform vec3 viewPos;                                   \n"
            "                                                        \n"
            "in vec3 FragPos;                                        \n"
            "in vec3 Normal;                                         \n"
            "                                                        \n"
            "void main()                                             \n"
            "{                                                       \n"
            "    float ambientStrength = 0.1;                        \n"
            "    vec3 ambient = ambientStrength * lightColor;        \n"
            "                                                        \n"
            "    vec3 normal = normalize(Normal);                    \n"
            "    vec3 lightDir = normalize(lightPos - FragPos);      \n"
            "    float diff = max(dot(normal, lightDir), 0.0);       \n"
            "    vec3 diffuse = diff * lightColor;                   \n"
            "                                                        \n"
            "    float specularStrength = 0.5;                       \n"
            "    vec3 viewDir = normalize(viewPos - FragPos);                   \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);                  \n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);    \n"
            "    vec3 specular = specularStrength * spec * lightColor;          \n"
            "                                                                   \n"
            "    vec3 result = (ambient + diffuse + specular) * objectColor;    \n"
            "    FragColor = vec4(result, 1.0);                                 \n"
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
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        // first, configure the cube's VAO (and VBO)
        glGenVertexArrays(1, &cubeVAO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // position attribute
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) 0);
        // normal attribute
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (3 * sizeof (GLfloat)));
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);

        // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
        glGenVertexArrays(1, &lightCubeVAO);
        glBindVertexArray(lightCubeVAO);
        // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it;
        // the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) 0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);
    } else {
        LOGE("PhongLightBasicSample::Init create program fail");
        return;
    }
}



void PhongLightBasicSample::draw(int screenW, int screenH) {
    LOGE("PhongLightBasicSample::Draw()");

    if (!lightingShader.isAvailable() || !lightCubeShader.isAvailable()) {
        LOGE("PhongLightBasicSample::Draw() return");
        return;
    }
    LOGE("PhongLightBasicSample::Do Draw()");
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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
    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setVec3("viewPos", eyePosition);

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

void PhongLightBasicSample::destroy() {
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
void PhongLightBasicSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("PhongLightBasicSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    lightPos = glm::vec3(0.0f, 3.0f, 0.0f);//1.5f, 2.0f, 3.0f
    // View matrix
    eyePosition = glm::vec3 (1.8f, 2.5f, 3.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void PhongLightBasicSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}
