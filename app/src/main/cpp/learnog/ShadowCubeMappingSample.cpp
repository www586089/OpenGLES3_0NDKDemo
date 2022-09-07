//
// Created by Thinkpad on 2022/9/5.
//

#include "ShadowCubeMappingSample.h"
#include <gtc/matrix_transform.hpp>
#include <vector>
#include <map>
#include <ImageDef.h>
#include "../utils/GLUtils.h"
#include "Shader.h"

ShadowCubeMappingSample::ShadowCubeMappingSample() {
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

ShadowCubeMappingSample::~ShadowCubeMappingSample() {
    NativeImageUtil::FreeNativeImage(&woodImage);
    cubeVAO = GL_NONE;
    cubeVBO = GL_NONE;
}

void ShadowCubeMappingSample::init() {
    if (simpleDepthShader.isAvailable()) {
        return;
    }
    char vShaderShadowMap[] =
            "#version 300 es                                                    \n"
            "layout (location = 0) in vec3 aPos;                                \n"
            "layout (location = 1) in vec3 aNormal;                             \n"
            "layout (location = 2) in vec2 aTexCoords;                          \n"
            "                                                                   \n"
            "out vec3 FragPos;                                                  \n"
            "out vec3 Normal;                                                   \n"
            "out vec2 TexCoords;                                                \n"
            "out vec4 FragPosLightSpace;                                        \n"
            "                                                                   \n"
            "uniform mat4 projection;                                           \n"
            "uniform mat4 view;                                                 \n"
            "uniform mat4 model;                                                \n"
            "uniform mat4 lightSpaceMatrix;                                     \n"
            "uniform bool reverseNormals;"
            "                                                                   \n"
            "void main() {                                                      \n"
            "    FragPos = vec3(model * vec4(aPos, 1.0));                       \n"
            "    if (reverseNormals) {"
            "        Normal = transpose(inverse(mat3(model))) * (-1.0 * aNormal);"
            "    } else {"
            "        Normal = transpose(inverse(mat3(model))) * aNormal; \n"
            "    }"
            "               \n"
            "    TexCoords = aTexCoords;                                        \n"
            "    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);     \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);     \n"
            "}";
    char fShaderShadowMap[] =
            "#version 300 es                                                     \n"
            "out vec4 FragColor;                                                 \n"
            "                                                                    \n"
            "in vec3 FragPos;                                                    \n"
            "in vec3 Normal;                                                     \n"
            "in vec2 TexCoords;                                                  \n"
            "in vec4 FragPosLightSpace;                                          \n"
            "                                                                    \n"
            "uniform sampler2D diffuseTexture;                                   \n"
            "uniform samplerCube depthMap;                                        \n"
            "                                                                    \n"
            "uniform vec3 lightPos;                                              \n"
            "uniform vec3 viewPos;                                               \n"
            "\n"
            "uniform float farPlane;\n"
            "uniform bool shadows;"
            "                                                                    \n"
            "float ShadowCalculation(vec4 fragPosLightSpace) {                   \n"
            "    // perform perspective divide                                   \n"
            "    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;  \n"
            "    // transform to [0,1] range                                     \n"
            "    projCoords = projCoords * 0.5 + 0.5;                            \n"
            "    // get closest depth value from light's perspective (using [0,1]\n"
            "    // range fragPosLight as coords)                                \n"
            "    float closestDepth = texture(depthMap, projCoords.xyz).r;       \n"
            "    // get depth of current fragment from light's perspective       \n"
            "    float currentDepth = projCoords.z;                              \n"
            "    if (currentDepth > 1.0) {                                       \n"
            "        return 1.0;// some problem                                  \n"
            "    }                                                               \n"
            "    // check whether current frag pos is in shadow                  \n"
            "    //float bias = 0.005;                                           \n"
            "    vec3 lightDir = normalize(lightPos - FragPos);                  \n"
            "    vec3 normal = normalize(Normal);                                \n"
            "    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);  \n"
            "    // check whether current frag pos is in shadow                  \n"
            "    float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;\n"
            "    // PCF                                                          \n"
//            "    float shadow = 0.0;                                             \n"
//            "    ivec2 size = textureSize(depthMap, 0);                         \n"
//            "    vec2 texelSize = vec2(1.0 / float(size.x), 1.0 / float(size.y));\n"
//            "                                                                    \n"
//            "    for(int x = -1; x <= 1; ++x) {                                  \n"
//            "        for(int y = -1; y <= 1; ++y) {                              \n"
//            "            float pcfDepth = texture(depthMap, projCoords.xy + vec2(x, y) * texelSize).r; \n"
//            "            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;  \n"
//            "        }                                                           \n"
//            "    }                                                               \n"
//            "    shadow /= 9.0;                                                  \n"
            "                                                                    \n"
            "    return shadow;                                                  \n"
            "}                                                                   \n"
            "                                                                    \n"
            "void main() {                                                       \n"
            "    vec3 color = texture(diffuseTexture, TexCoords).rgb;            \n"
            "    vec3 normal = normalize(Normal);                                \n"
            "    vec3 lightColor = vec3(0.6);                                    \n"
            "    // ambient                                                      \n"
            "    vec3 ambient = 0.3 * lightColor;                                \n"
            "    // diffuse                                                      \n"
            "    vec3 lightDir = normalize(lightPos - FragPos);                  \n"
            "    float diff = max(dot(lightDir, normal), 0.0);                   \n"
            "    vec3 diffuse = diff * lightColor;                               \n"
            "    // specular                                                     \n"
            "    vec3 viewDir = normalize(viewPos - FragPos);                    \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);                   \n"
            "    float spec = 0.0;                                               \n"
            "    vec3 halfwayDir = normalize(lightDir + viewDir);                \n"
            "    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);            \n"
            "    vec3 specular = spec * lightColor;                              \n"
            "    // calculate shadow                                             \n"
            "    float shadow = ShadowCalculation(FragPosLightSpace);            \n"
            "    vec3 result = ambient + (1.0 - shadow) * (diffuse + specular);  \n"
            "    vec3 lighting = result * color;                                 \n"
            "                                                                    \n"
            "    FragColor = vec4(lighting, 1.0);                                \n"
            "}";
    char vShaderShadowMapDepthStr[] =
            "#version 300 es                                                  \n"
            "layout (location = 0) in vec3 aPos;                              \n"
            "                                                                 \n"
            "uniform mat4 lightSpaceMatrix;                                   \n"
            "uniform mat4 model;                                              \n"
            "                                                                 \n"
            "void main() {                                                    \n"
            "    gl_Position = lightSpaceMatrix * model * vec4(aPos, 1.0);    \n"
            "}";
    char fShaderShadowMapDepthStr[] =
            "#version 300 es                                                  \n"
            "void main()                                                      \n"
            "{                                                                \n"
            "    //empty shader only for depth Texture                        \n"
            "}";

    char vShaderDebugQuadStr[] =
            "#version 300 es                                           \n"
            "layout (location = 0) in vec3 aPos;                       \n"
            "layout (location = 1) in vec2 aTexCoords;                 \n"
            "                                                          \n"
            "out vec2 TexCoords;                                       \n"
            "                                                          \n"
            "void main()                                               \n"
            "{                                                         \n"
            "    TexCoords = aTexCoords;                               \n"
            "    gl_Position = vec4(aPos, 1.0);                        \n"
            "}";
    char fShaderDebugQuadStr[] =
            "#version 300 es                                            \n"
            "out vec4 FragColor;                                        \n"
            "                                                           \n"
            "in vec2 TexCoords;                                         \n"
            "                                                           \n"
            "uniform sampler2D depthCubeMapTexture;                         \n"
            "uniform float nearPlane;                                   \n"
            "uniform float farPlane;                                    \n"
            "                                                           \n"
            "// required when using a perspective projection matrix     \n"
            "float LinearizeDepth(float depth) {                        \n"
            "    float z = depth * 2.0 - 1.0; // Back to NDC            \n"
            "    return (2.0 * nearPlane * farPlane) / (farPlane + nearPlane - z * (farPlane - nearPlane));\n"
            "}                                                          \n"
            "                                                           \n"
            "void main() {                                              \n"
            "    float depthValue = texture(depthCubeMapTexture, TexCoords).r;     \n"
            "    //FragColor = vec4(vec3(LinearizeDepth(depthValue) / farPlane), 1.0); // perspective\n"
            "    FragColor = vec4(vec3(depthValue), 1.0); // orthographic\n"
            "    //FragColor = texture(depthCubeMapTexture, TexCoords);     \n"
            "}";
    shader = Shader(vShaderShadowMap, fShaderShadowMap);
    simpleDepthShader = Shader(vShaderShadowMapDepthStr, fShaderShadowMapDepthStr);
    debugDepthQuadShader = Shader(vShaderDebugQuadStr, fShaderDebugQuadStr);

    if (simpleDepthShader.isAvailable()) {
        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float planeVertices[] = {
                // positions            // normals         // texcoords
                 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
                -25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
                -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

                 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
                -25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
                 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
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

        // config frame buffer begin ----->
        glGenFramebuffers(1, &depthMapFBO);
//        glGenTextures(1, &depthCubeMapTexture);
//        glBindTexture(GL_TEXTURE_2D, depthCubeMapTexture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         attach depth texture as FBO's depth buffer
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCubeMapTexture, 0);
//        GLenum drawBuffers[] = {GL_NONE};
//        glDrawBuffers(1, drawBuffers);
//        glReadBuffer(GL_NONE);


//        GLuint colorBuffer;
//        // color buffer
//        glGenTextures(1, &colorBufferTexture);
//        glBindTexture(GL_TEXTURE_2D, colorBufferTexture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        // depth buffer(render buffer)
//        GLuint rboDepth;
//        glGenRenderbuffers(1, &depthCubeMapTexture);
//        glBindRenderbuffer(GL_RENDERBUFFER, depthCubeMapTexture);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT);
//        // attach buffers
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBufferTexture, 0);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthCubeMapTexture);

//        glGenTextures(1, &depthCubeMapTexture);
//        glBindTexture(GL_TEXTURE_2D, depthCubeMapTexture);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
//        glBindTexture(GL_TEXTURE_2D, GL_NONE);
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        GLenum none = GL_NONE;
//        glDrawBuffers(1, &none);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCubeMapTexture, 0);
//
//        if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE) {
//            LOGE("ShadowCubeMappingSample::init glCheckFramebufferStatus status != GL_FRAMEBUFFER_COMPLETE");
//        }
//        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        // config frame buffer <------------

        // shader configuration
        // --------------------
        shader.use();
        shader.setInt("diffuseTexture", 0);
        shader.setInt("depthMap", 1);
        debugDepthQuadShader.use();
        debugDepthQuadShader.setInt("depthCubeMapTexture", 0);


        // Config wood Texture
        glGenTextures(1, &woodTexture);
        glBindTexture(GL_TEXTURE_2D, woodTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    } else {
        LOGE("ShadowCubeMappingSample::Init create program fail");
        return;
    }
}


