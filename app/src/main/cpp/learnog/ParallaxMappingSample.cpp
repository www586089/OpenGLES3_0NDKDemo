//
// Created by Thinkpad on 2022/9/8.
//

#include "ParallaxMappingSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Shader.h"

ParallaxMappingSample::ParallaxMappingSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

ParallaxMappingSample::~ParallaxMappingSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void ParallaxMappingSample::init() {
    if (lightingShader.isAvailable()) {
        return;
    }
    char vNoNormalShaderStr[] =
            "#version 300 es                                                 \n"
            "layout (location = 0) in vec3 aPos;                             \n"
            "layout (location = 1) in vec3 aNormal;                          \n"
            "layout (location = 2) in vec2 aTexCoords;                       \n"
            "                                                                \n"
            "out vec3 FragPos;                                               \n"
            "out vec2 TexCoords;                                             \n"
            "out vec3 Normal;"
            "                                                                \n"
            "uniform mat4 projection;                                        \n"
            "uniform mat4 view;                                              \n"
            "uniform mat4 model;                                             \n"
            "                                                                \n"
            "void main() {                                                   \n"
            "    FragPos = vec3(model * vec4(aPos, 1.0));                    \n"
            "    TexCoords = aTexCoords;                                     \n"
            "    Normal = normalize(aNormal);\n"
            "                                                                \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);  \n"
            "}";

    char fNoNormalShaderStr[] =
            "#version 300 es                                                 \n"
            "out vec4 FragColor;                                             \n"
            "                                                                \n"
            "in vec3 FragPos;                                                \n"
            "in vec2 TexCoords;                                              \n"
            "in vec3 Normal;                                                 \n"
            "                                                                \n"
            "uniform sampler2D diffuseMap;                                   \n"
            "                                                                \n"
            "uniform vec3 lightPos;                                          \n"
            "uniform vec3 viewPos;                                           \n"
            "                                                                \n"
            "void main() {                                                   \n"
            "    // obtain normal from normal map in range [0,1]             \n"
            "    vec3 normal = normalize(Normal);                            \n"
            "                                                                \n"
            "    // get diffuse color                                        \n"
            "    vec3 color = texture(diffuseMap, TexCoords).rgb;            \n"
            "    // ambient                                                  \n"
            "    vec3 ambient = 0.1 * color;                                 \n"
            "    // diffuse                                                  \n"
            "    vec3 lightDir = normalize(lightPos - FragPos);              \n"
            "    float diff = max(dot(lightDir, normal), 0.0);               \n"
            "    vec3 diffuse = diff * color;                                \n"
            "    // specular                                                 \n"
            "    vec3 viewDir = normalize(viewPos - FragPos);                \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);               \n"
            "    vec3 halfwayDir = normalize(lightDir + viewDir);            \n"
            "    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);  \n"
            "                                                                \n"
            "    vec3 specular = vec3(0.2) * spec;                           \n"
            "    FragColor = vec4(ambient + diffuse + specular, 1.0);        \n"
            "}";
    char lightingVShaderStr[] =
            "#version 300 es                                                 \n"
            "layout (location = 0) in vec3 aPos;                             \n"
            "layout (location = 1) in vec3 aNormal;                          \n"
            "layout (location = 2) in vec2 aTexCoords;                       \n"
            "layout (location = 3) in vec3 aTangent;                         \n"
            "layout (location = 4) in vec3 aBitangent;                       \n"
            "                                                                \n"
            "out vec3 FragPos;                                               \n"
            "out vec2 TexCoords;                                             \n"
            "out vec3 TangentLightPos;                                       \n"
            "out vec3 TangentViewPos;                                        \n"
            "out vec3 TangentFragPos;                                        \n"
            "                                                                \n"
            "uniform mat4 projection;                                        \n"
            "uniform mat4 view;                                              \n"
            "uniform mat4 model;                                             \n"
            "                                                                \n"
            "uniform vec3 lightPos;                                          \n"
            "uniform vec3 viewPos;                                           \n"
            "                                                                \n"
            "void main() {                                                   \n"
            "    FragPos = vec3(model * vec4(aPos, 1.0));                    \n"
            "    TexCoords = aTexCoords;                                     \n"
            "                                                                \n"
            "    mat3 normalMatrix = transpose(inverse(mat3(model)));        \n"
            "    vec3 T = normalize(normalMatrix * aTangent);                \n"
            "    vec3 N = normalize(normalMatrix * aNormal);                 \n"
            "    T = normalize(T - dot(T, N) * N);                           \n"
            "    vec3 B = cross(N, T);                                       \n"
            "                                                                \n"
            "    mat3 TBN = transpose(mat3(T, B, N));                        \n"
            "    TangentLightPos = TBN * lightPos;                           \n"
            "    TangentViewPos  = TBN * viewPos;                            \n"
            "    TangentFragPos  = TBN * FragPos;                            \n"
            "                                                                \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);  \n"
            "}";

    char lightingFShaderStr[] =
            "#version 300 es                                                   \n"
            "out vec4 FragColor;                                               \n"
            "                                                                  \n"
            "in vec3 FragPos;                                                  \n"
            "in vec2 TexCoords;                                                \n"
            "in vec3 TangentLightPos;                                          \n"
            "in vec3 TangentViewPos;                                           \n"
            "in vec3 TangentFragPos;                                           \n"
            "                                                                  \n"
            "uniform sampler2D diffuseMap;                                     \n"
            "uniform sampler2D normalMap;                                      \n"
            "uniform sampler2D depthMap;                                       \n"
            "uniform float heightScale;                                        \n"
            "                                                                  \n"
            "uniform vec3 lightPos;                                            \n"
            "uniform vec3 viewPos;                                             \n"
            "                                                                  \n"
            "vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir) {              \n"
            "    float height =  texture(depthMap, texCoords).r;               \n"
            "    return texCoords - viewDir.xy * (height * heightScale);       \n"
            "}                                                                 \n"
            "                                                                  \n"
            "void main() {                                                     \n"
            "    // offset texture coordinates with Parallax Mapping           \n"
            "    vec3 viewDir = normalize(TangentViewPos - TangentFragPos);    \n"
            "    vec2 texCoords = TexCoords;                                   \n"
            "                                                                  \n"
            "    texCoords = ParallaxMapping(texCoords,  viewDir);             \n"
            "    if(texCoords.x > 1.0 || texCoords.y > 1.0                     \n"
            "        || texCoords.x < 0.0 || texCoords.y < 0.0) {              \n"
            "        discard;                                                  \n"
            "    }                                                             \n"
            "     // obtain normal from normal map in range [0,1]              \n"
            "    vec3 normal = texture(normalMap, texCoords).rgb;              \n"
            "    // transform normal vector to range [-1,1]                    \n"
            "    //// this normal is in tangent space                          \n"
            "    normal = normalize(normal * 2.0 - 1.0);                       \n"
            "                                                                  \n"
            "    // get diffuse color                                          \n"
            "    vec3 color = texture(diffuseMap, texCoords).rgb;              \n"
            "    // ambient                                                    \n"
            "    vec3 ambient = 0.1 * color;                                   \n"
            "    // diffuse                                                    \n"
            "    vec3 lightDir = normalize(TangentLightPos - TangentFragPos);  \n"
            "    float diff = max(dot(lightDir, normal), 0.0);                 \n"
            "    vec3 diffuse = diff * color;                                  \n"
            "    // specular                                                   \n"
            "  //vec3 viewDir = normalize(TangentViewPos - TangentFragPos);    \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);                 \n"
            "    vec3 halfwayDir = normalize(lightDir + viewDir);              \n"
            "    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);    \n"
            "                                                                  \n"
            "    vec3 specular = vec3(0.2) * spec;                             \n"
            "    FragColor = vec4(ambient + diffuse + specular, 1.0);          \n"
            "}";

    lightingShader = Shader(lightingVShaderStr, lightingFShaderStr);
    noNormalMappingShader = Shader(vNoNormalShaderStr, fNoNormalShaderStr);
    if (lightingShader.isAvailable()) {
        // Config Diffuse Map
        glGenTextures(1, &diffuseMap);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);


        // Config Normal Map
        glGenTextures(1, &normalMap);
        glBindTexture(GL_TEXTURE_2D, normalMap);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // Config Depth Map
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        lightingShader.use();
        lightingShader.setInt("diffuseMap", 0);
        lightingShader.setInt("normalMap", 1);
        lightingShader.setInt("depthMap", 2);

        noNormalMappingShader.use();
        noNormalMappingShader.setInt("diffuseMap", 0);
    } else {
        LOGE("ParallaxMappingSample::Init create program fail");
        return;
    }
}

