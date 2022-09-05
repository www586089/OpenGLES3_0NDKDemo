//
// Created by Thinkpad on 2022/9/5.
//

#include "ShadowMappingDepthSample.h"
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include "../utils/GLUtils.h"
#include "Shader.h"

ShadowMappingDepthSample::ShadowMappingDepthSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

ShadowMappingDepthSample::~ShadowMappingDepthSample() {
    NativeImageUtil::FreeNativeImage(&cubeImage);
    NativeImageUtil::FreeNativeImage(&floorImage);
    NativeImageUtil::FreeNativeImage(&grassImage);
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

void ShadowMappingDepthSample::init() {
    if (shader.isAvailable()) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float r = 0.4f;
    GLfloat cubeVertices[] = {
            //positions  // texture Coords
            -r, -r, -r,  0.0f, 0.0f,
            r, -r, -r,  1.0f, 0.0f,
            r,  r, -r,  1.0f, 1.0f,
            r,  r, -r,  1.0f, 1.0f,
            -r,  r, -r,  0.0f, 1.0f,
            -r, -r, -r,  0.0f, 0.0f,

            -r, -r,  r,  0.0f, 0.0f,
            r, -r,  r,  1.0f, 0.0f,
            r,  r,  r,  1.0f, 1.0f,
            r,  r,  r,  1.0f, 1.0f,
            -r,  r,  r,  0.0f, 1.0f,
            -r, -r,  r,  0.0f, 0.0f,

            -r,  r,  r,  1.0f, 0.0f,
            -r,  r, -r,  1.0f, 1.0f,
            -r, -r, -r,  0.0f, 1.0f,
            -r, -r, -r,  0.0f, 1.0f,
            -r, -r,  r,  0.0f, 0.0f,
            -r,  r,  r,  1.0f, 0.0f,

            r,  r,  r,  1.0f, 0.0f,
            r,  r, -r,  1.0f, 1.0f,
            r, -r, -r,  0.0f, 1.0f,
            r, -r, -r,  0.0f, 1.0f,
            r, -r,  r,  0.0f, 0.0f,
            r,  r,  r,  1.0f, 0.0f,

            -r, -r, -r,  0.0f, 1.0f,
            r, -r, -r,  1.0f, 1.0f,
            r, -r,  r,  1.0f, 0.0f,
            r, -r,  r,  1.0f, 0.0f,
            -r, -r,  r,  0.0f, 0.0f,
            -r, -r, -r,  0.0f, 1.0f,

            -r,  r, -r,  0.0f, 1.0f,
            r,  r, -r,  1.0f, 1.0f,
            r,  r,  r,  1.0f, 0.0f,
            r,  r,  r,  1.0f, 0.0f,
            -r,  r,  r,  0.0f, 0.0f,
            -r,  r, -r,  0.0f, 1.0f
    };

    float planeVertices[] = {
            // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode).
            // this will cause the floor texture to repeat)
            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

            5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
            -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
            5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };
    float transparentVertices[] = {
            // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

            0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
            1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
            1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
    char vShaderStr[] =
            "#version 300 es                                                   \n"
            "layout (location = 0) in vec3 aPos;                               \n"
            "layout (location = 1) in vec2 aTexCoords;                         \n"
            "                                                                  \n"
            "out vec2 TexCoords;                                               \n"
            "                                                                  \n"
            "uniform mat4 model;                                               \n"
            "uniform mat4 view;                                                \n"
            "uniform mat4 projection;                                          \n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    TexCoords = aTexCoords;                                       \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);    \n"
            "}";
    char fShaderStr[] =
            "#version 300 es                                                   \n"
            "out vec4 FragColor;                                               \n"
            "                                                                  \n"
            "in vec2 TexCoords;                                                \n"
            "                                                                  \n"
            "uniform sampler2D texture1;                                       \n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    vec4 texColor = texture(texture1, TexCoords);\n"
            "    FragColor = texColor;"
            "}";
    shader = Shader(vShaderStr, fShaderStr);

    if (shader.isAvailable()) {
        // cube VAO
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);

        glBindVertexArray(cubeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        GLuint offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offset * sizeof(float)));
        glEnableVertexAttribArray(1);
        offset += 3;
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offset * sizeof(float)));
        glBindVertexArray(GL_NONE);

        // plane VAO
        glGenVertexArrays(1, &planeVAO);
        glGenBuffers(1, &planeVBO);

        glBindVertexArray(planeVAO);

        glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offset * sizeof(float)));
        glEnableVertexAttribArray(1);
        offset += 3;
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (offset * sizeof(float)));
        glBindVertexArray(GL_NONE);

        // transparent VAO
        glGenVertexArrays(1, &transparentVAO);
        glGenBuffers(1, &transparentVBO);
        glBindVertexArray(transparentVAO);
        glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(GL_NONE);


        // Config Cube Texture
        glGenTextures(1, &cubeTexture);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);


        // Config Floor Texture
        glGenTextures(1, &floorTexture);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // Config Grass Texture
        glGenTextures(1, &grassTexture);
        glBindTexture(GL_TEXTURE_2D, grassTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    } else {
        LOGE("ShadowMappingDepthSample::Init create program fail");
        return;
    }
}



