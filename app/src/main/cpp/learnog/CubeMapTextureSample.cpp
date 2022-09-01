//
// Created by Thinkpad on 2022/8/31.
//

#include "CubeMapTextureSample.h"
#include <gtc/matrix_transform.hpp>
#include <vector>
#include "../utils/GLUtils.h"
#include "Shader.h"

//https://blog.csdn.net/wangxingxing321/article/details/107669548?spm=1001.2014.3001.5502
CubeMapTextureSample::CubeMapTextureSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

CubeMapTextureSample::~CubeMapTextureSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

void CubeMapTextureSample::init() {
    if (shader.isAvailable()) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float r = 0.2f;
    GLfloat cubeVertices[] = {
            //positions  //normal
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

            -r,  r,  r,  1.0f,  0.0f,  0.0f,
            -r,  r, -r,  1.0f,  0.0f,  0.0f,
            -r, -r, -r,  1.0f,  0.0f,  0.0f,
            -r, -r, -r,  1.0f,  0.0f,  0.0f,
            -r, -r,  r,  1.0f,  0.0f,  0.0f,
            -r,  r,  r,  1.0f,  0.0f,  0.0f,

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

    float skyboxA = 2.0f;
    float skyboxVertices[] = {
            // positions          
            -skyboxA,  skyboxA, -skyboxA,
            -skyboxA, -skyboxA, -skyboxA,
             skyboxA, -skyboxA, -skyboxA,
             skyboxA, -skyboxA, -skyboxA,
             skyboxA,  skyboxA, -skyboxA,
            -skyboxA,  skyboxA, -skyboxA,

            -skyboxA, -skyboxA,  skyboxA,
            -skyboxA, -skyboxA, -skyboxA,
            -skyboxA,  skyboxA, -skyboxA,
            -skyboxA,  skyboxA, -skyboxA,
            -skyboxA,  skyboxA,  skyboxA,
            -skyboxA, -skyboxA,  skyboxA,

             skyboxA, -skyboxA, -skyboxA,
             skyboxA, -skyboxA,  skyboxA,
             skyboxA,  skyboxA,  skyboxA,
             skyboxA,  skyboxA,  skyboxA,
             skyboxA,  skyboxA, -skyboxA,
             skyboxA, -skyboxA, -skyboxA,

            -skyboxA, -skyboxA,  skyboxA,
            -skyboxA,  skyboxA,  skyboxA,
             skyboxA,  skyboxA,  skyboxA,
             skyboxA,  skyboxA,  skyboxA,
             skyboxA, -skyboxA,  skyboxA,
            -skyboxA, -skyboxA,  skyboxA,

            -skyboxA,  skyboxA, -skyboxA,
             skyboxA,  skyboxA, -skyboxA,
             skyboxA,  skyboxA,  skyboxA,
             skyboxA,  skyboxA,  skyboxA,
            -skyboxA,  skyboxA,  skyboxA,
            -skyboxA,  skyboxA, -skyboxA,

            -skyboxA, -skyboxA, -skyboxA,
            -skyboxA, -skyboxA,  skyboxA,
             skyboxA, -skyboxA, -skyboxA,
             skyboxA, -skyboxA, -skyboxA,
            -skyboxA, -skyboxA,  skyboxA,
             skyboxA, -skyboxA,  skyboxA
    };
    char vCubeShaderStr[] =
            "#version 300 es                                                   \n"
            "layout (location = 0) in vec3 aPos;                               \n"
            "layout (location = 1) in vec2 aNormal;                         \n"
            "                                                                  \n"
            "out vec3 Normal;\n"
            "out vec3 Position;"
            "                                                                  \n"
            "uniform mat4 model;                                               \n"
            "uniform mat4 view;                                                \n"
            "uniform mat4 projection;                                          \n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    Normal = mat3(transpose(inverse(model))) * aNormal;\n"
            "    Position = vec3(model * vec4(aPos, 1.0));"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);    \n"
            "}";
    char fCubeShaderStr[] =
            "#version 300 es                                                   \n"
            "out vec4 FragColor;                                               \n"
            "                                                                  \n"
            "in vec3 Normal;\n"
            "in vec3 Position;\n"
            " \n"
            "uniform vec3 cameraPos;\n"
            "uniform samplerCube skybox;"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    float ratio = 1.00 / 1.52;"
            "    vec3 I = normalize(Position - cameraPos);\n"
            "    //vec3 R = reflect(I, normalize(Normal));\n"
            "    vec3 R = refract(I, normalize(Normal), ratio);"
            "    FragColor = vec4(texture(skybox, R).rgb, 1.0);"
            "}";
    char vSkyBoxShaderStr[] =
            "#version 300 es                                        \n"
            "layout (location = 0) in vec3 aPos;                    \n"
            "                                                       \n"
            "out vec3 TexCoords;                                    \n"
            "                                                       \n"
            "uniform mat4 model;                                    \n"
            "uniform mat4 projection;                               \n"
            "uniform mat4 view;                                     \n"
            "                                                       \n"
            "void main()                                            \n"
            "{                                                      \n"
            "    TexCoords = aPos;                                  \n"
            "    mat4 mvpMatrix = projection * view * model;        \n"
            "    vec4 pos = mvpMatrix * vec4(aPos, 1.0);            \n"
            "    gl_Position = pos.xyww;                            \n"
            "}  ";
    char fSkyBoxShaderStr[] =
            "#version 300 es                                        \n"
            "precision mediump float;                               \n"
            "out vec4 FragColor;                                    \n"
            "                                                       \n"
            "in vec3 TexCoords;                                     \n"
            "                                                       \n"
            "uniform samplerCube skybox;                            \n"
            "                                                       \n"
            "void main()                                            \n"
            "{                                                      \n"
            "    FragColor = texture(skybox, TexCoords);            \n"
            "}";

    shader = Shader(vCubeShaderStr, fCubeShaderStr);
    skyBoxShader = Shader(vSkyBoxShaderStr, fSkyBoxShaderStr);

    if (shader.isAvailable()) {
        // cube VAO
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        GLuint offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (offset * sizeof(float)));
        glEnableVertexAttribArray(1);
        offset += 3;
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (offset * sizeof(float)));
        glBindVertexArray(GL_NONE);

        // skybox VAO
        glGenBuffers(1, &skyboxVBO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        glGenVertexArrays(1, &skyboxVAO);
        glBindVertexArray(skyboxVAO);
        glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
        glEnableVertexAttribArray(0);
        offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) (offset * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);


        // Config Cube Texture
        glGenTextures(1, &cubeTexture);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

    } else {
        LOGE("CubeMapTextureSample::Init create program fail");
        return;
    }
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front)
// -Z (back)
// -------------------------------------------------------
GLuint CubeMapTextureSample::loadCubemap() {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height;
    for (unsigned int i = 0; i < sizeof(skyBoxRenderImg) / sizeof(NativeImage); i++) {
        unsigned char *data = skyBoxRenderImg[i].ppPlane[0];
        width = skyBoxRenderImg[i].width;
        height = skyBoxRenderImg[i].height;
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);

    return textureID;
}

