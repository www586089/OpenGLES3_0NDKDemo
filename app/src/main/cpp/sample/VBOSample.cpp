//
// Created by Thinkpad on 2022/8/12.
//

#include "VBOSample.h"
#include "../utils/GLUtils.h"

#define VERTEX_POS_SIZE        3 // x, y and z
#define VERTEX_COLOR_SIZE      4 // r, g, b, and a

#define VERTEX_POS_INDEX       0 //shader layout loc
#define VERTEX_COLOR_INDEX     1 //shader layout loc



#define VERTEX_STRIDE         (sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE))

int drawType = 1;
VBOSample::VBOSample() {

}

VBOSample::~VBOSample() {

}

void VBOSample::loadImage(NativeImage *pImage) {
    //null implement

}
// 4 vertices, with(x,y,z) ,(r, g, b, a) per-vertex
GLfloat vertices[4 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] = {
        -0.5f,  0.5f, 0.0f,        // v0
         1.0f,  0.0f, 0.0f, 1.0f,  // c0
        -0.5f, -0.5f, 0.0f,        // v1
         0.0f,  1.0f, 0.0f, 1.0f,  // c1
         0.5f, -0.5f, 0.0f,        // v2
         0.0f,  0.0f, 1.0f, 1.0f,  // c2
         0.5f,  0.5f, 0.0f,        // v3
         0.5f,  1.0f, 1.0f, 1.0f,  // c3
};
// Index buffer data
GLushort indices[6] = {0, 1, 2, 0, 2, 3};

void VBOSample::init() {
    const char vShaderStr[] =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec4 a_color;      \n"
            "out vec4 v_color;                          \n"
            "void main()                                \n"
            "{                                          \n"
            "    v_color = a_color;                     \n"
            "    gl_Position = a_position;              \n"
            "}";


    const char fShaderStr[] =
            "#version 300 es                            \n"
            "precision mediump float;                   \n"
            "in vec4 v_color;                           \n"
            "out vec4 o_fragColor;                      \n"
            "void main()                                \n"
            "{                                          \n"
            "    o_fragColor = v_color;                 \n"
            "}";


    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);


    if (0 != drawType) {
        // Generate VBO Ids and load the VBOs with data
        glGenBuffers(2, mVBOIds);
        glBindBuffer(GL_ARRAY_BUFFER, mVBOIds[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBOIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    }
}

void VBOSample::draw(int screenW, int screenH) {
    if (m_ProgramObj == 0) {
        return;
    }

    glUseProgram(m_ProgramObj);

    if (0 == drawType) {
        //使用正常数组绘制
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(VERTEX_POS_INDEX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

        glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, vertices);
        glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, vertices + VERTEX_POS_SIZE);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

        glDisableVertexAttribArray(VERTEX_POS_INDEX);
        glDisableVertexAttribArray(VERTEX_COLOR_INDEX);
    } else {
        //使用VBO绘制
        GLuint offset = 0;
        glBindBuffer(GL_ARRAY_BUFFER, mVBOIds[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBOIds[1]);

        glEnableVertexAttribArray(VERTEX_POS_INDEX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDEX);

        glVertexAttribPointer(VERTEX_POS_INDEX, VERTEX_POS_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *) offset);
        offset += VERTEX_POS_SIZE * sizeof(GLfloat);
        glVertexAttribPointer(VERTEX_COLOR_INDEX, VERTEX_COLOR_SIZE, GL_FLOAT, GL_FALSE, VERTEX_STRIDE, (const void *) offset);


        // Draw with the VBO settings
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

        glDisableVertexAttribArray(VERTEX_POS_INDEX);
        glDisableVertexAttribArray(VERTEX_COLOR_INDEX);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void VBOSample::destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        if (0 != drawType) {
            glDeleteBuffers(2, mVBOIds);
        }
        m_ProgramObj = GL_NONE;
    }
}
