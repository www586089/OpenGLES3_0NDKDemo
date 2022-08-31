#include "../sample/TriangleSample.h"
#include "MyGLRenderContext.h"
#include "../utils/LogUtil.h"
#include "../sample/TextureMapSample.h"
#include "../sample/NV21TextureMapSample.h"
#include "../sample/VaoSample.h"
#include "../sample/VBOSample.h"
#include "../learnog/LightingColorSample.h"
#include "../learnog/PhongLightBasicSample.h"
#include "../learnog/PhongMaterialsSample.h"
#include "../learnog/PhongLightTextureSample.h"
#include "../learnog/DirectionalLightSample.h"
#include "../learnog/PointLightSample.h"
#include "../learnog/SpotlightSample.h"
#include "../learnog/MultiLightsSample.h"
#include "../learnog/DepthBufferTestSample.h"
#include "../learnog/StencilBufferTestSample.h"
#include "ColorBlendTestSample.h"
#include "FaceCullingSample.h"
#include "FrameBufferSample.h"
#include "CubeMapTextureSample.h"
#include <stdint.h>
#include <GLES3/gl3.h>
#include <GeometryInstanced.h>
#include <FBOSample.h>
#include <FBOLegLengthenSample.h>
#include <CoordinateSystemSample.h>
#include <BasicLightingSample.h>
#include <TransformFeedbackSample.h>
#include <MultiLightsSample2.h>
#include <Model3DSample.h>
#include <BlendingSample.h>

MyGLRenderContext *MyGLRenderContext::m_pContext = nullptr;

MyGLRenderContext::MyGLRenderContext() {

}

MyGLRenderContext::~MyGLRenderContext() {

}

void MyGLRenderContext::setImageData(int format, int width, int height, uint8_t *pData) {
    LOGE("MyGLRenderContext::setImageData format=%d, width=%d, height=%d, pData=%p", format, width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }

    if (pCurrentSample) {
        pCurrentSample->loadImage(&nativeImage);
    }
}

void MyGLRenderContext::setImageDataWithIndex(int index, int format, int width, int height,uint8_t *pData) {
    LOGE("MyGLRenderContext::setImageDataWithIndex index=%d, format=%d, width=%d, height=%d, pData=%p",index, format, width, height, pData);
    NativeImage nativeImage;
    nativeImage.format = format;
    nativeImage.width = width;
    nativeImage.height = height;
    nativeImage.ppPlane[0] = pData;

    switch (format) {
        case IMAGE_FORMAT_NV12:
        case IMAGE_FORMAT_NV21:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            break;
        case IMAGE_FORMAT_I420:
            nativeImage.ppPlane[1] = nativeImage.ppPlane[0] + width * height;
            nativeImage.ppPlane[2] = nativeImage.ppPlane[1] + width * height / 4;
            break;
        default:
            break;
    }

    if (nullptr != pCurrentSample) {
        pCurrentSample->loadMultiImageWithIndex(index, &nativeImage);
    }

}

void MyGLRenderContext::onSurfaceCreated() {
    LOGE("MyGLRenderContext::onSurfaceCreated");
    glClearColor(1.0f, 1.0f, 0.5f, 1.0f);
}

void MyGLRenderContext::onSurfaceChanged(int width, int height) {
    LOGE("MyGLRenderContext::onSurfaceChanged [w, h] = [%d, %d]", width, height);
    glViewport(0, 0, width, height);
    mScreenWidth = width;
    mScreenHeight = height;
}

void MyGLRenderContext::onDrawFrame() {
    LOGE("MyGLRenderContext::onDrawFrame");
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    if (nullptr != pPreviousSample) {
        LOGE("MyGLRenderContext::onDrawFrame--->destroy previous sample");
        pPreviousSample->destroy();
        delete pPreviousSample;
        pPreviousSample = nullptr;
    }

    if (nullptr != pCurrentSample) {
        LOGE("MyGLRenderContext::onDrawFrame------> draw current");
        pCurrentSample->init();
        pCurrentSample->draw(mScreenWidth, mScreenHeight);
    }
}