void CubeMapTextureSample::draw(int screenW, int screenH) {
    LOGE("CubeMapTextureSample::Draw()");

    if (!shader.isAvailable()) {
        LOGE("CubeMapTextureSample::Draw() return");
        return;
    }
    LOGE("CubeMapTextureSample::Do Draw()");
    // render
    // ------
    // configure global opengl state
    // -----------------------------
    if (firstFrame) {
        firstFrame = false;
        cubemapTexture = loadCubemap();
    }

    // render
    // ------
    glEnable(GL_DEPTH_TEST);
//    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

    // draw scene as normal
    shader.use();
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);
    shader.setVec3("cameraPos", eyePosition);
    shader.setInt("skybox", 0);
    // cubes
    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);



    skyBoxShader.use();
    /**
     * change depth function so depth test passes when
     * values are equal to depth buffer's content
     */
//    glDepthFunc(GL_LEQUAL);
    // ... set view and projection matrix
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    skyBoxShader.setMat4("model", model);
    skyBoxShader.setMat4("view", view);
    skyBoxShader.setMat4("projection", projection);
    // Sky box cube
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    skyBoxShader.setInt("skybox", 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
//    glDepthFunc(GL_LESS); // set depth function back to default
}

void CubeMapTextureSample::loadImage(NativeImage *pImage) {
    LOGE("CubeMapTextureSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        cubeImage.width = pImage->width;
        cubeImage.height = pImage->height;
        cubeImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &cubeImage);
    }
}

void CubeMapTextureSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("CubeMapTextureSample::LoadImage, index = %d, pImage = %p", index, pImage->ppPlane[0]);
    if (pImage) {
        if (skyBoxRenderImg[index].ppPlane[0]) {
            NativeImageUtil::FreeNativeImage(&skyBoxRenderImg[index]);
        }

        skyBoxRenderImg[index].width = pImage->width;
        skyBoxRenderImg[index].height = pImage->height;
        skyBoxRenderImg[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &skyBoxRenderImg[index]);
    }
}

void CubeMapTextureSample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();
        skyBoxShader.deleteProgram();

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteVertexArrays(1, &skyboxVAO);
        glDeleteBuffers(1, &skyboxVBO);

        glDeleteTextures(1, &cubeTexture);
        glDeleteTextures(1, &cubemapTexture);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void CubeMapTextureSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("CubeMapTextureSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    // View matrix
    eyePosition = glm::vec3 (0.0f, 0.0f, 1.8f);
    glm::vec3 center = glm::vec3 (0, 0, -1);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void CubeMapTextureSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}