void ParallaxMappingSample::renderQuad() {
    if (quadVAO == 0) {
        // positions
        glm::vec3 pos1(-1.0f,  1.0f, 0.0f);
        glm::vec3 pos2(-1.0f, -1.0f, 0.0f);
        glm::vec3 pos3( 1.0f, -1.0f, 0.0f);
        glm::vec3 pos4( 1.0f,  1.0f, 0.0f);
        // texture coordinates
        glm::vec2 uv1(0.0f, 1.0f);
        glm::vec2 uv2(0.0f, 0.0f);
        glm::vec2 uv3(1.0f, 0.0f);
        glm::vec2 uv4(1.0f, 1.0f);
        // normal vector
        glm::vec3 nm(0.0f, 0.0f, 1.0f);

        // calculate tangent/bitangent vectors of both triangles
        glm::vec3 tangent1, bitangent1;
        glm::vec3 tangent2, bitangent2;
        // triangle 1
        // ----------
        glm::vec3 edge1 = pos2 - pos1;
        glm::vec3 edge2 = pos3 - pos1;
        glm::vec2 deltaUV1 = uv2 - uv1;
        glm::vec2 deltaUV2 = uv3 - uv1;

        float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent1.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent1.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent1.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);

        bitangent1.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent1.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent1.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);

        // triangle 2
        // ----------
        edge1 = pos3 - pos1;
        edge2 = pos4 - pos1;
        deltaUV1 = uv3 - uv1;
        deltaUV2 = uv4 - uv1;

        f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

        tangent2.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
        tangent2.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
        tangent2.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);


        bitangent2.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
        bitangent2.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
        bitangent2.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);


        float quadVertices[] = {
                // positions            // normal         // texcoords  // tangent                          // bitangent
                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos2.x, pos2.y, pos2.z, nm.x, nm.y, nm.z, uv2.x, uv2.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent1.x, tangent1.y, tangent1.z, bitangent1.x, bitangent1.y, bitangent1.z,

                pos1.x, pos1.y, pos1.z, nm.x, nm.y, nm.z, uv1.x, uv1.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos3.x, pos3.y, pos3.z, nm.x, nm.y, nm.z, uv3.x, uv3.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z,
                pos4.x, pos4.y, pos4.z, nm.x, nm.y, nm.z, uv4.x, uv4.y, tangent2.x, tangent2.y, tangent2.z, bitangent2.x, bitangent2.y, bitangent2.z
        };
        // configure plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 14 * sizeof(float), (void*)(11 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(GL_NONE);
}


