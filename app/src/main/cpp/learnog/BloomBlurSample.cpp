//
// Created by Thinkpad on 2022/9/11.
//

#include "BloomBlurSample.h"
#include <gtc/matrix_transform.hpp>
#include <vector>
#include "../utils/GLUtils.h"
#include "Shader.h"

BloomBlurSample::BloomBlurSample() {
    firstFrame = true;
    woodTexture = -1;
    quadVAO = 0;
    quadVBO = 0;

    cubeVAO = 0;
    cubeVBO = 0;

    hdrFBO = 0;
    rboDepthBuffer = 0;

    farthestLightPosZ = 20.0f;
    m_AngleX = 0;
    m_AngleY = 0;
}

BloomBlurSample::~BloomBlurSample() {
    firstFrame = true;
    woodTexture = -1;
    quadVAO = 0;
    quadVBO = 0;

    cubeVAO = 0;
    cubeVBO = 0;

    hdrFBO = 0;
    rboDepthBuffer = 0;

    farthestLightPosZ = 20.0f;
    m_AngleX = 0;
    m_AngleY = 0;
}

void BloomBlurSample::init() {
    if (shader.isAvailable()) {
        return;
    }
    char vBloomShaderStr[] =
            "#version 300 es                                                     \n"
            "layout (location = 0) in vec3 aPos;                                 \n"
            "layout (location = 1) in vec3 aNormal;                              \n"
            "layout (location = 2) in vec2 aTexCoords;                           \n"
            "                                                                    \n"
            "out vec3 FragPos;                                                   \n"
            "out vec3 Normal;                                                    \n"
            "out vec2 TexCoords;                                                 \n"
            "                                                                    \n"
            "uniform mat4 projection;                                            \n"
            "uniform mat4 view;                                                  \n"
            "uniform mat4 model;                                                 \n"
            "                                                                    \n"
            "void main() {                                                       \n"
            "    FragPos = vec3(model * vec4(aPos, 1.0));                        \n"
            "    TexCoords = aTexCoords;                                         \n"
            "                                                                    \n"
            "    mat3 normalMatrix = transpose(inverse(mat3(model)));            \n"
            "    Normal = normalize(normalMatrix * aNormal);                     \n"
            "                                                                    \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);      \n"
            "}";

    char fBloomShaderStr[] =
            "#version 300 es                                                     \n"
            "layout(location = 0) out vec4 FragColor;                            \n"
            "layout(location = 1) out vec4 BrightColor;                          \n"
            "                                                                    \n"
            "in vec3 FragPos;                                                    \n"
            "in vec3 Normal;                                                     \n"
            "in vec2 TexCoords;                                                  \n"
            "                                                                    \n"
            "struct Light {                                                      \n"
            "    vec3 Position;                                                  \n"
            "    vec3 Color;                                                     \n"
            "};                                                                  \n"
            "                                                                    \n"
            "uniform Light lights[4];                                            \n"
            "uniform sampler2D diffuseTexture;                                   \n"
            "uniform vec3 viewPos;                                               \n"
            "                                                                    \n"
            "void main() {                                                       \n"
            "    vec3 color = texture(diffuseTexture, TexCoords).rgb;            \n"
            "    vec3 normal = normalize(Normal);                                \n"
            "    // ambient                                                      \n"
            "    vec3 ambient = 0.0 * color;                                     \n"
            "    // lighting                                                     \n"
            "    vec3 lighting = vec3(0.0);                                      \n"
            "    for(int i = 0; i < 4; i++) {                                    \n"
            "        // diffuse                                                  \n"
            "        vec3 lightDir = normalize(lights[i].Position - FragPos);    \n"
            "        float diff = max(dot(lightDir, normal), 0.0);               \n"
            "        vec3 diffuse = lights[i].Color * diff * color;              \n"
            "        vec3 result = diffuse;                                      \n"
            "        // attenuation (use quadratic as we have gamma correction)  \n"
            "        float distance = length(FragPos - lights[i].Position);      \n"
            "        result *= 1.0 / (distance * distance);                      \n"
            "        lighting += result;                                         \n"
            "    }                                                               \n"
            "    vec3 result = ambient + lighting;                               \n"
            "    // check whether result is higher than some threshold, if so,   \n"
            "    // output as bloom threshold color                              \n"
            "    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));   \n"
            "    if (brightness > 1.0) {                                         \n"
            "        BrightColor = vec4(result, 1.0);                            \n"
            "    } else {                                                        \n"
            "        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);                     \n"
            "    }                                                               \n"
            "    FragColor = vec4(result, 1.0);                                  \n"
            "}";
    char fLightBoxShaderStr[] =
            "#version 300 es                                                     \n"
            "layout (location = 0) out vec4 FragColor;                           \n"
            "layout (location = 1) out vec4 BrightColor;                         \n"
            "                                                                    \n"
            "uniform vec3 lightColor;                                            \n"
            "                                                                    \n"
            "void main() {                                                       \n"
            "    FragColor = vec4(lightColor, 1.0);                              \n"
            "    vec3 factor = vec3(0.2126, 0.7152, 0.0722);                     \n"
            "    float brightness = dot(FragColor.rgb, factor);                  \n"
            "    if(brightness > 1.0) {                                          \n"
            "        BrightColor = vec4(FragColor.rgb, 1.0);                     \n"
            "    } else {                                                        \n"
            "        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);                     \n"
            "    }                                                               \n"
            "}";
    char vBlurShaderStr[] =
            "#version 300 es                                                     \n"
            "layout (location = 0) in vec3 aPos;                                 \n"
            "layout (location = 1) in vec2 aTexCoords;                           \n"
            "                                                                    \n"
            "out vec2 TexCoords;                                                 \n"
            "                                                                    \n"
            "void main() {                                                       \n"
            "    TexCoords = aTexCoords;                                         \n"
            "    gl_Position = vec4(aPos, 1.0);                                  \n"
            "}";
    char fBlurShaderStr[] =
            "#version 300 es                                                     \n"
            "out vec4 FragColor;                                                 \n"
            "                                                                    \n"
            "in vec2 TexCoords;                                                  \n"
            "                                                                    \n"
            "uniform sampler2D image;                                            \n"
            "                                                                    \n"
            "uniform bool horizontal;                                            \n"
            "float weight[5] = float[] (0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);   \n"
            "                                                                                                    \n"
            "void main() {                                                                                       \n"
            "    ivec2 size = textureSize(image, 0);                                                             \n"
            "    vec2 tex_offset = vec2(1.0 / float(size.x), 1.0 / float(size.y));//gets size of single texel    \n"
            "    vec3 result = texture(image, TexCoords).rgb * weight[0];                                        \n"
            "    if(horizontal) {                                                                                \n"
            "        for(int i = 1; i < 5; ++i) {                                                                \n"
            "            float index = float(1.0);                                                               \n"
            "            result += texture(image, TexCoords + vec2(tex_offset.x * index, 0.0)).rgb * weight[i];  \n"
            "            result += texture(image, TexCoords - vec2(tex_offset.x * index, 0.0)).rgb * weight[i];  \n"
            "        }                                                                                           \n"
            "    } else {                                                                                        \n"
            "        for(int i = 1; i < 5; ++i) {                                                                \n"
            "            float index = float(1.0);                                                               \n"
            "            result += texture(image, TexCoords + vec2(0.0, tex_offset.y * index)).rgb * weight[i];  \n"
            "            result += texture(image, TexCoords - vec2(0.0, tex_offset.y * index)).rgb * weight[i];  \n"
            "        }                                                                                           \n"
            "    }                                                                                               \n"
            "    FragColor = vec4(result, 1.0);                                                                  \n"
            "}";
    char vBloomFinalShaderStr[] =
            "#version 300 es                                                \n"
            "layout (location = 0) in vec3 aPos;                            \n"
            "layout (location = 1) in vec2 aTexCoords;                      \n"
            "                                                               \n"
            "out vec2 TexCoords;                                            \n"
            "                                                               \n"
            "void main() {                                                  \n"
            "    TexCoords = aTexCoords;                                    \n"
            "    gl_Position = vec4(aPos, 1.0);                             \n"
            "}";
    char fBloomFinalShaderStr[] =
            "#version 300 es                                               \n"
            "out vec4 FragColor;                                           \n"
            "                                                              \n"
            "in vec2 TexCoords;                                            \n"
            "                                                              \n"
            "uniform sampler2D scene;                                      \n"
            "uniform sampler2D bloomBlur;                                  \n"
            "uniform bool bloom;                                           \n"
            "uniform float exposure;                                       \n"
            "                                                              \n"
            "void main() {                                                 \n"
            "    const float gamma = 2.2;                                  \n"
            "    vec3 hdrColor = texture(scene, TexCoords).rgb;            \n"
            "    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;      \n"
            "    if(bloom) {                                               \n"
            "        hdrColor += bloomColor; // additive blending          \n"
            "    }                                                         \n"
            "    // tone mapping                                           \n"
            "    // 1. reinhard                                            \n"
            "    // vec3 result = hdrColor / (hdrColor + vec3(1.0));       \n"
            "    // exposure                                               \n"
            "    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);      \n"
            "    // 2. also gamma correct while we're at it                \n"
            "    result = pow(result, vec3(1.0 / gamma));                  \n"
            "    FragColor = vec4(result, 1.0);                            \n"
            "}";

    LOGE("----debug----: 1 shader ");
    shader = Shader(vBloomShaderStr, fBloomShaderStr);
    LOGE("----debug----: 2 shaderLight");
    shaderLight = Shader(vBloomShaderStr, fLightBoxShaderStr);
    LOGE("----debug----: 3 shaderBlur");
    shaderBlur = Shader(vBlurShaderStr, fBlurShaderStr);
    LOGE("----debug----: 4 shaderBloomFinal");
    shaderBloomFinal = Shader(vBloomFinalShaderStr, fBloomFinalShaderStr);
    if (shader.isAvailable()) {

        // Config wood texture
        glGenTextures(1, &woodTexture);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // Config wood texture
        glGenTextures(1, &containerTexture);
        glBindTexture(GL_TEXTURE_2D, containerTexture);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // shader configuration
        // --------------------
        shader.use();
        shader.setInt("diffuseTexture", 0);
        shaderBlur.use();
        shaderBlur.setInt("image", 0);
        shaderBloomFinal.use();
        shaderBloomFinal.setInt("scene", 0);
        shaderBloomFinal.setInt("bloomBlur", 1);
    } else {
        LOGE("BloomBlurSample::Init create program fail");
        return;
    }
}

