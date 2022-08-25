//
// Created by Thinkpad on 2022/8/24.
//

#include "LightingColorSample.h"

#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"

LightingColorSample::LightingColorSample() {

    lightingShader = GL_NONE;
    lightCubeShader = GL_NONE;
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

LightingColorSample::~LightingColorSample() {
    lightingShader = GL_NONE;
    lightCubeShader = GL_NONE;
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void LightingColorSample::init() {
    if (GL_NONE != lightingShader) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float r = 0.2f;
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
    lightingShader = GLUtils::CreateProgram(lightingVShaderStr, lightingFShaderStr, m_VertexShader, m_FragmentShader);
    lightCubeShader = GLUtils::CreateProgram(lightingCubeVShaderStr, lightingCubeFShaderStr, m_VertexShader, m_FragmentShader);

    if (lightingShader && lightCubeShader) {
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
        LOGE("LightingColorSample::Init create program fail");
        return;
    }
}



void LightingColorSample::draw(int screenW, int screenH) {
    LOGE("LightingColorSample::Draw()");

    if (lightingShader == GL_NONE || lightCubeShader == GL_NONE) {
        LOGE("LightingColorSample::Draw() return");
        return;
    }
    LOGE("LightingColorSample::Do Draw()");
    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // camera
    Camera camera(glm::vec3(0.0f, 0.0f, .0f));
    // lighting
    glm::vec3 lightPos(1.0f, 1.6f, 1.0f);

    // be sure to activate shader when setting uniforms/drawing objects
    glUseProgram(lightingShader);
    GLint objectColorLocation = glGetUniformLocation(lightingShader, "objectColor");
    glUniform3f(objectColorLocation, 1.0f, 0.5f, 0.31f);
    GLint lightColorLocation = glGetUniformLocation(lightingShader, "lightColor");
    glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);


    int angleX = m_AngleX;
    int angleY = m_AngleY;
    angleX = angleX % 360;
    angleY = angleY % 360;


    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // view/projection transformations
    glm::mat4 projection = glm::perspective(45.0f, (float) screenW / (float) screenH, 0.1f, 100.0f);
    // View matrix
    glm::vec3 eyePosition = glm::vec3 (1.0f, 1.5f, 3.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    glm::mat4 view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
    // view/projection transformations
    GLint projectionLocation = glGetUniformLocation(lightingShader, "projection");
    GLint viewLocation = glGetUniformLocation(lightingShader, "view");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    // world transformation
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    GLint modelLocation = glGetUniformLocation((GLint) lightingShader, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

    // render the cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);


    // also draw the lamp object
    glUseProgram(lightCubeShader);
    projectionLocation = glGetUniformLocation(lightCubeShader, "projection");
    viewLocation = glGetUniformLocation(lightCubeShader, "view");
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);

    // lighting
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f)); //绕Y轴旋转
    model = glm::translate(model, lightPos);

    modelLocation = glGetUniformLocation(lightCubeShader, "model");
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, &model[0][0]);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
}

void LightingColorSample::destroy() {
    if (GL_NONE != lightingShader) {
        glDeleteProgram(lightingShader);
        glDeleteProgram(lightCubeShader);

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
void LightingColorSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("LightingColorSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::vec3 eyePosition = glm::vec3 (2, 2, 2);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    glm::mat4 View = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = Projection * View * Model;
}

void LightingColorSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}