void ParallaxMappingSample::loadImage(NativeImage *pImage) {
    LOGE("ParallaxMappingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        diffuseImage.width = pImage->width;
        diffuseImage.height = pImage->height;
        diffuseImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &diffuseImage);
    }
}

void ParallaxMappingSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("ParallaxMappingSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        if (0 == index) {
            normalImage.width = pImage->width;
            normalImage.height = pImage->height;
            normalImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &normalImage);
        } else if (1 == index) {
            depthMapImage.width = pImage->width;
            depthMapImage.height = pImage->height;
            depthMapImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &depthMapImage);
        }
    }
}

void ParallaxMappingSample::draw(int screenW, int screenH) {
    LOGE("ParallaxMappingSample::Draw()");

    if (!lightingShader.isAvailable()) {
        LOGE("ParallaxMappingSample::Draw() return");
        return;
    }
    // render
    // ------
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    UpdateMVPMatrix(mvpMatrix, m_AngleX, m_AngleY, (float) screenW / (float) screenH);

    if (normalMapping) {
        pShader = &lightingShader;
    } else {
        pShader = &noNormalMappingShader;
    }
    // configure view/projection matrices
    pShader->use();
    pShader->setMat4("projection", projection);
    pShader->setMat4("view", view);
    // render parallax-mapped quad
    pShader->setMat4("model", model);
    pShader->setVec3("viewPos", eyePosition);
    pShader->setVec3("lightPos", lightPos);
    pShader->setFloat("heightScale", 1.0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, diffuseImage.width, diffuseImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, diffuseImage.ppPlane[0]);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, normalImage.width, normalImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, normalImage.ppPlane[0]);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, depthMapImage.width, depthMapImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, depthMapImage.ppPlane[0]);
    renderQuad();

    // render light source (simply re-renders a smaller plane at the light's position for debugging/visualization)
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, glm::vec3(0.1f));
    model = glm::rotate(model, static_cast<float>(MATH_PI / 3.0), glm::vec3(0, 1, 0));
    pShader->setMat4("model", model);
    renderQuad();
}

void ParallaxMappingSample::changeStatus(int type, int flag) {
    if (flag == 0) {
        normalMapping = true;
    } else {
        normalMapping = false;
    }
}

void ParallaxMappingSample::destroy() {
    if (lightingShader.isAvailable()) {
        lightingShader.deleteProgram();
        lightCubeShader.deleteProgram();

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightCubeVAO);
        glDeleteTextures(1, &textureDiffuse);
        glDeleteTextures(1, &textureSpecular);
    }
    pShader = NULL;
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void ParallaxMappingSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("ParallaxMappingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float lightX = 2.8f * sin(radiansY);
    float lightZ = 2.8f * cos(radiansY);
    //lightPos = glm::vec3(lightX, 3.0f, lightZ);//1.5f, 2.0f, 3.0f
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));

    // View matrix
    eyePosition = glm::vec3 (0.0f, 0.0f, 3.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void ParallaxMappingSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}