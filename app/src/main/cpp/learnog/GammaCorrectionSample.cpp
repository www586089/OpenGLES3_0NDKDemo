//
// Created by Thinkpad on 2022/9/4.
//

#include "GammaCorrectionSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Shader.h"

GammaCorrectionSample::GammaCorrectionSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

GammaCorrectionSample::~GammaCorrectionSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void GammaCorrectionSample::init() {
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
            "uniform sampler2D floorTexture;\n"
            "\n"
            "uniform vec3 lightPositions[4];\n"
            "uniform vec3 lightColors[4];\n"
            "uniform vec3 viewPos;\n"
            "uniform bool gamma;\n"
            "\n"
            "vec3 BlinnPhong(vec3 normal, vec3 fragPos, vec3 lightPos, vec3 lightColor)\n"
            "{\n"
            "    // diffuse\n"
            "    vec3 lightDir = normalize(lightPos - fragPos);\n"
            "    float diff = max(dot(lightDir, normal), 0.0);\n"
            "    vec3 diffuse = diff * lightColor;\n"
            "    // specular\n"
            "    vec3 viewDir = normalize(viewPos - fragPos);\n"
            "    vec3 reflectDir = reflect(-lightDir, normal);\n"
            "    float spec = 0.0;\n"
            "    vec3 halfwayDir = normalize(lightDir + viewDir);  \n"
            "    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);\n"
            "    vec3 specular = spec * lightColor;    \n"
            "    // simple attenuation\n"
            "    float max_distance = 1.5;\n"
            "    float distance = length(lightPos - fragPos);\n"
            "    float attenuation = 1.0 / (gamma ? distance * distance : distance);\n"
            "    \n"
            "    diffuse *= attenuation;\n"
            "    specular *= attenuation;\n"
            "    \n"
            "    return diffuse + specular;\n"
            "}\n"
            "\n"
            "void main()\n"
            "{           \n"
            "    vec3 color = texture(floorTexture, TexCoords).rgb;\n"
            "    vec3 lighting = vec3(0.0);\n"
            "    for(int i = 0; i < 4; ++i)\n"
            "        lighting += BlinnPhong(normalize(Normal), FragPos, lightPositions[i], lightColors[i]);\n"
            "    color *= lighting;\n"
            "    if(gamma)\n"
            "        color = pow(color, vec3(1.0/2.2));\n"
            "    FragColor = vec4(color, 1.0);\n"
            "    //FragColor = texture(floorTexture, TexCoords);\n"
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

    glGenTextures(1, &floorTextureGammaCorrected);
    glBindTexture(GL_TEXTURE_2D, floorTextureGammaCorrected);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}


void GammaCorrectionSample::loadImage(NativeImage *pImage) {
    LOGE("GammaCorrectionSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        floorImage.width = pImage->width;
        floorImage.height = pImage->height;
        floorImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &floorImage);
    }
}

void GammaCorrectionSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("GammaCorrectionSample::LoadImage pImage = %p", pImage->ppPlane[0]);
}

void GammaCorrectionSample::draw(int screenW, int screenH) {
    LOGE("GammaCorrectionSample::Draw()");

    if (!lightingShader.isAvailable()) {
        LOGE("GammaCorrectionSample::Draw() return");
        return;
    }

    LOGE("GammaCorrectionSample::Do Draw()");
    // lighting info
    // -------------
    glm::vec3 lightPositions[] = {
            glm::vec3(-3.0f, 0.0f, 0.0f),
            glm::vec3(-1.0f, 0.0f, 0.0f),
            glm::vec3 (1.0f, 0.0f, 0.0f),
            glm::vec3 (3.0f, 0.0f, 0.0f)
    };
    glm::vec3 lightColors[] = {
            glm::vec3(0.25),
            glm::vec3(0.50),
            glm::vec3(0.75),
            glm::vec3(1.00)
    };

//    glm::vec3 lightPositions = glm::vec3(-3.0f, 0.0f, 0.0f);
//    glm::vec3 lightColors = glm::vec3(0.25);

    if (firstFrame) {
        firstFrame = false;
        // configure global opengl state
        // -----------------------------
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //upload RGBA image data
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, floorImage.width, floorImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, floorImage.ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        //upload sRGBA image data
        glBindTexture(GL_TEXTURE_2D, floorTextureGammaCorrected);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, floorImage.width, floorImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, floorImage.ppPlane[0]);
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
//    lightingShader.setVec3Count("lightPositions", 4, &lightPositions[0][0]);
//    lightingShader.setVec3Count("lightPos", 4, &lightColors[0][0]);
//    lightingShader.setVec3("lightPositions", lightPositions);
//    lightingShader.setVec3("lightPos", lightColors);
    GLuint program = lightingShader.getProgram();
    glUniform3fv(glGetUniformLocation(program, "lightPositions"), 4, &lightPositions[0][0]);
    glUniform3fv(glGetUniformLocation(program, "lightColors"), 4, &lightColors[0][0]);
    lightingShader.setVec3("viewPos", eyePosition);
    lightingShader.setInt("gamma", gammaEnabled);
    // floor
    glBindVertexArray(planeVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gammaEnabled ? floorTextureGammaCorrected : floorTexture);
    lightingShader.setInt("floorTexture", 0);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(GL_NONE);
}

void GammaCorrectionSample::destroy() {
    if (lightingShader.isAvailable()) {
        lightingShader.deleteProgram();

        glDeleteVertexArrays(1, &planeVAO);
        glDeleteVertexArrays(1, &planeVBO);
        glDeleteTextures(1, &floorTexture);
        glDeleteTextures(1, &floorTextureGammaCorrected);
        NativeImageUtil::FreeNativeImage(&floorImage);
    }
}

void GammaCorrectionSample::changeStatus(int type, int flag) {
    if (0 == flag) {
        gammaEnabled = false;
    } else {
        gammaEnabled = true;
    }
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void GammaCorrectionSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("GammaCorrectionSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float lightX = 12.5f * cos(radiansY);
    float lightZ = 12.5f * sin(radiansY);
    lightPos = glm::vec3(0, 0, 0);//1.5f, 2.0f, 3.0f
    // View matrix
    eyePosition = glm::vec3(lightX, 6.0, lightZ);
//    eyePosition = glm::vec3 (0.0f, 6.0f, 12.5f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void GammaCorrectionSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}