void ShadowCubeMappingSample::initShadowMap() {
//    UserData *userData = esContext->userData;
    GLenum none = GL_NONE;
    GLint defaultFramebuffer = GL_NONE;

    // use 1K by 1K texture for shadow map
//    userData->shadowMapTextureWidth = userData->shadowMapTextureHeight = 1024;
    // setup fbo
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glGenTextures(1, &depthCubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTexture);
    for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
        glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_COMPONENT24, depthCubeMapTexture, 0, i);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_CO, borderColor);

    // Setup hardware comparison
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_CUBE_MAP, GL_NONE);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);

    //glFramebufferTextureLayer()
    glDrawBuffers(1, &none);
    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_CUBE_MAP, depthCubeMapTexture, 0);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, depthCubeMapTexture);

    GLint *size = NULL;
    glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, size);
    LOGE("maxRenderBufferSize = %d", *size);
    glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, size);
    LOGE("Max3DTextureSize = %d", *size);
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    switch (status) {
        case GL_FRAMEBUFFER_COMPLETE:
            LOGE("ShadowCubeMappingSample::initShadowMap Success");
            break;
        case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
            LOGE("ShadowCubeMappingSample::initShadowMap Failure, Missing attachment.");
            break;
    }
    if (GL_FRAMEBUFFER_COMPLETE != status) {
        LOGE("ShadowCubeMappingSample::initShadowMap Failure, status = %d", status);
    } else {
        LOGE("ShadowCubeMappingSample::initShadowMap Success");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
}

