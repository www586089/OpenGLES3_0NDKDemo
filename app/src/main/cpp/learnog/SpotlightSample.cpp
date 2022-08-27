//
// Created by Thinkpad on 2022/8/27.
//

#include "SpotlightSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"
#include "Shader.h"

#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"
#include "Shader.h"

SpotlightSample::SpotlightSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

SpotlightSample::~SpotlightSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void SpotlightSample::init() {
    if (lightingShader.isAvailable()) {
        return;
    }
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    GLfloat r = 0.3f;
    GLfloat vertices[] = {
            //vertex     normal               // texture coord
            -r, -r, -r,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
             r, -r, -r,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
             r,  r, -r,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
             r,  r, -r,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
            -r,  r, -r,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
            -r, -r, -r,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

            -r, -r,  r,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
             r, -r,  r,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,
             r,  r,  r,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
             r,  r,  r,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,
            -r,  r,  r,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
            -r, -r,  r,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

            -r,  r,  r, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
            -r,  r, -r, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
            -r, -r, -r, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -r, -r, -r, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
            -r, -r,  r, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
            -r,  r,  r, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

             r,  r,  r,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
             r,  r, -r,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
             r, -r, -r,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             r, -r, -r,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
             r, -r,  r,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
             r,  r,  r,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

            -r, -r, -r,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
             r, -r, -r,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
             r, -r,  r,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
             r, -r,  r,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
            -r, -r,  r,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
            -r, -r, -r,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

            -r,  r, -r,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
             r,  r, -r,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
             r,  r,  r,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
             r,  r,  r,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
            -r,  r,  r,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
            -r,  r, -r,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    char lightingVShaderStr[] =
            "#version 300 es                                                   \n"
            "layout (location = 0) in vec3 aPos;                               \n"
            "layout (location = 1) in vec3 aNormal;                            \n"
            "layout (location = 2) in vec2 aTexCoords;\n"
            "                                                                  \n"
            "uniform mat4 model;                                               \n"
            "uniform mat4 view;                                                \n"
            "uniform mat4 projection;                                          \n"
            "                                                                  \n"
            "out vec3 FragPos;                                                 \n"
            "out vec3 Normal;                                                  \n"
            "out vec2 TexCoords;\n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    FragPos = vec3(model * vec4(aPos, 1.0));                      \n"
            "    gl_Position = projection * view * vec4(FragPos, 1.0);         \n"
            "    Normal = aNormal;                                             \n"
            "    TexCoords = aTexCoords;\n"
            "}";

    char lightingFShaderStr[] =
            "#version 300 es                 \n"
            "out vec4 FragColor;             \n"
            "                                \n"
            "struct Material {               \n"
            "    sampler2D diffuse;          \n"
            "    sampler2D specular;         \n"
            "    float shininess;            \n"
            "};                              \n"
            "                                \n"
            "struct Light {                  \n"
            "    vec3 position;              \n"
            "    vec3 direction;             \n"
            "    float cutOff;               \n"
            "    float outerCutOff;          \n"
            "                                \n"
            "    vec3 ambient;               \n"
            "    vec3 diffuse;               \n"
            "    vec3 specular;              \n"
            "                                \n"
            "    float constant;             \n"
            "    float linear;               \n"
            "    float quadratic;            \n"
            "};                              \n"
            "                                \n"
            "in vec3 FragPos;                \n"
            "in vec3 Normal;                 \n"
            "in vec2 TexCoords;              \n"
            "                                \n"
            "uniform vec3 viewPos;           \n"
            "uniform Material material;      \n"
            "uniform Light light;            \n"
            "                                \n"
            "void main()                                                                                                                \n"
            "{                                                                                                                          \n"
            "    vec3 lightDir = normalize(light.position - FragPos);                                                                   \n"
            "                                                                                                                           \n"
            "    // check if lighting is inside the spotlight cosine                                                                      \n"
            "    float theta = dot(lightDir, normalize(-light.direction));                                                              \n"
            "                                                                                                                           \n"
            "    if(theta > light.cutOff) // remember that we're working with angles as cosines instead of degrees so a '>' is used.    \n"
            "    {                                                                                                                      \n"
            "        // ambient                                                                                                         \n"
            "        vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;                                           \n"
            "                                                                                                                           \n"
            "        // diffuse                                                                                                         \n"
            "        vec3 norm = normalize(Normal);                                                                                     \n"
            "        float diff = max(dot(norm, lightDir), 0.0);                                                                        \n"
            "        vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;                                    \n"
            "                                                                                                                           \n"
            "        // specular                                                                                                        \n"
            "        vec3 viewDir = normalize(viewPos - FragPos);                                                                       \n"
            "        vec3 reflectDir = reflect(-lightDir, norm);                                                                        \n"
            "        float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);                                          \n"
            "        vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;                                 \n"
            "                                                                                                                           \n"
            "        // attenuation                                                                                                     \n"
            "        float distance    = length(light.position - FragPos);                                                              \n"
            "        float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n"
            "                                                                                                                           \n"
            "        // remove attenuation from ambient, as otherwise at large distances the light would be darker inside               \n"
            "        // than outside the spotlight due the ambient term in the else branche                                             \n"
            "        // ambient  *= attenuation;                                                                                        \n"
            "        diffuse   *= attenuation;                                                                                          \n"
            "        specular *= attenuation;                                                                                           \n"
            "                                                                                                                           \n"
            "        vec3 result = ambient + diffuse + specular;                                                                        \n"
            "        FragColor = vec4(result, 1.0);                                                                                     \n"
            "    }                                                                                                                      \n"
            "    else                                                                                                                   \n"
            "    {                                                                                                                      \n"
            "        // else, use ambient light so scene isn't completely dark outside the spotlight.                                   \n"
            "        FragColor = vec4(light.ambient * texture(material.diffuse, TexCoords).rgb, 1.0);                                   \n"
            "    }                                                                                                                      \n"
            "}                                                                                                                          \n"
            ;

    char lightingCubeVShaderStr[] =
            "#version 300 es                                                   \n"
            "layout (location = 0) in vec3 aPos;                               \n"
            "                                                                  \n"
            "uniform mat4 model;                                               \n"
            "uniform mat4 view;                                                \n"
            "uniform mat4 projection;                                          \n"
            "                                                                  \n"
            "void main()                                                       \n"
            "{                                                                 \n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0);    \n"
            "}";
    char lightingCubeFShaderStr[] =
            "#version 300 es                                                  \n"
            "out vec4 FragColor;                                              \n"
            "                                                                 \n"
            "void main()                                                      \n"
            "{                                                                \n"
            "    FragColor = vec4(1.0); // set alle 4 vector values to 1.0    \n"
            "}";
    lightingShader = Shader(lightingVShaderStr, lightingFShaderStr);
    lightCubeShader = Shader(lightingCubeVShaderStr, lightingCubeFShaderStr);

    if (lightingShader.isAvailable() && lightCubeShader.isAvailable()) {

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

        // Config Diffuse Map
        glGenTextures(1, &textureDiffuse);
        glBindTexture(GL_TEXTURE_2D, textureDiffuse);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);


        // Config Specular Map
        glGenTextures(1, &textureSpecular);
        glBindTexture(GL_TEXTURE_2D, textureSpecular);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);

        // first, configure the cube's VAO (and VBO)
        glGenVertexArrays(1, &cubeVAO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // position attribute
        glEnableVertexAttribArray(0);
        GLuint offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (offset * sizeof(GLfloat)));
        // normal attribute
        glEnableVertexAttribArray(1);
        offset += 3;
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (offset * sizeof (GLfloat)));
        //texture coordinate
        glEnableVertexAttribArray(2);
        offset += 3;
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (offset * sizeof (GLfloat)));
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);

        // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
        glGenVertexArrays(1, &lightCubeVAO);
        glBindVertexArray(lightCubeVAO);
        // we only need to bind to the VBO (to link it with glVertexAttribPointer), no need to fill it;
        // the VBO's data already contains all we need (it's already bound, but we do it again for educational purposes)
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        offset = 0;
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*) (offset * sizeof(GLfloat)));
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);
    } else {
        LOGE("SpotlightSample::Init create program fail");
        return;
    }
}


