//
// Created by Thinkpad on 2022/9/3.
//
#include <gtc/matrix_transform.hpp>
#include "InstancingAsteroidArraySample.h"
#include "../utils/GLUtils.h"

InstancingAsteroidArraySample::InstancingAsteroidArraySample() {
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    planet = nullptr;
    m_pShader = nullptr;
}

InstancingAsteroidArraySample::~InstancingAsteroidArraySample() {

}

void InstancingAsteroidArraySample::init() {
    if(planet != nullptr && m_pShader != nullptr)
        return;

    char vShaderStr[] =
            "#version 300 es                                   \n"
            "precision mediump float;                          \n"
            "layout (location = 0) in vec3 a_position;         \n"
            "layout (location = 1) in vec3 a_normal;           \n"
            "layout (location = 2) in vec2 a_texCoord;         \n"
            "out vec2 v_texCoord;                              \n"
            "uniform mat4 u_MVPMatrix;                         \n"
            "uniform mat4 u_ModelMatrix;                       \n"
            "uniform vec3 lightPos;                            \n"
            "uniform vec3 lightColor;                          \n"
            "uniform vec3 viewPos;                             \n"
            "out vec3 ambient;                                 \n"
            "out vec3 diffuse;                                 \n"
            "out vec3 specular;                                \n"
            "void main()                                                                    \n"
            "{                                                                              \n"
            "    v_texCoord = a_texCoord;                                                   \n"
            "    vec4 position = vec4(a_position, 1.0);                                     \n"
            "    gl_Position = u_MVPMatrix * position;                                      \n"
            "    vec3 fragPos = vec3(u_ModelMatrix * position);                             \n"
            "                                                                               \n"
            "    // Ambient                                                                 \n"
            "    float ambientStrength = 0.25;                                              \n"
            "    ambient = ambientStrength * lightColor;                                    \n"
            "                                                                               \n"
            "    // Diffuse                                                                 \n"
            "    float diffuseStrength = 0.5;                                               \n"
            "    vec3 unitNormal = normalize(vec3(u_ModelMatrix * vec4(a_normal, 1.0)));    \n"
            "    vec3 lightDir = normalize(lightPos - fragPos);                             \n"
            "    float diff = max(dot(unitNormal, lightDir), 0.0);                          \n"
            "    diffuse = diffuseStrength * diff * lightColor;                             \n"
            "                                                                               \n"
            "    // Specular                                                                \n"
            "    float specularStrength = 0.3;                                              \n"
            "    vec3 viewDir = normalize(viewPos - fragPos);                               \n"
            "    vec3 reflectDir = reflect(-lightDir, unitNormal);                          \n"
            "    float spec = pow(max(dot(unitNormal, reflectDir), 0.0), 16.0);             \n"
            "    specular = specularStrength * spec * lightColor;                           \n"
            "}";

    char fShaderStr[] =
            "#version 300 es                                                              \n"
            "precision mediump float;                                                     \n"
            "out vec4 outColor;                                                           \n"
            "in vec2 v_texCoord;                                                          \n"
            "in vec3 ambient;                                                             \n"
            "in vec3 diffuse;                                                             \n"
            "in vec3 specular;                                                            \n"
            "uniform sampler2D texture_diffuse1;                                          \n"
            "void main()                                                                  \n"
            "{                                                                            \n"
            "    vec4 objectColor = texture(texture_diffuse1, v_texCoord);                \n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);    \n"
            "    outColor = vec4(finalColor, 1.0);                                        \n"
            "}";

    char fNoTextureShaderStr[] =
            "#version 300 es                                                            \n"
            "precision highp float;                                                     \n"
            "out vec4 outColor;                                                         \n"
            "in vec3 ambient;                                                           \n"
            "in vec3 diffuse;                                                           \n"
            "in vec3 specular;                                                          \n"
            "void main()                                                                \n"
            "{                                                                          \n"
            "    vec4 objectColor = vec4(0.6, 0.6, 0.6, 1.0);                           \n"
            "    vec3 finalColor = (ambient + diffuse + specular) * vec3(objectColor);  \n"
            "    outColor = vec4(finalColor, 1.0);                                      \n"
            "}";
    char rockVShaderStr[] =
            "#version 300 es\n"
            "layout (location = 0) in vec3 aPos;\n"
            "layout (location = 2) in vec2 aTexCoords;\n"
            "\n"
            "out vec2 TexCoords;\n"
            "\n"
            "uniform mat4 projection;\n"
            "uniform mat4 view;\n"
            "uniform mat4 model;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    TexCoords = aTexCoords;\n"
            "    gl_Position = projection * view * model * vec4(aPos, 1.0f); \n"
            "}";

    char rockFShaderStr[] =
            "#version 300 es\n"
            "out vec4 FragColor;\n"
            "\n"
            "in vec2 TexCoords;\n"
            "\n"
            "uniform sampler2D texture_diffuse1;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    FragColor = texture(texture_diffuse1, TexCoords);\n"
            "}";
    //TODO 先把 model 文件夹拷贝到 /sdcard/Android/data/com.byteflow.app/files/Download 路径下，然后可以选择你要加载的模型
    std::string path(DEFAULT_OGL_ASSETS_DIR);
//    planet = new Model(path + "/model/lucy_heartfilia_praying/scene.gltf");
//    planet = new Model(path + "/model/skull/scene.gltf");
//    planet = new Model(path + "/model/chinese_ancient_game_girl_role/scene.gltf");
    //planet = new Model(path + "/model/poly/Apricot_02_hi_poly.obj");
    //planet = new Model(path + "/model/backpack/backpack.obj");
    //planet = new Model(path + "/model/moon/scene.gltf");//moon
//    planet = new Model(path + "/model/artificial_womb/scene.gltf");//error
//    planet = new Model(path + "/model/girl_swimsuit/scene.gltf");
//    planet = new Model(path + "/model/black_dragon_with_idle_animation/scene.gltf");
    planet = new Model(path + "/model/planet/planet.obj");
    rock = new Model(path + "/model/rock/rock.obj");

    if (planet->ContainsTextures()) {
        m_pShader = new AssimpShader(vShaderStr, fShaderStr);
    } else {
        m_pShader = new AssimpShader(vShaderStr, fNoTextureShaderStr);
    }

    modelMatrices = new glm::mat4[amount];
    srand(static_cast<unsigned int>(GetSysCurrentTimeSec())); // initialize random seed
    float radius = 50.0;
    float offset = 2.5f;
    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        // 1. translation: displace along circle with 'radius' in range [-offset, offset]
        float angle = (float)i / (float)amount * 360.0f;
        float displacement = (float) (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float x = sin(angle) * radius + displacement;
        displacement = (float) (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float y = displacement * 0.4f; // keep height of asteroid field smaller compared to width of x and z
        displacement = (float) (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
        float z = cos(angle) * radius + displacement;
        model = glm::translate(model, glm::vec3(x, y, z));

        // 2. scale: Scale between 0.05 and 0.25f
        auto scale = static_cast<float>((rand() % 20) / 100.0 + 0.05);
        model = glm::scale(model, glm::vec3(scale));

        // 3. rotation: add random rotation around a (semi)randomly picked rotation axis vector
        auto rotAngle = static_cast<float>((rand() % 360));
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        LOGE("modelMatrices[%d] = (%f, %f, %f)" , i, x, y, z);
        // 4. now add to list of matrices
        modelMatrices[i] = model;
    }
}

void InstancingAsteroidArraySample::loadImage(NativeImage *pImage) {
    LOGE("InstancingAsteroidArraySample::LoadImage pImage = %p", pImage->ppPlane[0]);
}

void InstancingAsteroidArraySample::draw(int screenW, int screenH) {
    if(planet == nullptr || m_pShader == nullptr) return;
    LOGE("InstancingAsteroidArraySample::Draw()");
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);



    m_pShader->use();

    // draw planet
    m_ModelMatrix = glm::mat4(1.0f);
    m_ModelMatrix = glm::translate(m_ModelMatrix, glm::vec3(0.0f, -3.0f, 0.0f));
    m_ModelMatrix = glm::scale(m_ModelMatrix, glm::vec3(4.0f, 4.0f, 4.0f));
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);

    m_pShader->setMat4("u_MVPMatrix", m_MVPMatrix);
    m_pShader->setMat4("u_ModelMatrix", m_ModelMatrix);
    m_pShader->setVec3("lightPos", glm::vec3(0, 0, planet->GetMaxViewDistance()));
    m_pShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_pShader->setVec3("viewPos", glm::vec3(0, 0, planet->GetMaxViewDistance()));
    planet->Draw((*m_pShader));

    // draw meteorites
    for (unsigned int i = 0; i < amount; i++) {
        m_ModelMatrix = modelMatrices[i];
        UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);
        m_pShader->setMat4("u_MVPMatrix", m_MVPMatrix);
        m_pShader->setMat4("u_ModelMatrix", m_ModelMatrix);
        m_pShader->setVec3("lightPos", glm::vec3(0, 0, rock->GetMaxViewDistance()));
        m_pShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
        m_pShader->setVec3("viewPos", glm::vec3(0, 0, rock->GetMaxViewDistance()));
        rock->Draw((*m_pShader));
    }
}

