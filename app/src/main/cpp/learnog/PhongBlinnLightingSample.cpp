//
// Created by Thinkpad on 2022/9/4.
//

#include "PhongBlinnLightingSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Shader.h"

PhongBlinnLightingSample::PhongBlinnLightingSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

PhongBlinnLightingSample::~PhongBlinnLightingSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void PhongBlinnLightingSample::init() {
    if (lightingShader.isAvailable()) {
        return;
    }

    char lightingVShaderStr[] =
            "#version 300 es                                           \n"
            "layout (location = 0) in vec3 aPos;                       \n"
            "layout (location = 1) in vec3 aNormal;                    \n"
            "layout (location = 2) in vec2 aTexCoords;                 \n"
            "                                                          \n"
            "out vec3 FragPos;                                         \n"
            "out vec3 Normal;                                          \n"
            "out vec2 TexCoords;                                       \n"
            "                                                          \n"
            "uniform mat4 projection;                                  \n"
            "uniform mat4 view;                                        \n"
            "                                                          \n"
            "void main()                                               \n"
            "{                                                         \n"
            "    FragPos = aPos;                                       \n"
            "    Normal = aNormal;                                     \n"
            "    TexCoords = aTexCoords;                               \n"
            "    gl_Position = projection * view * vec4(aPos, 1.0);    \n"
            "}";

    char lightingFShaderStr[] =
            "#version 300 es                                               \n"
            "out vec4 FragColor;                                           \n"
            "                                                              \n"
            "in vec3 FragPos;                                              \n"
            "in vec3 Normal;                                               \n"
            "in vec2 TexCoords;                                            \n"
            "                                                              \n"
            "uniform sampler2D floorTexture;                               \n"
            "uniform vec3 lightPos;                                        \n"
            "uniform vec3 viewPos;                                         \n"
            "uniform bool blinn;                                           \n"
            "                                                              \n"
            "void main()                                                   \n"
            "{                                                             \n"
            "    vec3 color = texture(floorTexture, TexCoords).rgb;        \n"
            "    // ambient                                                \n"
            "    vec3 ambient = 0.05 * color;                              \n"
            "    // diffuse                                                \n"
            "    vec3 lightDir = normalize(lightPos - FragPos);            \n"
            "    vec3 normal = normalize(Normal);                          \n"
            "    float diff = max(dot(lightDir, normal), 0.0);             \n"
            "    vec3 diffuse = diff * color;                              \n"
            "    // specular                                               \n"
            "    vec3 viewDir = normalize(viewPos - FragPos);              \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);             \n"
            "    float spec = 0.0;                                         \n"
            "    if(blinn) {                                               \n"
            "        vec3 halfwayDir = normalize(lightDir + viewDir);      \n"
            "        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);  \n"
            "    } else  {                                                 \n"
            "       vec3 reflectDir = reflect(-lightDir, normal);          \n"
            "       spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);   \n"
            "    }                                                         \n"
            "    // assuming bright white light color                      \n"
            "    vec3 specular = vec3(0.3) * spec;                         \n"
            "    FragColor = vec4(ambient + diffuse + specular, 1.0);      \n"
            "}";
    lightingShader = Shader(lightingVShaderStr, lightingFShaderStr);
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float planeVertices[] = {
            // positions            // normals         // texcoords
             10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

             10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
            -10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
             10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
    };
    // plane VAO
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindVertexArray(planeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glBindVertexArray(GL_NONE);

    // Config Floor Texture
    glGenTextures(1, &floorTexture);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}


void PhongBlinnLightingSample::loadImage(NativeImage *pImage) {
    LOGE("PhongBlinnLightingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        floorImage.width = pImage->width;
        floorImage.height = pImage->height;
        floorImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &floorImage);
    }
}

void PhongBlinnLightingSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("PhongBlinnLightingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
}

void PhongBlinnLightingSample::draw(int screenW, int screenH) {
    LOGE("PhongBlinnLightingSample::Draw()");

    if (!lightingShader.isAvailable()) {
        LOGE("PhongBlinnLightingSample::Draw() return");
        return;
    }

    LOGE("PhongBlinnLightingSample::Do Draw()");
    if (firstFrame) {
        firstFrame = false;
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //upload RGBA image data
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, floorImage.width, floorImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, floorImage.ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // view/projection transformations
    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);

    lightingShader.use();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    // set light uniforms
    lightingShader.setVec3("viewPos", eyePosition);
    lightingShader.setVec3("lightPos", lightPos);
    lightingShader.setInt("blinn", blinn);
    // floor
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    lightingShader.setInt("floorTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(GL_NONE);
}

void PhongBlinnLightingSample::destroy() {
    if (lightingShader.isAvailable()) {
        lightingShader.deleteProgram();

        glDeleteVertexArrays(1, &planeVAO);
        glDeleteVertexArrays(1, &planeVBO);
        glDeleteTextures(1, &floorTexture);
        NativeImageUtil::FreeNativeImage(&floorImage);
    }
}

void PhongBlinnLightingSample::changeStatus(int type, int flag) {
    if (0 == flag) {
        blinn = false;
    } else {
        blinn = true;
    }
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void PhongBlinnLightingSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("PhongBlinnLightingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float lightX = 4.0f * sin(radiansY);
    float lightZ = 4.0f * cos(radiansY);
    lightPos = glm::vec3(0, 0, 0);//1.5f, 2.0f, 3.0f
    // View matrix
    eyePosition = glm::vec3(lightX, 0.0, lightZ);//glm::vec3 (0.0f, 0.0f, 3.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void PhongBlinnLightingSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}