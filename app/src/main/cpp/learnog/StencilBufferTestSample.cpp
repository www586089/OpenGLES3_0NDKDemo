//
// Created by Thinkpad on 2022/8/29.
//

#include "StencilBufferTestSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Shader.h"

StencilBufferTestSample::StencilBufferTestSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

StencilBufferTestSample::~StencilBufferTestSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

void StencilBufferTestSample::init() {
    if (shader.isAvailable()) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float r = 0.5f;
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
            "    FragColor = texture(texture1, TexCoords);                     \n"
            "}";
    char fShaderSingleColorStr[] =
            "#version 300 es\n"
            "out vec4 FragColor;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    FragColor = vec4(0.04, 0.28, 0.26, 1.0);\n"
            "}";
    shader = Shader(vShaderStr, fShaderStr);
    colorShader = Shader(vShaderStr, fShaderSingleColorStr);

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
    } else {
        LOGE("StencilBufferTestSample::Init create program fail");
        return;
    }
}

bool firstFrame = true;


void StencilBufferTestSample::draw(int screenW, int screenH) {
    LOGE("StencilBufferTestSample::Draw()");

    if (!shader.isAvailable()) {
        LOGE("StencilBufferTestSample::Draw() return");
        return;
    }
    LOGE("StencilBufferTestSample::Do Draw()");
    // render
    // ------
    // configure global opengl state
    // -----------------------------
    if (firstFrame) {
        firstFrame = false;
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    }


    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);


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
    shader.setInt("texture1", 0);


    // draw floor as normal, but don't write the floor to the stencil buffer,
    // we only care about the containers. We set its mask to 0x00 to not write to the stencil buffer.
    glStencilMask(0x00);
    // floor
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, floorImage.width, floorImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, floorImage.ppPlane[0]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", model/*glm::mat4(1.0f)*/);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(GL_NONE);

    //cubes
    // 1st. render pass, draw objects as normal, writing to the stencil buffer
    // --------------------------------------------------------------------
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cubeImage.width, cubeImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeImage.ppPlane[0]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    // 2nd. render pass: now draw slightly scaled versions of the objects, this time disabling stencil writing.
    // Because the stencil buffer is now filled with several 1s. The parts of the buffer that are 1 are not drawn, thus only drawing
    // the objects' size differences, making it look like borders.
    // -----------------------------------------------------------------------------------------------------------------------------
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);

    colorShader.use();
    colorShader.setMat4("view", view);
    colorShader.setMat4("projection", projection);
    float scale = 1.02f;
    // cubes

    glBindVertexArray(cubeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cubeImage.width, cubeImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubeImage.ppPlane[0]);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    colorShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(scale, scale, scale));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    colorShader.setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    glStencilMask(0xFF);
    glStencilFunc(GL_ALWAYS, 0, 0xFF);
    glEnable(GL_DEPTH_TEST);
}

void StencilBufferTestSample::loadImage(NativeImage *pImage) {
    LOGE("StencilBufferTestSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        cubeImage.width = pImage->width;
        cubeImage.height = pImage->height;
        cubeImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &cubeImage);
    }
}

void StencilBufferTestSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("StencilBufferTestSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        if (0 == index) {
            floorImage.width = pImage->width;
            floorImage.height = pImage->height;
            floorImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &floorImage);
        }
    }
}

void StencilBufferTestSample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteBuffers(1, &planeVBO);

        glDeleteTextures(1, &cubeTexture);
        glDeleteBuffers(1, &floorTexture);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void StencilBufferTestSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("StencilBufferTestSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    // View matrix
    glm::vec3 eyePosition = glm::vec3 (4.2f, 1.6f, -0.2f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void StencilBufferTestSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}