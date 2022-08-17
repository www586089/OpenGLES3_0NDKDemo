#include "GLUtils.h"
#include "LogUtil.h"
#include <stdlib.h>
#include <GLES3/gl3.h>
#include <GLES/egl.h>

using namespace std;

void shaderDemo();
void maxVertexAttrib();
void maxVertexNum();

void GO_CHECK_GL_ERROR() {

}

GLuint GLUtils::LoadShader(GLenum shaderType, const char *pSource) {
    GLuint shader = 0;
    shader = glCreateShader(shaderType);
    if (shader) {
        glShaderSource(shader, 1, &pSource, NULL);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint infoLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen) {
                char *buf = (char *) malloc((size_t) infoLen);
                if (buf) {
                    glGetShaderInfoLog(shader, infoLen, NULL, buf);
                    LOGI("GLUtils::LoadShader Could not compile shader %d:\n%s\n", shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }
    return shader;
}

 GLuint GLUtils::CreateProgram(const char *pVertexShaderSource,
                     const char *pFragShaderSource,
                     GLuint &vertexShaderHandle,
                     GLuint &fragShaderHandle) {
    GLuint program = 0;
    vertexShaderHandle = LoadShader(GL_VERTEX_SHADER, pVertexShaderSource);
    if (!vertexShaderHandle) return program;

    fragShaderHandle = LoadShader(GL_FRAGMENT_SHADER, pFragShaderSource);
    if (!fragShaderHandle) return program;

    program = glCreateProgram();
    if (program) {
        glAttachShader(program, vertexShaderHandle);
        CheckGLError("glAttachShader");
        glAttachShader(program, fragShaderHandle);
        CheckGLError("glAttachShader");
        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        glDetachShader(program, vertexShaderHandle);
        glDeleteShader(vertexShaderHandle);
        vertexShaderHandle = 0;
        glDetachShader(program, fragShaderHandle);
        glDeleteShader(fragShaderHandle);
        fragShaderHandle = 0;
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char *buf = (char *) malloc((size_t) bufLength);
                if (buf) {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    LOGE("GLUtils::CreateProgram Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            DeleteProgram(program);
            program = 0;
        }
    }
    LOGE("GLUtils::CreateProgram program = %d", program);
    maxVertexNum();
    return program;
}

void GLUtils::DeleteProgram(GLuint &program) {
    LOGE("GLUtils::DeleteProgram");
    if (program) {
        glUseProgram(0);
        glDeleteProgram(program);
        program = 0;
    }
}

void maxVertexAttrib() {
    GLint maxVertexAttribs;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribs);
    LOGE("maxVertexAttrib = %d", maxVertexAttribs);
}

void CheckGLError(const char *pGLOperation) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGE("GLUtils::CheckGLError GL Operation %s() glError (0x%x)\n", pGLOperation, error);
    }
}
void shaderDemo() {
    GLboolean shaderCompiler;
    GLint numBinaryFormats;
    GLint *formats;
    // Determine if a shader compiler available
    glGetBooleanv(GL_SHADER_COMPILER, &shaderCompiler);
    LOGE("shaderDemo, shaderCompiler = %hhu", shaderCompiler);
    // Determine binary formats available
    glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &numBinaryFormats);
    formats = static_cast<GLint *>(malloc(sizeof(GLint) * numBinaryFormats));
    glGetIntegerv(GL_SHADER_BINARY_FORMATS, formats);
    // "formats" now holds the list of supported binary formats
}
void eglDemo() {
    EGLint majorVersion;
    EGLint minorVersion;
    EGLDisplay display;

    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (EGL_NO_DISPLAY == display) {
        EGLint status = eglGetError();
        LOGE("Unable to open connection to local windowing system. status = %d", status);
        return;
    }
    if (!eglInitialize(display, &majorVersion, &minorVersion)) {
        EGLint status = eglGetError();
        LOGE("Unable to initialize EGL. Handle and recover, status = %d", status);
        return;
    }

    EGLint attribList[] = {
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RED_SIZE, 5,
            EGL_GREEN_SIZE, 6,
            EGL_BLUE_SIZE, 5,
            EGL_DEPTH_SIZE, 1,
            EGL_NONE
    };
    const EGLint MaxConfigs = 10;
    EGLConfig  configs[MaxConfigs];

    EGLint numConfigs;
    if (!eglChooseConfig(display, attribList, configs, MaxConfigs, &numConfigs)) {
        //Something didn't work ... handle error situation
    } else {
        //Everything's okay, Continue to create a rendering surface.
    }

    EGLSurface surface;
    EGLNativeWindowType window;
    EGLint attribList2[] = {
            EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
            EGL_NONE
    };
    surface = eglCreateWindowSurface(display, configs[0], window, attribList2);
    if (EGL_NO_SURFACE == window) {
        switch (eglGetError()) {
            case EGL_BAD_MATCH:
                break;
            case EGL_BAD_CONFIG:
                break;
            case EGL_BAD_NATIVE_WINDOW:
                break;
            case EGL_BAD_ALLOC:
                break;
        }
    }
}

void maxVertexNum() {
    GLint maxVertexAttribtes; // n will be >= 16
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVertexAttribtes);
    LOGD("maxVertexNum: %d", maxVertexAttribtes);
}