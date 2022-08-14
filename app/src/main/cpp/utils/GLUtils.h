//
// Created by Thinkpad on 2022/7/9.
//
#include <GLES3/gl3.h>

#ifndef STUDIOTEST_GLUTILS_H
#define STUDIOTEST_GLUTILS_H

GLuint LoadShader(GLenum shaderType, const char *pSource);

GLuint CreateProgram(const char *pVertexShaderSource, const char *pFragShaderSource,
                           unsigned int &vertexShaderHandle, unsigned int &fragShaderHandle);

void DeleteProgram(unsigned int &program);

void CheckGLError(const char *pGLOperation);
void GO_CHECK_GL_ERROR();
#endif //STUDIOTEST_GLUTILS_H