void ShadowCubeMappingSample::initShadowMap2() {
//    UserData *userData = esContext->userData;
    GLenum none = GL_NONE;
    GLint defaultFramebuffer = GL_NONE;

    // use 1K by 1K texture for shadow map
//    userData->shadowMapTextureWidth = userData->shadowMapTextureHeight = 1024;

    glGenTextures(1, &depthCubeMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthCubeMapTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
//    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_CO, borderColor);

    // Setup hardware comparison
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFramebuffer);
    // setup fbo
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glDrawBuffers(1, &none);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthCubeMapTexture, 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthCubeMapTexture);

    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
        LOGE("ShadowMappingSample::initShadowMap Failure");
    } else {
        LOGE("ShadowMappingSample::initShadowMap Success");
    }

//    glBindFramebuffer ( GL_FRAMEBUFFER, defaultFramebuffer );
}


void ShadowCubeMappingSample::draw(int screenW, int screenH) {
    LOGE("ShadowCubeMappingSample::Draw()");

    if (!simpleDepthShader.isAvailable()) {
        LOGE("ShadowCubeMappingSample::Draw() return");
        return;
    }
    if (firstFrame) {
        LOGE("ShadowCubeMappingSample::Draw() config fbo, depthMapFBO = %d.", depthMapFBO);
        firstFrame = false;
//        SHADOW_WIDTH = screenW;
//        SHADOW_HEIGHT = screenH;
//        // 0. create depth cubemap transformation matrices
//        // -----------------------------------------------
//        float nearPlane = 1.0f;
//        float farPlane = 25.0f;
//        glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT, nearPlane, farPlane);
//        std::vector<glm::mat4> shadowTransforms;
//        shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//        shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//        shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
//        shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
//        shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//        shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//        UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / (float) screenH);
//
//        glActiveTexture(GL_TEXTURE0);
//        glBindTexture(GL_TEXTURE_2D, woodTexture);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, woodImage.width, woodImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, woodImage.ppPlane[0]);
//        glBindTexture(GL_TEXTURE_2D, GL_NONE);
//
//        ///------------------->begin
//        GLenum none = GL_NONE;
//        glViewport(0, 0, screenW, screenH);
//        glClear(GL_DEPTH_BUFFER_BIT);
//        glGenBuffers(GL_FRAMEBUFFER, &depthMapFBO);
//        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//        glDrawBuffers(1, &none);
//        GLenum textureType = GL_TEXTURE_CUBE_MAP;
//        glGenTextures(1, &depthCubeMapTexture);
//        glBindTexture(textureType, depthCubeMapTexture);
//        glm::mat4 lightSpaceMatrix;
//        simpleDepthShader.use();
//
//        for (unsigned int i = 0; i < 6; i++) {
//            lightSpaceMatrix = shadowTransforms[i];
//            GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X + i;
//            glTexImage2D(face, 0, GL_DEPTH_COMPONENT24, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);
//            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, face, depthCubeMapTexture, 0);
//            simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
//            glActiveTexture(GL_TEXTURE0);
//            renderScene(simpleDepthShader);
//        }
//        glTexParameteri(textureType, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
//        glTexParameteri(textureType, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
//        glTexParameteri(textureType, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glTexParameteri(textureType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(textureType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//        glTexParameteri(textureType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//        glTexParameteri(textureType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
//
//        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
//        switch (status) {
//            case GL_FRAMEBUFFER_COMPLETE:
//                LOGE("ShadowCubeMappingSample::draw Success");
//                break;
//            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
//                LOGE("ShadowCubeMappingSample::draw Failure, Missing attachment.");
//                break;
//        }
//        if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER)) {
//            LOGE("ShadowCubeMappingSample::draw init framebuffer Failure, status = %d", status);
//        } else {
//            LOGE("ShadowCubeMappingSample::draw init framebuffer Success");
//        }
//
//        glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);
        /// end<-----------------------
//        initShadowMap();
        initShadowMap2();
    }
    LOGE("ShadowCubeMappingSample::Do Draw()");

    // ------
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    // 0. create depth cubemap transformation matrices
//    // -----------------------------------------------
//    float nearPlane = 1.0f;
//    float farPlane = 25.0f;
//    glm::mat4 shadowProjection = glm::perspective(glm::radians(90.0f), (float) SHADOW_WIDTH / (float) SHADOW_HEIGHT, nearPlane, farPlane);
//    std::vector<glm::mat4> shadowTransforms;
//    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
//    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
//    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//    shadowTransforms.push_back(shadowProjection * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
//    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / (float) screenH);


    // 1. render scene to depth cubemap
    // --------------------------------
//    glViewport(0, 0, screenW, screenH);
//    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//    glClear(GL_DEPTH_BUFFER_BIT);
//    simpleDepthShader.use();
//    for (unsigned int i = 0; i < 6; ++i) {
//        simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
//    }
//    simpleDepthShader.setFloat("farPlane", farPlane);
//    simpleDepthShader.setVec3("lightPos", lightPos);
//    renderScene(simpleDepthShader);
//    glBindFramebuffer(GL_FRAMEBUFFER, GL_NONE);

    // 2. render scene as normal
    // -------------------------
    glViewport(0, 0, screenW, screenH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    // set lighting uniforms
    shader.setVec3("lightPos", lightPos);
    shader.setVec3("viewPos", eyePosition);
    shader.setInt("shadows", shadows); // enable/disable shadows by pressing 'SPACE'
    shader.setFloat("farPlane", 25.0F);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, woodTexture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMapTexture);
    LOGE("debug: before renderscenen");
    renderScene(shader);


    // 3 render Depth map to quad for visual debugging
    // ---------------------------------------------
//    debugDepthQuadShader.use();
//    debugDepthQuadShader.setFloat("nearPlane", nearPlane);
//    debugDepthQuadShader.setFloat("farPlane", farPlane);
//    glActiveTexture(GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, depthCubeMapTexture);
    //renderQuad();
}

