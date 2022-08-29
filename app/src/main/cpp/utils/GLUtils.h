//
// Created by Thinkpad on 2022/7/9.
//
#include <GLES3/gl3.h>

#ifndef STUDIOTEST_GLUTILS_H
#define STUDIOTEST_GLUTILS_H

class GLUtils {
public:
    static GLuint LoadShader(GLenum shaderType, const char *pSource);

    static GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                                unsigned int &vertexShaderHandle, unsigned int &fragShaderHandle);
    static GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource);

    static GLuint CreateProgramWithFeedback(
            const char *pVertexShaderSource,
            const char *pFragShaderSource,
            GLuint &vertexShaderHandle,
            GLuint &fragShaderHandle,
            const GLchar **varying,
            int varyingCount);

    static void DeleteProgram(unsigned int &program);

//    static void CheckGLError(const char *pGLOperation);
//    static void GO_CHECK_GL_ERROR();

};

void CheckGLError(const char *pGLOperation);
#endif //STUDIOTEST_GLUTILS_H