void InstancingAsteroidArraySample::destroy() {
    LOGE("InstancingAsteroidArraySample::Destroy");
    if (planet != nullptr) {
        planet->Destroy();
        delete planet;
        planet = nullptr;
    }

    if (m_pShader != nullptr) {
        m_pShader->Destroy();
        delete m_pShader;
        m_pShader = nullptr;
    }
}

void InstancingAsteroidArraySample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGE("InstancingAsteroidArraySample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
//    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, planet->GetMaxViewDistance() * 4);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    float radius = 55.0f;
    float x = radius * cos(radiansY);
    float y = 2.0f;
    float z = radius * sin(radiansY);
    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(x, y, z),/*planet->GetMaxViewDistance() * 1.8f*/ // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );
//    glm::mat4 View = glm::lookAt(
//            glm::vec3(0, 0, 55/*planet->GetMaxViewDistance() * 1.8f*/), // Camera is at (0,0,1), in World Space
//            glm::vec3(0, 0, 0), // and looks at the origin
//            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
//    );

    // Model matrix
//    glm::mat4 Model = glm::mat4(1.0f);
//    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
//    Model = glm::scale(Model, glm::vec3(4.0f, 4.0f, 4.0f));
//    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
//    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
//    Model = glm::translate(Model, -planet->GetAdjustModelPosVec());
//    Model = glm::translate(Model, glm::vec3(0.0f, -3.0f, 0.0f));

//    m_ModelMatrix = Model;
//    m_ModelMatrix = glm::rotate(m_ModelMatrix, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    mvpMatrix = Projection * View * m_ModelMatrix;
}

void InstancingAsteroidArraySample::updateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLSampleBase::updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    LOGE("InstancingAsteroidArraySample::updateTransformMatrix, rotateX = %f", rotateX);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

