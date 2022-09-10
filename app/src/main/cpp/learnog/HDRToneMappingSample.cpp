//
// Created by Thinkpad on 2022/9/9.
//

#include "HDRToneMappingSample.h"
#include <gtc/matrix_transform.hpp>
#include <vector>
#include "../utils/GLUtils.h"
#include "Shader.h"

HDRToneMappingSample::HDRToneMappingSample() {
    firstFrame = true;
    woodTexture = -1;
    quadVAO = 0;
    quadVBO = 0;
    
    cubeVAO = 0;
    cubeVBO = 0;
    
    hdrFBO = 0;
    colorBuffer = 0;
    rboDepthBuffer = 0;

    farthestLightPosZ = 20.0f;
    m_AngleX = 0;
    m_AngleY = 0;
}

HDRToneMappingSample::~HDRToneMappingSample() {
    firstFrame = true;
    woodTexture = -1;
    quadVAO = 0;
    quadVBO = 0;

    cubeVAO = 0;
    cubeVBO = 0;

    hdrFBO = 0;
    colorBuffer = 0;
    rboDepthBuffer = 0;

    farthestLightPosZ = 20.0f;
    m_AngleX = 0;
    m_AngleY = 0;
}

void HDRToneMappingSample::init() {
    if (shader.isAvailable()) {
        return;
    }
    char lightingVShaderStr[] =
            "#version 300 es\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec3 aNormal;\n"
            "layout (location = 2) in vec2 aTexCoords;\n"
            "\n"
            "out vec3 FragPos;\n"
            "out vec3 Normal;\n"
            "out vec2 TexCoords;\n"
            "\n"
            "uniform mat4 projection;\n"
            "uniform mat4 view;\n"
            "uniform mat4 model;\n"
            "\n"
            "uniform bool inverse_normals;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    FragPos = vec3(model * vec4(aPos, 1.0));   \n"
            "    TexCoords = aTexCoords;\n"
            "    \n"
            "    vec3 n = inverse_normals ? -aNormal : aNormal;\n"
            "    \n"
            "    mat3 normalMatrix = transpose(inverse(mat3(model)));\n"
            "    Normal = normalize(normalMatrix * n);\n"
            "    \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
            "}";

    char lightingFShaderStr[] =
            "#version 300 es\n"
            "out vec4 FragColor;\n"
            "\n"
            "in vec3 FragPos;\n"
            "in vec3 Normal;\n"
            "in vec2 TexCoords;\n"
            "\n"
            "struct Light {\n"
            "    vec3 Position;\n"
            "    vec3 Color;\n"
            "};\n"
            "\n"
            "uniform Light lights[4];\n"
            "uniform sampler2D diffuseTexture;\n"
            "uniform vec3 viewPos;\n"
            "\n"
            "void main()\n"
            "{           \n"
            "    vec3 color = texture(diffuseTexture, TexCoords).rgb;\n"
            "    vec3 normal = normalize(Normal);\n"
            "    // ambient\n"
            "    vec3 ambient = 0.0 * color;\n"
            "    // lighting\n"
            "    vec3 lighting = vec3(0.0);\n"
            "    for(int i = 0; i < 4; i++)\n"
            "    {\n"
            "        // diffuse\n"
            "        vec3 lightDir = normalize(lights[i].Position - FragPos);\n"
            "        float diff = max(dot(lightDir, normal), 0.0);\n"
            "        vec3 diffuse = lights[i].Color * diff * color;      \n"
            "        vec3 result = diffuse;        \n"
            "        // attenuation (use quadratic as we have gamma correction)\n"
            "        float distance = length(FragPos - lights[i].Position);\n"
            "        result *= 1.0 / (distance * distance);\n"
            "        lighting += result;\n"
            "                \n"
            "    }\n"
            "    FragColor = vec4(ambient + lighting, 1.0);\n"
            "}";
    char vHdrShaderStr[] =
            "#version 300 es\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 1) in vec2 aTexCoords;\n"
            "\n"
            "out vec2 TexCoords;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    TexCoords = aTexCoords;\n"
            "    gl_Position = vec4(aPos, 1.0);\n"
            "}";
    char fHdrShaderStr[] =
            "#version 300 es\n"
            "out vec4 FragColor;\n"
            "\n"
            "in vec2 TexCoords;\n"
            "\n"
            "uniform sampler2D hdrBuffer;\n"
            "uniform bool hdr;\n"
            "uniform float exposure;\n"
            "\n"
            "void main()\n"
            "{             \n"
            "    const float gamma = 2.2;\n"
            "    vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;\n"
            "    if(hdr) {\n"
            "        // reinhard\n"
            "        // vec3 result = hdrColor / (hdrColor + vec3(1.0));\n"
            "        // exposure\n"
            "        vec3 result = vec3(1.0) - exp(-hdrColor * exposure);\n"
            "        // also gamma correct while we're at it       \n"
            "        result = pow(result, vec3(1.0 / gamma));\n"
            "        FragColor = vec4(result, 1.0);\n"
            "    } else {\n"
            "        vec3 result = pow(hdrColor, vec3(1.0 / gamma));\n"
            "        FragColor = vec4(result, 1.0);\n"
            "    }\n"
            "}";

    shader = Shader(lightingVShaderStr, lightingFShaderStr);
    hdrShader = Shader(vHdrShaderStr, fHdrShaderStr);
    if (shader.isAvailable()) {

        // shader configuration
        // --------------------
        shader.use();
        shader.setInt("diffuseTexture", 0);
        hdrShader.use();
        hdrShader.setInt("hdrBuffer", 0);
    } else {
        LOGE("HDRToneMappingSample::Init create program fail");
        return;
    }
}