// renders the 3D scene
// --------------------
void ShadowCubeMappingSample::renderScene(const Shader &cShader) {
    // room cube
    model = glm::scale(model, glm::vec3(5.0f));
    cShader.setMat4("model", model);
    /**
     * note that we disable culling here since we render 'inside' the cube instead of
     * the usual 'outside' which throws off the normal culling methods.
     */
    glDisable(GL_CULL_FACE);
    /**
     * A small little hack to invert normals when drawing cube from the inside
     * so lighting still works.
     */
    cShader.setInt("reverseNormals", 1);
    LOGE("debug: before rendercube");
    renderCube();
    cShader.setInt("reverseNormals", 0); // and of course disable it
    glEnable(GL_CULL_FACE);
    // cubes
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(4.0f, -3.5f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    cShader.setMat4("model", model);
    LOGE("debug: before render cube1");
    renderCube();
    LOGE("debug: end render cube1");
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 3.0f, 1.0));
    model = glm::scale(model, glm::vec3(0.75f));
    cShader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-3.0f, -1.0f, 0.0));
    model = glm::scale(model, glm::vec3(0.5f));
    cShader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 1.0f, 1.5));
    model = glm::scale(model, glm::vec3(0.5f));
    cShader.setMat4("model", model);
    renderCube();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-1.5f, 2.0f, -3.0));
    model = glm::rotate(model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
    model = glm::scale(model, glm::vec3(0.75f));
    cShader.setMat4("model", model);
    LOGE("debug: before render last cube");
    renderCube();
    LOGE("debug: end render last cube");
}


