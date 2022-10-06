//
// Created by Thinkpad on 2022/10/5.
//

#include "DeferredShadingSample.h"
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <model.h>
#include <gtc/type_ptr.hpp>
#include "../utils/GLUtils.h"
#include "Shader.h"

DeferredShadingSample::DeferredShadingSample() {
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

DeferredShadingSample::~DeferredShadingSample() {
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

void DeferredShadingSample::init() {
    if (shaderGeometryPass.isAvailable()) {
        return;
    }
    char gBufferVS[] =
            "#version 300 es                                                     \n"
            "layout (location = 0) in vec3 aPos;            \n"
            "layout (location = 1) in vec3 aNormal;         \n"
            "layout (location = 2) in vec2 aTexCoords;      \n"
            "                                               \n"
            "out vec3 FragPos;                              \n"
            "out vec2 TexCoords;                            \n"
            "out vec3 Normal;                               \n"
            "                                               \n"
            "uniform mat4 model;                            \n"
            "uniform mat4 view;                             \n"
            "uniform mat4 projection;                       \n"
            "                                               \n"
            "void main() {                                  \n"
            "    vec4 worldPos = model * vec4(aPos, 1.0);   \n"
            "    FragPos = worldPos.xyz;                    \n"
            "    TexCoords = aTexCoords;                    \n"
            "                                               \n"
            "    mat3 normalMatrix = transpose(inverse(mat3(model)));\n"
            "    Normal = normalMatrix * aNormal;                    \n"
            "                                                        \n"
            "    gl_Position = projection * view * worldPos;         \n"
            "}";
    char gBufferFS[] =
            "#version 300 es                                                     \n"
            "layout (location = 0) out vec3 gPosition;            \n"
            "layout (location = 1) out vec3 gNormal;              \n"
            "layout (location = 2) out vec4 gAlbedoSpec;          \n"
            "                                                     \n"
            "in vec2 TexCoords;                                   \n"
            "in vec3 FragPos;                                     \n"
            "in vec3 Normal;                                      \n"
            "                                                     \n"
            "uniform sampler2D texture_diffuse1;                  \n"
            "uniform sampler2D texture_specular1;                 \n"
            "                                                     \n"
            "void main() {                                                            \n"
            "    // store the fragment position vector in the first gbuffer texture   \n"
            "    gPosition = FragPos;                                                 \n"
            "    // also store the per-fragment normals into the gbuffer              \n"
            "    gNormal = normalize(Normal);                                         \n"
            "    // and the diffuse per-fragment color                                \n"
            "    gAlbedoSpec.rgb = texture(texture_diffuse1, TexCoords).rgb;          \n"
            "    // store specular intensity in gAlbedoSpec's alpha component         \n"
            "    gAlbedoSpec.a = texture(texture_specular1, TexCoords).r;             \n"
            "}";
    char vDeferredShadingStr[] =
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
    char fDeferredShadingStr[] =
            "#version 300 es                                                \n"
            "out vec4 FragColor;                                            \n"
            "                                                               \n"
            "in vec2 TexCoords;                                             \n"
            "                                                               \n"
            "uniform sampler2D gPosition;                                   \n"
            "uniform sampler2D gNormal;                                     \n"
            "uniform sampler2D gAlbedoSpec;                                 \n"
            "                                                               \n"
            "struct Light {                                                 \n"
            "    vec3 Position;                                             \n"
            "    vec3 Color;                                                \n"
            "                                                               \n"
            "    float Linear;                                              \n"
            "    float Quadratic;                                           \n"
            "};                                                             \n"
            "const int NR_LIGHTS = 32;                                      \n"
            "uniform Light lights[NR_LIGHTS];                               \n"
            "uniform vec3 viewPos;                                          \n"
            "                                                               \n"
            "void main() {                                                  \n"
            "    // retrieve data from gbuffer                              \n"
            "    vec3 FragPos = texture(gPosition, TexCoords).rgb;          \n"
            "    vec3 Normal = texture(gNormal, TexCoords).rgb;             \n"
            "    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;        \n"
            "    float Specular = texture(gAlbedoSpec, TexCoords).a;        \n"
            "                                                               \n"
            "    // then calculate lighting as usual                        \n"
            "    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component                     \n"
            "    vec3 viewDir  = normalize(viewPos - FragPos);                                       \n"
            "    for(int i = 0; i < NR_LIGHTS; ++i) {                                                \n"
            "        // diffuse                                                                      \n"
            "        vec3 lightDir = normalize(lights[i].Position - FragPos);                        \n"
            "        vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;     \n"
            "        // specular                                                                     \n"
            "        vec3 halfwayDir = normalize(lightDir + viewDir);                                \n"
            "        float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);                      \n"
            "        vec3 specular = lights[i].Color * spec * Specular;                              \n"
            "        // attenuation                                                                  \n"
            "        float distance = length(lights[i].Position - FragPos);                          \n"
            "        float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);\n"
            "        diffuse *= attenuation;                                                         \n"
            "        specular *= attenuation;                                                        \n"
            "        lighting += diffuse + specular;                                                 \n"
            "    }                                                                                   \n"
            "    FragColor = vec4(lighting, 1.0);                                                    \n"
            "}";

    char vLightBoxStr[] =
            "#version 300 es                                               \n"
            "layout (location = 0) in vec3 aPos;                           \n"
            "layout (location = 1) in vec3 aNormal;                        \n"
            "layout (location = 2) in vec2 aTexCoords;                     \n"
            "                                                              \n"
            "uniform mat4 projection;                                      \n"
            "uniform mat4 view;                                            \n"
            "uniform mat4 model;                                           \n"
            "                                                              \n"
            "void main() {                                                 \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);\n"
            "}";
    char fLightBoxStr[] =
            "#version 300 es                                               \n"
            "layout (location = 0) out vec4 FragColor;                     \n"
            "                                                              \n"
            "uniform vec3 lightColor;                                      \n"
            "                                                              \n"
            "void main() {                                                 \n"
            "    FragColor = vec4(lightColor, 1.0);                        \n"
            "}";

    // Setup and compile our shaders
    shaderGeometryPass = AssimpShader(gBufferVS, gBufferFS);
    shaderLightingPass = AssimpShader(vDeferredShadingStr, fDeferredShadingStr);
    shaderLightBox = AssimpShader(vLightBoxStr, fLightBoxStr);

    if (shaderGeometryPass.isAvailable()) {
        // Models
        std::string path(DEFAULT_OGL_ASSETS_DIR);
        backpack = Model(path + "/model/backpack/backpack.obj");
//        cyborg = Model(path + "/model/chinese_ancient_game_girl_role/scene.gltf");
        objectPositions.push_back(glm::vec3(-3.0,  -0.5, -3.0));
        objectPositions.push_back(glm::vec3( 0.0,  -0.5, -3.0));
        objectPositions.push_back(glm::vec3( 3.0,  -0.5, -3.0));
        objectPositions.push_back(glm::vec3(-3.0,  -0.5,  0.0));
        objectPositions.push_back(glm::vec3( 0.0,  -0.5,  0.0));
        objectPositions.push_back(glm::vec3( 3.0,  -0.5,  0.0));
        objectPositions.push_back(glm::vec3(-3.0,  -0.5,  3.0));
        objectPositions.push_back(glm::vec3( 0.0,  -0.5,  3.0));
        objectPositions.push_back(glm::vec3( 3.0,  -0.5,  3.0));
        // lighting info
        // -------------
        const unsigned int NR_LIGHTS = 32;
        srand(13);
        for (unsigned int i = 0; i < NR_LIGHTS; i++) {
            // calculate slightly random offsets
            float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
            float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
            lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
            // also calculate random color
            float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
            LOGE("DeferredShadingSample Light[%d] = (%f, %f, %f)", i, rColor, gColor, bColor);
            lightColors.push_back(glm::vec3(rColor, gColor, bColor));
        }

        // shader configuration
        // --------------------
        shaderLightingPass.use();
        shaderLightingPass.setInt("gPosition", 0);
        shaderLightingPass.setInt("gNormal", 1);
        shaderLightingPass.setInt("gAlbedoSpec", 2);
    } else {
        LOGE("DeferredShadingSample::Init create program fail");
        return;
    }
}

/**
 * RenderQuad() Renders a 1x1 quad in NDC, best used for framebuffer color targets and post-processing effects.
 */
void DeferredShadingSample::renderQuad() {
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

void DeferredShadingSample::renderCube() {
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

void DeferredShadingSample::loadImage(NativeImage *pImage) {
    LOGE("DeferredShadingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
}

void DeferredShadingSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("DeferredShadingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
}

void DeferredShadingSample::initGBuffer(int width, int height) {
    LOGE("DeferredShadingSample::initGBuffer()");
    // Set up G-Buffer
    // 3 textures:
    // 1. Positions (RGB)
    // 2. Color (RGB) + Specular (A)
    // 3. Normals (RGB)
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    // position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    // normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // color + specular color buffer
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    // create and attach depth buffer (render buffer)
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        LOGE("DeferredShadingSample: initGBuffer ---> Framebuffer not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
}

void DeferredShadingSample::draw(int screenW, int screenH) {
    LOGE("DeferredShadingSample::Draw()");

    if (!shaderLightingPass.isAvailable()) {
        LOGE("DeferredShadingSample::Draw() return");
        return;
    }
    if (firstFrame) {
        firstFrame = false;
        glEnable(GL_DEPTH_TEST);
        initGBuffer(screenW, screenH);
    }
    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / (float) screenH);
    // render
    // ------
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 1. geometry pass: render scene's geometry/color data into gbuffer
    // -----------------------------------------------------------------
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderGeometryPass.use();
    shaderGeometryPass.setMat4("projection", projection);
    shaderGeometryPass.setMat4("view", view);
    for (auto & objectPosition : objectPositions) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, objectPosition);
        model = glm::scale(model, glm::vec3(0.5f));
        shaderGeometryPass.setMat4("model", model);
        backpack.Draw(shaderGeometryPass);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    // 2. lighting pass: calculate lighting by iterating over a screen filled quad pixel-by-pixel using the gbuffer's content.
    // -----------------------------------------------------------------------------------------------------------------------
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shaderLightingPass.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    // send light relevant uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++) {
        shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        shaderLightingPass.setVec3("lights[" + std::to_string(i) + "].Color", lightColors[i]);
        // update attenuation parameters and calculate radius
        const float linear = 0.7f;
        const float quadratic = 1.8f;
        shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Linear", linear);
        shaderLightingPass.setFloat("lights[" + std::to_string(i) + "].Quadratic", quadratic);
    }
    shaderLightingPass.setVec3("viewPos", eyePosition);
    // finally render quad
    renderQuad();

    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    // ----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours you'll likely have to write to the
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the FBO's internal format).
    glBlitFramebuffer(0, 0, screenW, screenH, 0, 0, screenW, screenH, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // 3. render lights on top of scene
    // --------------------------------
    shaderLightBox.use();
    shaderLightBox.setMat4("projection", projection);
    shaderLightBox.setMat4("view", view);
    for (unsigned int i = 0; i < lightPositions.size(); i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPositions[i]);
        model = glm::scale(model, glm::vec3(0.125f));
        shaderLightBox.setMat4("model", model);
        shaderLightBox.setVec3("lightColor", lightColors[i]);
        renderCube();
    }
}

void DeferredShadingSample::changeStatus(int type, int flag) {
    farthestLightPosZ += 1.0;
    LOGE("DeferredShadingSample::changestatus far = %f", farthestLightPosZ);
}


void DeferredShadingSample::destroy() {
    if (shaderLightingPass.isAvailable()) {
        shaderLightingPass.deleteProgram();
        shaderGeometryPass.deleteProgram();
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void DeferredShadingSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("DeferredShadingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float r = 10.0;
    float lightX = r * cos(radiansY);
    float lightZ = r * sin(radiansY);
    LOGE("DeferredShadingSample, lightX = %f, lightZ = %f", lightX, lightZ);
    //lightPos = glm::vec3(lightX, 3.0f, lightZ);//1.5f, 2.0f, 3.0f
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));

    // View matrix
    eyePosition = glm::vec3 (lightX, 2,  lightZ);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void DeferredShadingSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}