void HDRToneMappingSample::renderQuad() {
    if (quadVAO == 0) {
        float quadVertices[] = {
                // positions        // texture Coords
                -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
                 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),(void *) (3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(GL_NONE);
}

void HDRToneMappingSample::renderCube() {
    // initialize (if necessary)
    if (cubeVAO == 0) {
        float vertices[] = 
                {
                // back face
                -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f, 0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f, 0.0f, 0.0f, 0.0f  // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void *) (3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),(void *) (6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
}

void HDRToneMappingSample::loadImage(NativeImage *pImage) {
    LOGE("HDRToneMappingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        woodImage.width = pImage->width;
        woodImage.height = pImage->height;
        woodImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &woodImage);
    }
}

void HDRToneMappingSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("HDRToneMappingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
}


void HDRToneMappingSample::draw(int screenW, int screenH) {
    LOGE("HDRToneMappingSample::Draw()");

    if (!shader.isAvailable()) {
        LOGE("HDRToneMappingSample::Draw() return");
        return;
    }
    if (firstFrame) {
        firstFrame = false;
        glEnable(GL_DEPTH_TEST);
        glGenFramebuffers(1, &hdrFBO);
        // create floating point color buffer
        glGenTextures(1, &colorBuffer);
        glBindTexture(GL_TEXTURE_2D, colorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenW, screenH, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // create depth buffer (render buffer)
        glGenRenderbuffers(1, &rboDepthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenW, screenH);
        // attach buffers
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepthBuffer);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LOGE("HDRToneMappingSample ::Framebuffer not complete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

        // Config wood texture
        glGenTextures(1, &woodTexture);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    // render
    // ------
    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / (float) screenH);

    // lighting info
    // -------------
    // positions
    std::vector<glm::vec3> lightPositions;
    lightPositions.push_back(glm::vec3( 0.0f,  0.0f, farthestLightPosZ)); // back light
    lightPositions.push_back(glm::vec3(-1.4f, -1.9f, 9.0f));
    lightPositions.push_back(glm::vec3( 0.0f, -1.8f, 4.0f));
    lightPositions.push_back(glm::vec3( 0.8f, -1.7f, 6.0f));
    // colors
    std::vector<glm::vec3> lightColors;
    lightColors.push_back(glm::vec3(200.0f, 200.0f, 200.0f));
    lightColors.push_back(glm::vec3(0.1f, 0.0f, 0.0f));
    lightColors.push_back(glm::vec3(0.0f, 0.0f, 0.2f));
    lightColors.push_back(glm::vec3(0.0f, 0.1f, 0.0f));

    // render
    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    // 1. render scene into floating point framebuffer
    // -----------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, woodImage.width, woodImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, woodImage.ppPlane[0]);
    // set lighting uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++) {
        shader.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        shader.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
    }
    shader.setVec3("viewPos", eyePosition);
    // render tunnel
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 25.0));
    model = glm::scale(model, glm::vec3(2.5f, 2.5f, 27.5f));
    shader.setMat4("model", model);
    shader.setInt("inverse_normals", true);
    renderCube();
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    // 2. now render floating point color buffer to 2D quad and tonemap HDR colors to default framebuffer's (clamped) color range
    // --------------------------------------------------------------------------------------------------------------------------
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    hdrShader.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffer);
    hdrShader.setInt("hdr", true);
    hdrShader.setFloat("exposure", 0.5);
    renderQuad();
}

void HDRToneMappingSample::changeStatus(int type, int flag) {
    farthestLightPosZ += 1.0;
    LOGE("HDRToneMappingSample::changestatus far = %f", farthestLightPosZ);
}


void HDRToneMappingSample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();
        hdrShader.deleteProgram();

        glDeleteTextures(1, &woodTexture);

        glDeleteVertexArrays(1, &quadVAO);
        glDeleteVertexArrays(1, &cubeVAO);

        glDeleteBuffers(1, &quadVBO);
        glDeleteBuffers(1, &cubeVBO);

        glDeleteFramebuffers(1, &hdrFBO);
        glDeleteTextures(1, &colorBuffer);
        glDeleteRenderbuffers(1, &rboDepthBuffer);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void HDRToneMappingSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("HDRToneMappingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float r = 3.0;
    float lightX = r * sin(radiansY);
    float lightZ = r * cos(radiansY);
    LOGE("HDRToneMappingSample, lightX = %f, lightZ = %f", lightX, lightZ);
    //lightPos = glm::vec3(lightX, 3.0f, lightZ);//1.5f, 2.0f, 3.0f
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));

    // View matrix
    eyePosition = glm::vec3 (lightX, 0.0f,  lightZ);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void HDRToneMappingSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}