// renderCube() renders a 1x1 3D cube in NDC.
// -------------------------------------------------
void ShadowCubeMappingSample::renderCube() {
    // initialize (if necessary)
    if (cubeVAO == 0) {
        float vertices[] = {
                // back face
                //positions           //normal            //texcoords
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
                 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
                -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
                -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
                // front face
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
                -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
                -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
                // left face
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
                -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
                // right face
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
                 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
                 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
                 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
                // bottom face
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
                -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
                -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
                // top face
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
                 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
                -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
                -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
}

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
void ShadowCubeMappingSample::renderQuad() {
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
    glBindVertexArray(0);
}

void ShadowCubeMappingSample::loadImage(NativeImage *pImage) {
    LOGE("ShadowCubeMappingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        woodImage.width = pImage->width;
        woodImage.height = pImage->height;
        woodImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &woodImage);
    }
}

void ShadowCubeMappingSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("ShadowCubeMappingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
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

void ShadowCubeMappingSample::destroy() {
    if (simpleDepthShader.isAvailable()) {
        simpleDepthShader.deleteProgram();

        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteBuffers(1, &cubeVBO);
        glDeleteVertexArrays(1, &planeVAO);
        glDeleteBuffers(1, &planeVBO);

        glDeleteTextures(1, &colorBufferTexture);
        glDeleteTextures(1, &woodTexture);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void ShadowCubeMappingSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("ShadowCubeMappingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);

    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    float lightX = 10.2f * cos(radiansY);
    float lightZ = 10.2f * sin(radiansY);
    // View matrix
    //eyePosition = glm::vec3 (lightX, 6.0f, lightZ);
    eyePosition = glm::vec3(0, 0, 3);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void ShadowCubeMappingSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}
