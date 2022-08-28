//
// Created by Thinkpad on 2022/8/27.
//

#include "MultiLightsSample.h"
#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"
#include "Shader.h"

#include <gtc/matrix_transform.hpp>
#include "../utils/GLUtils.h"
#include "Camera.h"
#include "Shader.h"

MultiLightsSample::MultiLightsSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

MultiLightsSample::~MultiLightsSample() {
    VBO = GL_NONE;
    lightCubeVAO = GL_NONE;

    cubeVAO = GL_NONE;
}

void MultiLightsSample::init() {
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
            "out vec4 FragColor;\n"
            "\n"
            "struct Material {\n"
            "    sampler2D diffuse;\n"
            "    sampler2D specular;\n"
            "    float shininess;\n"
            "}; \n"
            "\n"
            "struct DirLight {\n"
            "    vec3 direction;\n"
            "\t\n"
            "    vec3 ambient;\n"
            "    vec3 diffuse;\n"
            "    vec3 specular;\n"
            "};\n"
            "\n"
            "struct PointLight {\n"
            "    vec3 position;\n"
            "    \n"
            "    float constant;\n"
            "    float linear;\n"
            "    float quadratic;\n"
            "\t\n"
            "    vec3 ambient;\n"
            "    vec3 diffuse;\n"
            "    vec3 specular;\n"
            "};\n"
            "\n"
            "struct SpotLight {\n"
            "    vec3 position;\n"
            "    vec3 direction;\n"
            "    float cutOff;\n"
            "    float outerCutOff;\n"
            "  \n"
            "    float constant;\n"
            "    float linear;\n"
            "    float quadratic;\n"
            "  \n"
            "    vec3 ambient;\n"
            "    vec3 diffuse;\n"
            "    vec3 specular;       \n"
            "};\n"
            "\n"
            "#define NR_POINT_LIGHTS 4\n"
            "\n"
            "in vec3 FragPos;\n"
            "in vec3 Normal;\n"
            "in vec2 TexCoords;\n"
            "\n"
            "uniform vec3 viewPos;\n"
            "uniform DirLight dirLight;\n"
            "uniform PointLight pointLights[NR_POINT_LIGHTS];\n"
            "uniform SpotLight spotLight;\n"
            "uniform Material material;\n"
            "\n"
            "// function prototypes\n"
            "vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);\n"
            "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
            "vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);\n"
            "\n"
            "void main()                                                                                      \n"
            "{                                                                                                \n"
            "    // properties                                                                                \n"
            "    vec3 norm = normalize(Normal);                                                               \n"
            "    vec3 viewDir = normalize(viewPos - FragPos);                                                 \n"
            "                                                                                                 \n"
            "    // == =====================================================                                  \n"
            "    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight  \n"
            "    // For each phase, a calculate function is defined that calculates the corresponding color   \n"
            "    // per lamp. In the main() function we take all the calculated colors and sum them up for    \n"
            "    // this fragment's final color.                                                         \n"
            "    // == =====================================================                             \n"
            "    // phase 1: directional lighting                                                        \n"
            "    vec3 result = CalcDirLight(dirLight, norm, viewDir);                                    \n"
            "    // phase 2: point lights                                                                \n"
            "    for(int i = 0; i < NR_POINT_LIGHTS; i++)                                                \n"
            "        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);                   \n"
            "    // phase 3: spot light                                                                  \n"
            "    result += CalcSpotLight(spotLight, norm, FragPos, viewDir);                             \n"
            "                                                                                            \n"
            "    FragColor = vec4(result, 1.0);                                                          \n"
            "}                                                                                           \n"
            "\n"
            "// calculates the color when using a directional light.                                     \n"
            "vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)                                \n"
            "{                                                                                           \n"
            "    vec3 lightDir = normalize(-light.direction);                                            \n"
            "    // diffuse shading                                                                      \n"
            "    float diff = max(dot(normal, lightDir), 0.0);                                           \n"
            "    // specular shading                                                                     \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);                                           \n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);               \n"
            "    // combine results                                                                      \n"
            "    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));              \n"
            "    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));       \n"
            "    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));    \n"
            "    return (ambient + diffuse + specular);                                                  \n"
            "}                                                                                           \n"
            "\n"
            "// calculates the color when using a point light.                                           \n"
            "vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)              \n"
            "{                                                                                           \n"
            "    vec3 lightDir = normalize(light.position - fragPos);                                    \n"
            "    // diffuse shading                                                                      \n"
            "    float diff = max(dot(normal, lightDir), 0.0);                                           \n"
            "    // specular shading                                                                     \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);                                           \n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);               \n"
            "    // attenuation                                                                          \n"
            "    float distance = length(light.position - fragPos);                                      \n"
            "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));\n"
            "    // combine results                                                                      \n"
            "    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));              \n"
            "    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));       \n"
            "    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));    \n"
            "    ambient *= attenuation;                                                                 \n"
            "    diffuse *= attenuation;                                                                 \n"
            "    specular *= attenuation;                                                                \n"
            "    return (ambient + diffuse + specular);                                                  \n"
            "}                                                                                           \n"
            "                                                                                            \n"
            "// calculates the color when using a spot light.                                            \n"
            "vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)                \n"
            "{                                                                                           \n"
            "    vec3 lightDir = normalize(light.position - fragPos);                                    \n"
            "    // diffuse shading                                                                      \n"
            "    float diff = max(dot(normal, lightDir), 0.0);                                           \n"
            "    // specular shading                                                                     \n"
            "    vec3 reflectDir = reflect(-lightDir, normal);                                           \n"
            "    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);               \n"
            "    // attenuation                                                                          \n"
            "    float distance = length(light.position - fragPos);                                      \n"
            "    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    \n"
            "    // spotlight intensity                                                                  \n"
            "    float theta = dot(lightDir, normalize(-light.direction));                               \n"
            "    float epsilon = light.cutOff - light.outerCutOff;                                       \n"
            "    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);               \n"
            "    // combine results                                                                      \n"
            "    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));              \n"
            "    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));       \n"
            "    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));    \n"
            "    ambient *= attenuation * intensity;                                                     \n"
            "    diffuse *= attenuation * intensity;                                                     \n"
            "    specular *= attenuation * intensity;                                                    \n"
            "    return (ambient + diffuse + specular);                                                  \n"
            "}"
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
        LOGE("MultiLightsSample::Init create program fail");
        return;
    }
}