void ShadowMappingDepthSample::draw(int screenW, int screenH) {
    LOGE("ShadowMappingDepthSample::Draw()");

    if (!shader.isAvailable()) {
        LOGE("ShadowMappingDepthSample::Draw() return");
        return;
    }
    LOGE("ShadowMappingDepthSample::Do Draw()");
    // render
    // ------
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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
    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    //cubes
    glBindVertexArray(cubeVAO);
    shader.setInt("texture1", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cubeImage.width, cubeImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeImage.ppPlane[0]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);


    // floor
    glBindVertexArray(planeVAO);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, floorImage.width, floorImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, floorImage.ppPlane[0]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", model/*glm::mat4(1.0f)*/);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(GL_NONE);

    // transparent vegetation locations
    // --------------------------------
    std::vector<glm::vec3> vegetation = {
            glm::vec3(-1.5f, 0.0f, -0.48f),
            glm::vec3( 1.5f, 0.0f,  0.51f),
            glm::vec3( 0.0f, 0.0f,  0.7f),
            glm::vec3(-0.3f, 0.0f, -2.3f),
            glm::vec3( 0.5f, 0.0f, -0.6f)
    };
    // sort the transparent windows before rendering
    // ---------------------------------------------
    std::map<float, glm::vec3> sorted;
    for (unsigned int i = 0; i < vegetation.size(); i++) {
        float distance = glm::length(eyePosition - vegetation[i]);
        sorted[distance] = vegetation[i];
    }
    // vegetation
    glBindVertexArray(transparentVAO);
    glBindTexture(GL_TEXTURE_2D, grassTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, grassImage.width, grassImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, grassImage.ppPlane[0]);
    //没有按照从远到近胡顺序透明对象，此时会有遮挡
//    for (unsigned int i = 0; i < vegetation.size(); i++) {
//        model = glm::mat4(1.0f);
//        model = glm::translate(model, vegetation[i]);
//        model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
//        shader.setMat4("model", model);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//    }

    // windows (from furthest to nearest)
    for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, it->second);
        model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glBindVertexArray(GL_NONE);

}

void ShadowMappingDepthSample::loadImage(NativeImage *pImage) {
    LOGE("ShadowMappingDepthSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        cubeImage.width = pImage->width;
        cubeImage.height = pImage->height;
        cubeImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &cubeImage);
    }
}

void ShadowMappingDepthSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("ShadowMappingDepthSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        if (0 == index) {
            floorImage.width = pImage->width;
            floorImage.height = pImage->height;
            floorImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &floorImage);
        } else if (1 == index) {
            grassImage.width = pImage->width;
            grassImage.height = pImage->height;
            grassImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &grassImage);
        }
    }
}

void ShadowMappingDepthSample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteBuffers(1, &planeVBO);

        glDeleteTextures(1, &cubeTexture);
        glDeleteTextures(1, &floorTexture);
        glDeleteTextures(1, &grassTexture);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void ShadowMappingDepthSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("ShadowMappingDepthSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    // View matrix
    eyePosition = glm::vec3 (0.0f, 0.8f, 6.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void ShadowMappingDepthSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}