void SpotlightSample::loadImage(NativeImage *pImage) {
    LOGE("SpotlightSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        diffuseImage.width = pImage->width;
        diffuseImage.height = pImage->height;
        diffuseImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &diffuseImage);
    }
}

void SpotlightSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("SpotlightSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        if (0 == index) {
            specularImage.width = pImage->width;
            specularImage.height = pImage->height;
            specularImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &specularImage);
        }
    }
}

void SpotlightSample::draw(int screenW, int screenH) {
    LOGE("SpotlightSample::Draw()");

    if (!lightingShader.isAvailable() || !lightCubeShader.isAvailable()) {
        LOGE("SpotlightSample::Draw() return");
        return;
    }
    LOGE("SpotlightSample::Do Draw()");
    glEnable(GL_DEPTH_TEST);
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
    lightingShader.use();
    lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("viewPos", eyePosition);
    // set materials
    lightingShader.setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
    lightingShader.setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
//    lightingShader.setVec3("material.specular", 0.5f, 0.5f, 0.5f);
    lightingShader.setFloat("material.shininess", 32.0f);
    // set Light
    glm::vec3 lightColor;
    lightColor.x = static_cast<float>(sin(radiansY * 2.0));
    lightColor.y = static_cast<float>(sin(radiansY * 0.7));
    lightColor.z = static_cast<float>(sin(radiansY * 1.3));
    glm::vec3 diffuseColor = lightColor   * glm::vec3(0.9f); // decrease the influence
    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
    lightingShader.setVec3("light.ambient",  /*ambientColor*/0.2f, 0.2f, 0.2f);
    lightingShader.setVec3("light.diffuse",  /*diffuseColor*/0.75f, 0.75f, 0.75f); // darken diffuse light a bit
    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
//    lightingShader.setVec3("light.direction", 0.0f, -1.0f, 0.0f);//-0.2f, -1.0f, -0.3f
    lightingShader.setVec3("light.position", lightPos);
    lightingShader.setVec3("light.direction", 0.0f, -1.0f, -1.0f);
    lightingShader.setFloat("light.cutOff",   glm::cos(glm::radians(12.5f)));

    lightingShader.setFloat("light.constant",  1.0f);
    lightingShader.setFloat("light.linear",    0.045f);
    lightingShader.setFloat("light.quadratic", 0.0075f);

    // world transformation
    model = glm::mat4(1.0f);
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    lightingShader.setMat4("model", model);
    // view/projection transformations
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);

    //upload RGBA image data Diffuse
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureDiffuse);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, diffuseImage.width, diffuseImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, diffuseImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //upload RGBA image data Specular
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecular);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, specularImage.width, specularImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, specularImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // render the cube
    glBindVertexArray(cubeVAO);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureDiffuse);
    lightingShader.setInt("material.diffuse", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureSpecular);
    lightingShader.setInt("material.specular", 1);

    glm::vec3 cubePositions[] = {
            glm::vec3( 0.0f,   0.0f,  0.0f),
            glm::vec3( 0.0f,   0.1f, -2.0f),
            glm::vec3( 0.0f,   0.2f, -3.0f),
            glm::vec3( 0.0f,   0.3f, -4.0f),
            glm::vec3( 0.0f,   0.4f, -6.0f),
            glm::vec3( 1.2f,   0.0f, -3.0f),
            glm::vec3(-1.2f,   0.0f, -3.0f),
            glm::vec3(-2.2f,  -2.0f, -6.0f),
            glm::vec3( 2.2f,  -2.0f, -6.0f),
            glm::vec3( 0.0f,  -8.0f, -12.0f)
    };
    for (unsigned int i = 0; i < 10; i++) {
        // calculate the model matrix for each object and pass it to shader before drawing
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        float angle = 20.0f * i;
//        model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f)); //绕Y轴旋转
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        lightingShader.setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
    glBindVertexArray(GL_NONE);


    // also draw the lamp object
    lightCubeShader.use();
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
    model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f)); //绕Y轴旋转
    model = glm::translate(model, lightPos);
    lightCubeShader.setMat4("model", model);
    lightCubeShader.setMat4("view", view);
    lightCubeShader.setMat4("projection", projection);

    glBindVertexArray(lightCubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(GL_NONE);
}

void SpotlightSample::destroy() {
    if (lightingShader.isAvailable()) {
        lightingShader.deleteProgram();
        lightCubeShader.deleteProgram();

        glDeleteBuffers(1, &VBO);
        glDeleteVertexArrays(1, &cubeVAO);
        glDeleteVertexArrays(1, &lightCubeVAO);
        glDeleteTextures(1, &textureDiffuse);
        glDeleteTextures(1, &textureSpecular);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void SpotlightSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("SpotlightSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);
    projection = glm::perspective(45.0f, ratio, 0.1f, 100.0f);
    float lightX = 1.8f * sin(radiansY);
    float lightZ = 1.8f * cos(radiansY);
    lightPos = glm::vec3(lightX, 2.5f, lightZ);//1.5f, 2.0f, 3.0f
    // View matrix
    eyePosition = glm::vec3 (0.0f, 1.0f, 3.0f);
    glm::vec3 center = glm::vec3 (0, 0, 0);
    glm::vec3 upHeader = glm::vec3 (0, 1, 0);
    view = glm::lookAt(
            eyePosition,   // Camera is at (0,0,1), in World Space
            center,        // and looks at the origin
            upHeader       // Head is up (set to 0,-1,0 to look upside-down)
    );
}

void SpotlightSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}