void MultiLightsSample::loadImage(NativeImage *pImage) {
    LOGE("MultiLightsSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        diffuseImage.width = pImage->width;
        diffuseImage.height = pImage->height;
        diffuseImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &diffuseImage);
    }
}

void MultiLightsSample::loadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGE("MultiLightsSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        if (0 == index) {
            specularImage.width = pImage->width;
            specularImage.height = pImage->height;
            specularImage.format = pImage->format;
            NativeImageUtil::CopyNativeImage(pImage, &specularImage);
        }
    }
}

void MultiLightsSample::draw(int screenW, int screenH) {
    LOGE("MultiLightsSample::Draw()");

    if (!lightingShader.isAvailable() || !lightCubeShader.isAvailable()) {
        LOGE("MultiLightsSample::Draw() return");
        return;
    }
    LOGE("MultiLightsSample::Do Draw()");
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
//    glm::vec3 lightColor;
//    lightColor.x = static_cast<float>(sin(radiansY * 2.0));
//    lightColor.y = static_cast<float>(sin(radiansY * 0.7));
//    lightColor.z = static_cast<float>(sin(radiansY * 1.3));
//    glm::vec3 diffuseColor = lightColor   * glm::vec3(0.9f); // decrease the influence
//    glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
//    lightingShader.setVec3("light.ambient",  /*ambientColor*/0.2f, 0.2f, 0.2f);
//    lightingShader.setVec3("light.diffuse",  /*diffuseColor*/0.75f, 0.75f, 0.75f); // darken diffuse light a bit
//    lightingShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);
//    lightingShader.setVec3("light.direction", 0.0f, -1.0f, 0.0f);//-0.2f, -1.0f, -0.3f
//    lightingShader.setVec3("light.position", lightPos);
//    lightingShader.setVec3("light.direction", 0.0f, -1.0f, -1.0f);
//    lightingShader.setFloat("light.cutOff",   glm::cos(glm::radians(12.5f)));
//    lightingShader.setFloat("light.outerCutOff", glm::cos(glm::radians(17.5f)));
//
//    lightingShader.setFloat("light.constant",  1.0f);
//    lightingShader.setFloat("light.linear",    0.045f);
//    lightingShader.setFloat("light.quadratic", 0.0075f);


    // set Light begin

    /*
       Here we set all the uniforms for the 5/6 types of lights we have. We have to set them manually and index
       the proper PointLight struct in the array to set each uniform variable. This can be done more code-friendly
       by defining light types as classes and set their values in there, or by using a more efficient uniform approach
       by using 'Uniform buffer objects', but that is something we'll discuss in the 'Advanced GLSL' tutorial.
    */
    // positions of the point lights
    glm::vec3 pointLightPositions[] = {
            glm::vec3( 0.5f,  1.5f,  -2.0f),
            glm::vec3( 0.0f, -3.3f, -4.0f),
            glm::vec3(-0.5f,  2.0f, -6.0f),
            glm::vec3( 0.0f,  2.0f, -3.0f)
    };
    // directional light
    lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
    lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
    lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
    // point light 1
    lightingShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[0].constant", 1.0f);
    lightingShader.setFloat("pointLights[0].linear", 0.09f);
    lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
    // point light 2
    lightingShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[1].constant", 1.0f);
    lightingShader.setFloat("pointLights[1].linear", 0.09f);
    lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
    // point light 3
    lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[2].constant", 1.0f);
    lightingShader.setFloat("pointLights[2].linear", 0.09f);
    lightingShader.setFloat("pointLights[2].quadratic", 0.032f);
    // point light 4
    lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("pointLights[3].constant", 1.0f);
    lightingShader.setFloat("pointLights[3].linear", 0.09f);
    lightingShader.setFloat("pointLights[3].quadratic", 0.032f);
    // spotLight
    lightingShader.setVec3("spotLight.position", lightPos);
    lightingShader.setVec3("spotLight.direction", 0.0f, -1.0f, -1.0f);
    lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    lightingShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
    lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
    lightingShader.setFloat("spotLight.constant", 1.0f);
    lightingShader.setFloat("spotLight.linear", 0.09f);
    lightingShader.setFloat("spotLight.quadratic", 0.032f);
    lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
    // set Light end



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
        model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f)); //绕Y轴旋转
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
    glDrawArrays(GL_TRIANGLES, 0, 36); // draw spotlight
    // we now draw as many light bulbs as we have point lights.
    glBindVertexArray(lightCubeVAO);
    for (unsigned int i = 0; i < 4; i++) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::rotate(model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f)); //绕Y轴旋转
        model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
        lightCubeShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(GL_NONE);

}

void MultiLightsSample::destroy() {
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
void MultiLightsSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("MultiLightsSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
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

void MultiLightsSample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
//    m_ScaleX = scaleX;
//    m_ScaleY = scaleY;
}