void MyGLRenderContext::setParamsInt(int type, int first, int second) {
    LOGE("MyGLRenderContext::SetParamsInt type = %d, first = %d, second = %d", type, first, second);
    if (type == SAMPLE_TYPE) {
        pPreviousSample = pCurrentSample;

        LOGE("MyGLRenderContext::SetParamsInt 0 pPreviousSample = %p", pPreviousSample);

        switch (first) {
            case SAMPLE_TYPE_KEY_TRIANGLE:
                pCurrentSample = new TriangleSample();
                break;
            case SAMPLE_TYPE_KEY_TEXTURE_MAP:
                pCurrentSample = new TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_YUV_TEXTURE_MAP:
                pCurrentSample = new NV21TextureMapSample();
                break;
            case SAMPLE_TYPE_KEY_VBO:
                pCurrentSample = new VBOSample();
                break;
            case SAMPLE_TYPE_KEY_VAO:
                pCurrentSample = new VaoSample();
                break;
            case SAMPLE_TYPE_KEY_INSTANCED:
                pCurrentSample = new GeometryInstanced();
                break;
            case SAMPLE_TYPE_KEY_FBO:
                pCurrentSample = new FBOSample();
                break;
            case SAMPLE_TYPE_KEY_FBO_LEG_LENGTHEN:
                pCurrentSample = new FBOLegLengthenSample();
                break;
            case SAMPLE_TYPE_KEY_COORD_SYSTEM:
                pCurrentSample = new CoordinateSystemSample();
                break;
            case SAMPLE_TYPE_KEY_BASIC_LIGHTING:
                pCurrentSample = new BasicLightingSample();
                break;
            case SAMPLE_TYPE_KEY_TRANSFORM_FEEDBACK:
                pCurrentSample = new TransformFeedbackSample();
                break;
            case SAMPLE_LEARN_OG_COLOR:
                pCurrentSample = new LightingColorSample();
                break;
            case SAMPLE_LEARN_PHONG_BASIC:
                pCurrentSample = new PhongLightBasicSample();
                break;
            case SAMPLE_LEARN_PHONG_MATERIALS:
                pCurrentSample = new PhongMaterialsSample();
                break;
            case SAMPLE_LEARN_PHONG_TEXTURE:
                pCurrentSample = new PhongLightTextureSample();
                break;
            case SAMPLE_LEARN_LIGHT_DIRECTIONAL:
                pCurrentSample = new DirectionalLightSample();
                break;
            case SAMPLE_POINT_LIGHT:
                pCurrentSample = new PointLightSample();
                break;
            case SAMPLE_SPOTLIGHT:
                pCurrentSample = new SpotlightSample();
                break;
            case SAMPLE_MULTILIGHTS:
                pCurrentSample = new MultiLightsSample();
                break;
            case SAMPLE_TYPE_KEY_MULTI_LIGHTS:
                pCurrentSample = new MultiLightsSample2();
                break;
            case SAMPLE_TYPE_KEY_3D_MODEL:
                pCurrentSample = new Model3DSample();
                break;
            case SAMPLE_DEPTH_BUFFER_TEST:
                pCurrentSample = new DepthBufferTestSample();
                break;
            case SAMPLE_STENCIL_BUFFER_TEST:
                pCurrentSample = new StencilBufferTestSample();
                break;
            case SAMPLE_COLOR_BLEND_TEST:
                pCurrentSample = new ColorBlendTestSample();
                break;
            case SAMPLE_TYPE_KEY_BLENDING:
                pCurrentSample = new BlendingSample();
                break;
            case SAMPLE_FACE_CULLING:
                pCurrentSample = new FaceCullingSample();
                break;
            case SAMPLE_FRAME_BUFFER:
                pCurrentSample = new FrameBufferSample();
                break;
            case SAMPLE_CUBE_MAP_TEXTURE:
                pCurrentSample = new CubeMapTextureSample();
                break;
//            case SAMPLE_TYPE_KEY_DEPTH_TESTING:
//                pCurrentSample = new DepthTestingSample();
//                break;
//            case SAMPLE_TYPE_KEY_INSTANCING:
//                pCurrentSample = new Instancing3DSample();
//                break;
//            case SAMPLE_TYPE_KEY_STENCIL_TESTING:
//                pCurrentSample = new StencilTestingSample();
//                break;

//            case SAMPLE_TYPE_KEY_PARTICLES:
//                pCurrentSample = new ParticlesSample();
//                break;
//            case SAMPLE_TYPE_KEY_SKYBOX:
//                pCurrentSample = new SkyBoxSample();
//                break;
//            case SAMPLE_TYPE_KEY_PBO:
//                pCurrentSample = new PBOSample();
//                break;
//            case SAMPLE_TYPE_KEY_BEATING_HEART:
//                pCurrentSample = new BeatingHeartSample();
//                break;
//            case SAMPLE_TYPE_KEY_CLOUD:
//                pCurrentSample = new CloudSample();
//                break;
//            case SAMPLE_TYPE_KEY_TIME_TUNNEL:
//                pCurrentSample = new TimeTunnelSample();
//                break;
//            case SAMPLE_TYPE_KEY_BEZIER_CURVE:
//                pCurrentSample = new BezierCurveSample();
//                break;
//            case SAMPLE_TYPE_KEY_BIG_EYES:
//                pCurrentSample = new BigEyesSample();
//                break;
//            case SAMPLE_TYPE_KEY_FACE_SLENDER:
//                pCurrentSample = new FaceSlenderSample();
//                break;
//            case SAMPLE_TYPE_KEY_BIG_HEAD:
//                pCurrentSample = new BigHeadSample();
//                break;
//            case SAMPLE_TYPE_KEY_RATARY_HEAD:
//                pCurrentSample = new RotaryHeadSample();
//                break;
//            case SAMPLE_TYPE_KEY_VISUALIZE_AUDIO:
//                pCurrentSample = new VisualizeAudioSample();
//                break;
//            case SAMPLE_TYPE_KEY_SCRATCH_CARD:
//                pCurrentSample = new ScratchCardSample();
//                break;
//            case SAMPLE_TYPE_KEY_AVATAR:
//                pCurrentSample = new AvatarSample();
//                break;
//            case SAMPLE_TYPE_KEY_SHOCK_WAVE:
//                pCurrentSample = new ShockWaveSample();
//                break;
//            case SAMPLE_TYPE_KEY_MRT:
//                pCurrentSample = new MRTSample();
//                break;
//            case SAMPLE_TYPE_KEY_FBO_BLIT:
//                pCurrentSample = new FBOBlitSample();
//                break;
//            case SAMPLE_TYPE_KEY_TBO:
//                pCurrentSample = new TextureBufferSample();
//                break;
//            case SAMPLE_TYPE_KEY_UBO:
//                pCurrentSample = new UniformBufferSample();
//                break;
//            case SAMPLE_TYPE_KEY_RGB2YUYV:
//                pCurrentSample = new RGB2YUYVSample();
//                break;
//            case SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER:
//                pCurrentSample = new SharedEGLContextSample();
//                break;
//            case SAMPLE_TYPE_KEY_TEXT_RENDER:
//                pCurrentSample = new TextRenderSample();
//                break;
//            case SAMPLE_TYPE_KEY_STAY_COLOR:
//                pCurrentSample = new PortraitStayColorExample();
//                break;
//            case SAMPLE_TYPE_KEY_TRANSITIONS_1:
//                pCurrentSample = new GLTransitionExample();
//                break;
//            case SAMPLE_TYPE_KEY_TRANSITIONS_2:
//                pCurrentSample = new GLTransitionExample_2();
//                break;
//            case SAMPLE_TYPE_KEY_TRANSITIONS_3:
//                pCurrentSample = new GLTransitionExample_3();
//                break;
//            case SAMPLE_TYPE_KEY_TRANSITIONS_4:
//                pCurrentSample = new GLTransitionExample_4();
//                break;
//            case SAMPLE_TYPE_KEY_RGB2NV21:
//                pCurrentSample = new RGB2NV21Sample();
//                break;
//            case SAMPLE_TYPE_KEY_RGB2I420:
//                pCurrentSample = new RGB2I420Sample();
//                break;
//            case SAMPLE_TYPE_KEY_RGB2I444:
//                pCurrentSample = new RGB2I444Sample();
//                break;
//            case SAMPLE_TYPE_KEY_COPY_TEXTURE:
//                pCurrentSample = new CopyTextureExample();
//                break;
//            case SAMPLE_TYPE_KEY_BLIT_FRAME_BUFFER:
//                pCurrentSample = new BlitFrameBufferExample();
//                break;
            default:
                pCurrentSample = nullptr;
                break;
        }

        LOGE("MyGLRenderContext::SetParamsInt pPreviousSample = %p, pCurrentSample=%p", pPreviousSample, pCurrentSample);
    }
}

void MyGLRenderContext::updateTransformMatrix(float rotateX, float rotateY, float scaleX,float scaleY) {
    LOGE("MyGLRenderContext::updateTransformMatrix [rotateX, rotateY, scaleX, scaleY] = [%f, %f, %f, %f]", rotateX, rotateY, scaleX, scaleY);
    if (nullptr != pCurrentSample) {
        pCurrentSample->updateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    }
}


MyGLRenderContext *MyGLRenderContext::GetInstance() {
    LOGE("MyGLRenderContext::GetInstance");
    if (m_pContext == nullptr) {
        m_pContext = new MyGLRenderContext();
    }
    return m_pContext;
}

void MyGLRenderContext::DestroyInstance() {
    LOGE("MyGLRenderContext::DestroyInstance");
    if (m_pContext) {
        delete m_pContext;
        m_pContext = nullptr;
    }

}