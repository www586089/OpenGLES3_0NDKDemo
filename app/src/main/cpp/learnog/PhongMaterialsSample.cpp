//
// Created by Thinkpad on 2022/8/26.
//

#include "PhongMaterialsSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"
#include "Shader.h"

PhongMaterialsSample::PhongMaterialsSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

PhongMaterialsSample::~PhongMaterialsSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void PhongMaterialsSample::init() {
    if (lightingShader.isAvailable()) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    GLfloat r = 0.5f;
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
            "struct Material {\n"
            "    vec3 ambient;\n"
            "    vec3 diffuse;\n"
            "    vec3 specular;\n"
            "    float shininess;\n"
            "}; "
            "out vec4 FragColor;                                     \n"
            "                                                        \n"
            "uniform vec3 objectColor;                               \n"
            "uniform vec3 lightColor;                                \n"
            "uniform vec3 lightPos;                                  \n"
            "uniform vec3 viewPos;                                   \n"
            "uniform Material material;\n"
            "                                                        \n"
            "in vec3 FragPos;                                        \n"
            "in vec3 Normal;                                         \n"
            "                                                        \n"
            "void main()                                             \n"
            "{                                                       \n"
            "    float ambientStrength = 0.1;                        \n"
            "    vec3 ambient = lightColor * material.ambient;        \n"
            "                                                        \n"
            "    vec3 normal = normalize(Normal);                    \n"
            "    vec3 lightDir = normalize(lightPos - FragPos);      \n"
            "    float diff = max(dot(normal, lightDir), 0.0);       \n"
            "    vec3 diffuse = material.diffuse * diff * lightColor;                   \n"
            "                                                        \n"
            "    float specularStrength = 0.5;                       \n"
            "    vec3 viewDir = normalize(viewPos - FragPos);                   \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);                  \n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0),  material.shininess);    \n"
            "    vec3 specular =  material.specular * spec * lightColor;          \n"
            "                                                                   \n"
            "    vec3 result = (ambient + diffuse + specular) * objectColor;    \n"
            "    FragColor = vec4(result, 1.0);                                 \n"
            "}\n"
            ;

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
    GLchar const * varyings[] = {"Normal"};
    lightingShader = Shader(lightingVShaderStr, lightingFShaderStr, varyings, sizeof(varyings) / sizeof(varyings[0]));
    lightCubeShader = Shader(lightingCubeVShaderStr, lightingCubeFShaderStr);

    if (lightingShader.isAvailable() && lightCubeShader.isAvailable()) {

        //创建 Transform Feedback 缓存类似于创建 VBO
        glGenBuffers(1, &m_TransFeedbackBufId);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_TransFeedbackBufId);
        glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 3 * 6 * 6 * sizeof(GLfloat), NULL, GL_STATIC_READ);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        //为 Transform Feedback 对象绑定缓冲区对象
        glGenTransformFeedbacks(1, &m_TransFeedbackObjId);
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransFeedbackObjId);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_TransFeedbackBufId); // Specify the index of the binding point within the array specified by target.
        glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        // first, configure the cube's VAO (and VBO)
        glGenVertexArrays(1, &cubeVAO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // position attribute
        glEnableVertexAttribArray(0);
        GLuint offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (offset * sizeof(GLfloat)));
        // normal attribute
        glEnableVertexAttribArray(1);
        offset += 3;
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*) (offset * sizeof (GLfloat)));
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
        LOGE("PhongMaterialsSample::Init create program fail");
        return;
    }
}



void PhongMaterialsSample::draw(int screenW, int screenH) {
    LOGE("PhongMaterialsSample::Draw()");

    if (!lightingShader.isAvailable() || !lightCubeShader.isAvailable()) {
        LOGE("PhongMaterialsSample::Draw() return");
        return;
    }
    LOGE("PhongMaterialsSample::Do Draw()");
    glEnable(GL_DEPTH_TEST);
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
    // set materials
    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("material.shininess", 32.0f);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    lightingShader.setMat4("model", model);
    // view/projection transformations
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    // render the cube
    glBindVertexArray(cubeVAO);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TransFeedbackObjId);
    glBeginTransformFeedback(GL_TRIANGLES);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glEndTransformFeedback();
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, GL_NONE);
    glBindVertexArray(GL_NONE);

    // Read feedback buffer
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, m_TransFeedbackBufId);
    void* rawData = glMapBufferRange(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 3 * 6 * 6 * sizeof(GLfloat), GL_MAP_READ_BIT);

    float *p = (float*) rawData;
    for(int i= 0; i < 36; i++) {
        LOGE("PhongMaterialsSample::Draw() read feedback buffer Normal[%d] = [%f, %f, %f]",
             i, p[i * 3], p[i * 3 + 1], p[i * 3 + 2]);
    }

    glUnmapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);

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

void PhongMaterialsSample::destroy() {
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
void PhongMaterialsSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("PhongMaterialsSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float lightX = 2.8f * sin(radiansY);
    float lightZ = 2.8f * cos(radiansY);
    lightPos = glm::vec3(lightX, 3.0f, lightZ);//1.5f, 2.0f, 3.0f
    // View matrix
    eyePosition = glm::vec3 (1.2f, 2.5f, 2.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void PhongMaterialsSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}