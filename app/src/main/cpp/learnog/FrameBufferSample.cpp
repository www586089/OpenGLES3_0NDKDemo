//
// Created by Thinkpad on 2022/8/31.
//

#include "FrameBufferSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Shader.h"

//https://blog.csdn.net/wangxingxing321/article/details/107665565?spm=1001.2014.3001.5502
FrameBufferSample::FrameBufferSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

FrameBufferSample::~FrameBufferSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

void FrameBufferSample::init() {
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

    /**
     * vertex attributes for a quad that fills
     * the entire screen in Normalized Device Coordinates.
     */
    float quadVertices[] = {
            // positions   // texCoords
            -1.0f,  1.0f,  0.0f, 1.0f,
            -1.0f, -1.0f,  0.0f, 0.0f,
             1.0f, -1.0f,  1.0f, 0.0f,

            -1.0f,  1.0f,  0.0f, 1.0f,
             1.0f, -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f,  1.0f, 1.0f
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
    char vScreenShaderStr[] =
            "#version 300 es                                        \n"
            "layout (location = 0) in vec2 aPos;                    \n"
            "layout (location = 1) in vec2 aTexCoords;              \n"
            "                                                       \n"
            "out vec2 TexCoords;                                    \n"
            "                                                       \n"
            "void main()                                            \n"
            "{                                                      \n"
            "    TexCoords = aTexCoords;                            \n"
            "    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);      \n"
            "}";
    char fScreenShaderStr[] =
            "#version 300 es                                         \n"
            "out vec4 FragColor;                                     \n"
            "                                                        \n"
            "in vec2 TexCoords;                                      \n"
            "                                                        \n"
            "uniform sampler2D screenTexture;                        \n"
            "                                                        \n"
            " \n"
            "const float offset = 1.0 / 300.0;  \n"
            " \n"
            "void main()\n"
            "{\n"
            "    vec2 offsets[9] = vec2[](\n"
            "        vec2(-offset,  offset), // top-left\n"
            "        vec2( 0.0f,    offset), // top-center\n"
            "        vec2( offset,  offset), // top-right\n"
            "        vec2(-offset,  0.0f),   // center-left\n"
            "        vec2( 0.0f,    0.0f),   // center-center\n"
            "        vec2( offset,  0.0f),   // center-right\n"
            "        vec2(-offset, -offset), // bottom-left\n"
            "        vec2( 0.0f,   -offset), // bottom-center\n"
            "        vec2( offset, -offset)  // bottom-right    \n"
            "    );\n"
            " \n"
            "    float kernel[9] = float[](\n"
            "        -1.0, -1.0, -1.0,\n"
            "        -1.0,  9.0, -1.0,\n"
            "        -1.0, -1.0, -1.0 \n"
            "    );\n"
            "    \n"
            "    vec3 sampleTex[9];\n"
            "    for(int i = 0; i < 9; i++)\n"
            "    {\n"
            "        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));\n"
            "    }\n"
            "    vec3 col = vec3(0.0);\n"
            "    for(int i = 0; i < 9; i++)\n"
            "        col += sampleTex[i] * kernel[i];\n"
            "    \n"
            "    FragColor = vec4(col, 1.0);\n"
            "}  ";

    shader = Shader(vShaderStr, fShaderStr);
    screenShader = Shader(vScreenShaderStr, fScreenShaderStr);

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

        // screen quad VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));


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

        // shader configuration
        // --------------------
        shader.use();
        shader.setInt("texture1", 0);

        screenShader.use();
        screenShader.setInt("screenTexture", 0);
    } else {
        LOGE("FrameBufferSample::Init create program fail");
        return;
    }
}


void FrameBufferSample::draw(int screenW, int screenH) {
    LOGE("FrameBufferSample::Draw()");

    if (!shader.isAvailable()) {
        LOGE("FrameBufferSample::Draw() return");
        return;
    }
    LOGE("FrameBufferSample::Do Draw()");
    // render
    // ------
    // configure global opengl state
    // -----------------------------
    if (firstFrame) {
        firstFrame = false;
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        // framebuffer configuration
        // -------------------------
        glGenFramebuffers(1, &framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        // create a color attachment texture
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenW, screenH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
        // create a render buffer object for depth and stencil attachment (we won't be sampling these)
        glGenRenderbuffers(1, &rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        // use a single render buffer object for both a depth AND stencil buffer.
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenW, screenH);
        // now actually attach it
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LOGE("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


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
    // 1st render scene
    // ------
    // bind to framebuffer and draw scene as we normally would to color texture
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)

    // make sure we clear the framebuffer's content
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    shader.setMat4("view", view);
    shader.setMat4("projection", projection);

    // cubes
    glBindVertexArray(cubeVAO);
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
    glBindVertexArray(GL_NONE);

    // floor
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, floorImage.width, floorImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, floorImage.ppPlane[0]);
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", model/*glm::mat4(1.0f)*/);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(GL_NONE);


    // 2nd draw from the frame buffer.
    // now bind back to default framebuffer and draw a quad plane
    // with the attached framebuffer color texture
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    // disable depth test so screen-space quad isn't discarded due to depth test.
    glDisable(GL_DEPTH_TEST);
    // clear all relevant buffers, set clear color to
    // white (not really necessary actually, since we
    // won't be able to see behind the quad anyways)
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    screenShader.use();
    glBindVertexArray(quadVAO);
    // use the color attachment texture as the texture of the quad plane
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(GL_NONE);
}

void FrameBufferSample::loadImage(NativeImage *pImage) {
    LOGE("FrameBufferSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        cubeImage.width = pImage->width;
        cubeImage.height = pImage->height;
        cubeImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &cubeImage);
    }
}

void FrameBufferSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("FrameBufferSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        if (0 == index) {
            floorImage.width = pImage->width;
            floorImage.height = pImage->height;
            floorImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &floorImage);
        }
    }
}

void FrameBufferSample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteBuffers(1, &planeVBO);

        glDeleteBuffers(1, &floorTexture);

        glDeleteTextures(1, &cubeTexture);
        glDeleteTextures(1, &textureColorbuffer);
        glDeleteFramebuffers(1, &framebuffer);
        glDeleteRenderbuffers(1, &rbo);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void FrameBufferSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("FrameBufferSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
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

void FrameBufferSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}