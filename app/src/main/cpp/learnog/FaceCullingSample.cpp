//
// Created by Thinkpad on 2022/8/30.
//

#include "FaceCullingSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"

FaceCullingSample::FaceCullingSample() {
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    shader = nullptr;
}

FaceCullingSample::~FaceCullingSample() {

}

void FaceCullingSample::init() {
    if(shader != nullptr)
        return;
    float vertices[] = {
            // back face CCW      //VCoordinate //normal
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f, // bottom-left
             0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f,  0.0f, -1.0f, // bottom-right
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f, // top-right
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  0.0f, -1.0f, // top-right
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  0.0f, -1.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f,  0.0f, -1.0f, // bottom-left
            // front face CW
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f, // top-right
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  0.0f,  1.0f, // bottom-right
             0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f,  0.0f,  1.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  0.0f,  1.0f, // bottom-left
            -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f,  0.0f,  1.0f, // top-left
            // left face CCW
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f, 0.0f, // top-right
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
            -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, -1.0f,  0.0f, 0.0f, // top-left
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, -1.0f,  0.0f, 0.0f, // bottom-left
            -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, -1.0f,  0.0f, 0.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, -1.0f,  0.0f, 0.0f, // bottom-right
            // right face CW
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f, 0.0f, // top-left
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f,  0.0f, 0.0f, // top-right
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f, 0.0f, // bottom-right
             0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f,  0.0f, 0.0f, // bottom-right
             0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f,  0.0f, 0.0f, // bottom-left
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f,  0.0f, 0.0f, // top-left
            // bottom face CCW
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // top-right
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, // bottom-left
             0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f, // top-left
             0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, // bottom-left
            -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f, // top-right
            -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f, // bottom-right
            // top face CW
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f, // top-left
             0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f,  1.0f, 0.0f, // top-right
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
             0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f,  1.0f, 0.0f, // bottom-right
            -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f, // bottom-left
            -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f,  1.0f, 0.0f, // top-left
    };

/* Also make sure to add a call to OpenGL to specify that triangles defined by a clockwise ordering
   are now 'front-facing' triangles so the cube is rendered as normal:
   glFrontFace(GL_CW);
*/
    char vShaderStr[] =
            "#version 300 es                                   \n"
            "precision mediump float;                          \n"
            "layout (location = 0) in vec3 a_position;         \n"
            "layout (location = 1) in vec3 a_normal;           \n"
            "layout (location = 2) in vec2 a_texCoord;         \n"
            "out vec2 v_texCoord;                              \n"
            "uniform mat4 u_MVPMatrix;                         \n"
            "uniform mat4 u_ModelMatrix;                       \n"
            "uniform vec3 lightPos;                            \n"
            "uniform vec3 lightColor;                          \n"
            "uniform vec3 viewPos;                             \n"
            "out vec3 ambient;                                 \n"
            "out vec3 diffuse;                                 \n"
            "out vec3 specular;                                \n"
            "void main()                                                                    \n"
            "{                                                                              \n"
            "    v_texCoord = a_texCoord;                                                   \n"
            "    vec4 position = vec4(a_position, 1.0);                                     \n"
            "    gl_Position = u_MVPMatrix * position;                                      \n"
            "    vec3 fragPos = vec3(u_ModelMatrix * position);                             \n"
            "                                                                               \n"
            "    // Ambient                                                                 \n"
            "    float ambientStrength = 0.45;                                              \n"
            "    ambient = ambientStrength * lightColor;                                    \n"
            "                                                                               \n"
            "    // Diffuse                                                                 \n"
            "    float diffuseStrength = 0.8;                                               \n"
            "    vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));    \n"
            "    vec3 lightDir = normalize(lightPos - fragPos);                             \n"
            "    float diff = max(dot(unitNormal, lightDir), 0.0);                          \n"
            "    diffuse = diffuseStrength * diff * lightColor;                             \n"
            "                                                                               \n"
            "    // Specular                                                                \n"
            "    float specularStrength = 0.3;                                              \n"
            "    vec3 viewDir = normalize(viewPos - fragPos);                               \n"
            "    vec3 reflectDir = reflect(-lightDir, unitNormal);                          \n"
            "    float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 64.0);             \n"
            "    specular = specularStrength * spec * lightColor;                           \n"
            "}";

    char fShaderStr[] =
            "#version 300 es                                                              \n"
            "precision mediump float;                                                     \n"
            "out vec4 outColor;                                                           \n"
            "in vec2 v_texCoord;                                                          \n"
            "in vec3 ambient;                                                             \n"
            "in vec3 diffuse;                                                             \n"
            "in vec3 specular;                                                            \n"
            "uniform sampler2D texture_diffuse1;                                          \n"
            "void main()                                                                  \n"
            "{                                                                            \n"
            "    vec4 objectColor = texture(texture_diffuse1, v_texCoord);                \n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);    \n"
            "    outColor = vec4(finalColor, 1.0);                                        \n"
            "}";

    char fNoTextureShaderStr[] =
            "#version 300 es                                                            \n"
            "precision highp float;                                                     \n"
            "out vec4 outColor;                                                         \n"
            "in vec3 ambient;                                                           \n"
            "in vec3 diffuse;                                                           \n"
            "in vec3 specular;                                                          \n"
            "void main()                                                                \n"
            "{                                                                          \n"
            "    vec4 objectColor = vec4(0.6, 0.6, 0.6, 1.0);                           \n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);  \n"
            "    outColor = vec4(finalColor, 1.0);                                      \n"
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void *) 0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void *) (5* sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (const void *) (3* sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindVertexArray(GL_NONE);

    //create RGBA texture
    glGenTextures(1, &cubeTextureId);
    glBindTexture(GL_TEXTURE_2D, cubeTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void FaceCullingSample::loadImage(NativeImage *pImage) {
    LOGE("FaceCullingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        cubeImage.width = pImage->width;
        cubeImage.height = pImage->height;
        cubeImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &cubeImage);
    }
}

void FaceCullingSample::draw(int screenW, int screenH) {
    if(shader == nullptr) return;
    LOGE("FaceCullingSample::Draw()");
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);//开启面踢除功能
    glCullFace(GL_BACK);   //剔除背面
    glFrontFace(GL_CCW);    //GL_CW：正面为顺时针方向  GL_CCW:正面为逆时针

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cubeImage.width, cubeImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    shader->use();
    shader->setMat4("u_MVPMatrix", m_MVPMatrix);
    shader->setMat4("u_ModelMatrix", m_ModelMatrix);
    shader->setVec3("lightPos", glm::vec3(-0.5, 1.5, 0.0f));
    shader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    shader->setVec3("viewPos", glm::vec3(0, 0, 2.0f));


    glBindVertexArray(VAO);
    glBindTexture(GL_TEXTURE_2D, cubeTextureId);
    shader->setInt("texture_diffuse1", 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
}

void FaceCullingSample::destroy() {
    LOGE("FaceCullingSample::Destroy");

    if (shader != nullptr) {
        shader->Destroy();
        delete shader;
        shader = nullptr;

        glDeleteTextures(1, &cubeTextureId);
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }
}

void FaceCullingSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("FaceCullingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
//    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, m_pModel->GetMaxViewDistance() * 4);
    glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    glm::vec3 eye = glm::vec3 (2.0f, 2.0f, 3.0f);
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

void FaceCullingSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    LOGE("FaceCullingSample::updateTransformMatrix, rotateX = %f", rotateX);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}