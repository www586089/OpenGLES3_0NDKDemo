//
// Created by Thinkpad on 2022/8/12.
//

#include "../utils/GLUtils.h"
#include "NV21TextureMapSample.h"

void NV21TextureMapSample::loadImage(NativeImage *pImage) {
    LOGE("NV21TextureMapSample::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage){
        mRenderImage.width = pImage->width;
        mRenderImage.height = pImage->height;
        mRenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &mRenderImage);
    }
}

/**
 * // YUV 图不能直接用于显示，需要转换为 RGB 格式，而 YUV 转 RGB 是一个逐像素处理的耗时操作，
    // 在 CPU 端进行转换效率过低，这时正好可以利用 GPU 强大的并行处理能力来实现 YUV 到 RGB 的转换。

    // 只赋值x 则会出现黑白照片效果
    yuv.x = texture(y_texture, v_texCoord).r;

    // 因为YUV转RGB用的是Y、U-128和V-128，texture函数返回向量值的范围是0-1.0，128表示0.5, 所以要减去0.5
    yuv.y = texture(uv_texture, v_texCoord).a-0.5;
    yuv.z = texture(uv_texture, v_texCoord).r-0.5;

    //      RGB转YUV:
    //      Y = 0.299 R + 0.587 G + 0.114 B
    //      U = - 0.1687 R - 0.3313 G + 0.5 B + 128
    //      V = 0.5 R - 0.4187 G - 0.0813 B + 128

    //      YUV转RGB
    //      R = Y + 1.402 (V - 128)
    //      G = Y - 0.34414 (U - 128) - 0.71414 (V - 128)
    //      B = Y + 1.772 (U - 128)

    //  YUV 和 RGB 的转换矩阵
    highp vec3 rgb =  mat3( 1.0,        1.0,   1.0,    //第一列
                            0.0,   -0.34414, 1.772,   //第二列
                            1.403, -0.71414,   0.0    //第三列
                        ) * yuv;
 */
void NV21TextureMapSample::init() {
    char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    char fShaderStr[] =
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec2 v_texCoord;                                 \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform sampler2D y_texture;                        \n"
            "uniform sampler2D uv_texture;                       \n"
            "void main()                                         \n"
            "{                                                   \n"
            "	vec3 yuv;										 \n"
            "   yuv.x = texture(y_texture, v_texCoord).r;    	 \n"
            "   yuv.y = texture(uv_texture, v_texCoord).a-0.5;	 \n"
            "   yuv.z = texture(uv_texture, v_texCoord).r-0.5;	 \n"
            "	highp vec3 rgb = mat3("
            "               1,       1,       	1,				 \n"
            "               0, 		-0.344, 	1.770,			 \n"
            "               1.403,  -0.714,     0) * yuv; 	     \n"
            "	outColor = vec4(rgb, 1);						 \n"
            "}                                                   \n";

    // Load the shaders and get a linked program object
    m_ProgramObj= GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);

    // Get the sampler location
    m_ySamplerLoc = glGetUniformLocation (m_ProgramObj, "y_texture" );
    m_uvSamplerLoc = glGetUniformLocation(m_ProgramObj, "uv_texture");

    //create textures
    GLuint textureIds[2] = {0};
    glGenTextures(2, textureIds);

    m_yTextureId = textureIds[0];
    m_uvTextureId = textureIds[1];
}

void NV21TextureMapSample::draw(int screenWidth, int screenHeight) {
    LOGE("NV21TextureMapSample::Draw()");

    if(m_ProgramObj == GL_NONE || m_yTextureId == GL_NONE || m_uvTextureId == GL_NONE) return;

    //upload Y plane data
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, mRenderImage.width, mRenderImage.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, mRenderImage.ppPlane[0]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //update UV plane data
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, mRenderImage.width >> 1, mRenderImage.height >> 1, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, mRenderImage.ppPlane[1]);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    GLfloat verticesCoords[] = {
            -1.0f,  0.78f, 0.0f,  // Position 0
            -1.0f, -0.78f, 0.0f,  // Position 1
             1.0f, -0.78f, 0.0f,  // Position 2
             1.0f,  0.78f, 0.0f,  // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f,  0.0f,        // TexCoord 0
            0.0f,  1.0f,        // TexCoord 1
            1.0f,  1.0f,        // TexCoord 2
            1.0f,  0.0f         // TexCoord 3
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // Use the program object
    glUseProgram (m_ProgramObj);

    // Load the vertex position
    glVertexAttribPointer (0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof (GLfloat), verticesCoords);
    // Load the texture coordinate
    glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof (GLfloat), textureCoords);

    glEnableVertexAttribArray (0);
    glEnableVertexAttribArray (1);

    // Bind the Y plane map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_yTextureId);
    // Set the Y plane sampler to texture unit to 0
    glUniform1i(m_ySamplerLoc, 0);

    // Bind the UV plane map
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_uvTextureId);
    // Set the UV plane sampler to texture unit to 1
    glUniform1i(m_uvSamplerLoc, 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

void NV21TextureMapSample::destroy() {
    if (m_ProgramObj){
        glDeleteProgram(m_ProgramObj);
        glDeleteTextures(1, &m_yTextureId);
        glDeleteTextures(1, &m_uvTextureId);
        m_ProgramObj = GL_NONE;
    }
}