void BloomBlurSample::renderQuad() {
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

void BloomBlurSample::renderCube() {
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

void BloomBlurSample::loadImage(NativeImage *pImage) {
    LOGE("BloomBlurSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        woodImage.width = pImage->width;
        woodImage.height = pImage->height;
        woodImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &woodImage);
    }
}

void BloomBlurSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("BloomBlurSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (0 == index) {
        containerImage.width = pImage->width;
        containerImage.height = pImage->height;
        containerImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &containerImage);
    }
}


void BloomBlurSample::draw(int screenW, int screenH) {
    LOGE("BloomBlurSample::Draw()");

    if (!shader.isAvailable()) {
        LOGE("BloomBlurSample::Draw() return");
        return;
    }
    if (firstFrame) {
        firstFrame = false;
        glEnable(GL_DEPTH_TEST);
        // configure (floating point) framebuffers
        // ---------------------------------------
        glGenFramebuffers(1, &hdrFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, hdrFBO);
        // create 2 floating point color buffers (1 for normal rendering, other for brightness threshold values)
        glGenTextures(2, colorBuffers);
        for (unsigned int i = 0; i < 2; i++) {
            glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenW, screenH, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            // attach texture to framebuffer
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colorBuffers[i], 0);
        }
        // create and attach depth buffer (render buffer)
        glGenRenderbuffers(1, &rboDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenW, screenH);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
        // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
        GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
        glDrawBuffers(2, attachments);
        // finally check if framebuffer is complete
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            LOGE("Framebuffer not complete!");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // ping-pong-framebuffer for blurring
        glGenFramebuffers(2, pingpongFBO);
        glGenTextures(2, pingpongColorBuffers);
        for (unsigned int i = 0; i < 2; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
            glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[i]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenW, screenH, 0, GL_RGBA, GL_FLOAT, NULL);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongColorBuffers[i], 0);
            // also check if frame buffers are complete (no need for depth buffer)
            if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
                LOGE("Framebuffer not complete!");
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
    }
    // render
    // ------
    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / (float) screenH);

    // lighting info
    // -------------
    // positions
    std::vector<glm::vec3> lightPositions;
    lightPositions.emplace_back(glm::vec3( 0.0f, 0.5f,  1.5f));
    lightPositions.emplace_back(glm::vec3(-4.0f, 0.5f, -3.0f));
    lightPositions.emplace_back(glm::vec3( 3.0f, 0.5f,  1.0f));
    lightPositions.emplace_back(glm::vec3(-.8f,  2.4f, -1.0f));
    // colors
    std::vector<glm::vec3> lightColors;
    lightColors.emplace_back(glm::vec3(5.0f,   5.0f,  5.0f));
    lightColors.emplace_back(glm::vec3(10.0f,  0.0f,  0.0f));
    lightColors.emplace_back(glm::vec3(0.0f,   0.0f,  15.0f));
    lightColors.emplace_back(glm::vec3(0.0f,   5.0f,  0.0f));

    // render
    // ------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
    // create one large cube that acts as the floor
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(12.5f, 0.5f, 12.5f));
    shader.setMat4("model", model);
    renderCube();
    // then create multiple cubes as the scenery
    glBindTexture(GL_TEXTURE_2D, containerTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, containerImage.width, containerImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, containerImage.ppPlane[0]);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.0f, -1.0f, 2.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 2.7f, 4.0));
    model = glm::rotate(model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(1.25));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 1.0f, -3.0));
    model = glm::rotate(model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    shader.setMat4("model", model);
    renderCube();

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, 0.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    shader.setMat4("model", model);
    renderCube();

    // finally show all the light sources as bright cubes
    shaderLight.use();
    shaderLight.setMat4("projection", projection);
    shaderLight.setMat4("view", view);

    for (unsigned int i = 0; i < lightPositions.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPositions[i]));
        model = glm::scale(model, glm::vec3(0.25f));
        shaderLight.setMat4("model", model);
        shaderLight.setVec3("lightColor", lightColors[i]);
        renderCube();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    // 2. blur bright fragments with two-pass Gaussian Blur
    // --------------------------------------------------
    bool horizontal = true, firstIteration = true;
    unsigned int amount = 10;
    unsigned int pingpongColorBufferIndex;
    shaderBlur.use();
    for (unsigned int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
        shaderBlur.setInt("horizontal", horizontal);
        // bind texture of other framebuffer (or scene if first iteration)
        pingpongColorBufferIndex = (unsigned int) !horizontal;
        LOGE("BloomBlurSample::draw.......pingpongColorBuffers index = %d", pingpongColorBufferIndex);
        glBindTexture(GL_TEXTURE_2D, firstIteration ? colorBuffers[1] : pingpongColorBuffers[pingpongColorBufferIndex]);
        renderQuad();
        horizontal = !horizontal;
        if (firstIteration) {
            firstIteration = false;
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    // 3. now render floating point color buffer to 2D quad and tone map HDR colors
    // to default frame buffer's (clamped) color range
    // --------------------------------------------------------------------------------------------------------------------------
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderBloomFinal.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpongColorBuffers[!horizontal]);
    shaderBloomFinal.setBool("bloom", true);
    shaderBloomFinal.setFloat("exposure", 5.0);
    renderQuad();
}

void BloomBlurSample::changeStatus(int type, int flag) {
    farthestLightPosZ += 1.0;
    LOGE("BloomBlurSample::changestatus far = %f", farthestLightPosZ);
}


void BloomBlurSample::destroy() {
    if (shader.isAvailable()) {
        shader.deleteProgram();
        shaderLight.deleteProgram();

        glDeleteTextures(1, &woodTexture);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void BloomBlurSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("BloomBlurSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float r = 15.0;
    float lightX = r * sin(radiansY);
    float lightZ = r * cos(radiansY);
    LOGE("BloomBlurSample, lightX = %f, lightZ = %f", lightX, lightZ);
    //lightPos = glm::vec3(lightX, 3.0f, lightZ);//1.5f, 2.0f, 3.0f
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));

    // View matrix
    eyePosition = glm::vec3 (lightX, 8.0f,  lightZ);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void BloomBlurSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}