package com.zfang.opengles3_0ndk

const val SAMPLE_TYPE = 200

const val SAMPLE_TYPE_TRIANGLE = SAMPLE_TYPE
val SAMPLE_TYPE_TEXTURE_MAP = SAMPLE_TYPE_TRIANGLE + 1
val SAMPLE_TYPE_YUV_TEXTURE_MAP = SAMPLE_TYPE_TEXTURE_MAP + 1
val SAMPLE_TYPE_VBO = SAMPLE_TYPE_YUV_TEXTURE_MAP + 1
val SAMPLE_TYPE_VAO = SAMPLE_TYPE_VBO + 1
val SAMPLE_TYPE_INSTANCED = SAMPLE_TYPE_VAO + 1
val SAMPLE_TYPE_FBO = SAMPLE_TYPE_INSTANCED + 1
val SAMPLE_TYPE_EGL = SAMPLE_TYPE_FBO + 1
val SAMPLE_TYPE_FBO_LEG = SAMPLE_TYPE_EGL + 1
val SAMPLE_TYPE_COORD_SYSTEM = SAMPLE_TYPE_FBO_LEG + 1
val SAMPLE_TYPE_BASIC_LIGHTING = SAMPLE_TYPE_COORD_SYSTEM + 1
val SAMPLE_TYPE_TRANS_FEEDBACK = SAMPLE_TYPE_BASIC_LIGHTING + 1
val SAMPLE_LEARN_OG_COLOR = SAMPLE_TYPE_TRANS_FEEDBACK + 1    //学习Open GL https://blog.csdn.net/wangxingxing321/article/details/107634466?spm=1001.2014.3001.5502
val SAMPLE_LEARN_PHONG_BASIC = SAMPLE_LEARN_OG_COLOR + 1  //https://blog.csdn.net/wangxingxing321/article/details/107635149?spm=1001.2014.3001.5502
val SAMPLE_LEARN_PHONG_MATERIALS = SAMPLE_LEARN_PHONG_BASIC + 1 //https://blog.csdn.net/wangxingxing321/article/details/107636902?spm=1001.2014.3001.5502
val SAMPLE_LEARN_PHONG_TEXTURE = SAMPLE_LEARN_PHONG_MATERIALS + 1 //https://blog.csdn.net/wangxingxing321/article/details/107637724?spm=1001.2014.3001.5502
val SAMPLE_LEARN_LIGHT_DIRECTIONAL = SAMPLE_LEARN_PHONG_TEXTURE + 1//https://blog.csdn.net/wangxingxing321/article/details/107638223?spm=1001.2014.3001.5502
val SAMPLE_POINT_LIGHT = SAMPLE_LEARN_LIGHT_DIRECTIONAL + 1//https://blog.csdn.net/wangxingxing321/article/details/107638223?spm=1001.2014.3001.5502
val SAMPLE_SPOTLIGHT = SAMPLE_POINT_LIGHT + 1//https://blog.csdn.net/wangxingxing321/article/details/107638223?spm=1001.2014.3001.5502
val SAMPLE_MULTILIGHTS = SAMPLE_SPOTLIGHT + 1 //https://blog.csdn.net/wangxingxing321/article/details/107639325?spm=1001.2014.3001.5502
val SAMPLE_TYPE_MULTI_LIGHTS = SAMPLE_MULTILIGHTS + 1
val SAMPLE_TYPE_3D_MODEL = SAMPLE_TYPE_MULTI_LIGHTS + 1
val SAMPLE_DEPTH_BUFFER_TEST = SAMPLE_TYPE_3D_MODEL + 1
val SAMPLE_STENCIL_BUFFER_TEST = SAMPLE_DEPTH_BUFFER_TEST + 1
val SAMPLE_COLOR_BLEND_TEST = SAMPLE_STENCIL_BUFFER_TEST + 1

val SAMPLE_TYPE_DEPTH_TESTING = SAMPLE_TYPE + 11
val SAMPLE_TYPE_INSTANCING = SAMPLE_TYPE + 12
val SAMPLE_TYPE_STENCIL_TESTING = SAMPLE_TYPE + 13
val SAMPLE_TYPE_BLENDING = SAMPLE_TYPE + 14
val SAMPLE_TYPE_PARTICLES = SAMPLE_TYPE + 15
val SAMPLE_TYPE_SKYBOX = SAMPLE_TYPE + 16
val SAMPLE_TYPE_PBO = SAMPLE_TYPE + 18
val SAMPLE_TYPE_KEY_BEATING_HEART = SAMPLE_TYPE + 19
val SAMPLE_TYPE_KEY_CLOUD = SAMPLE_TYPE + 20
val SAMPLE_TYPE_KEY_TIME_TUNNEL = SAMPLE_TYPE + 211
val SAMPLE_TYPE_KEY_BEZIER_CURVE = SAMPLE_TYPE + 22
val SAMPLE_TYPE_KEY_BIG_EYES = SAMPLE_TYPE + 23
val SAMPLE_TYPE_KEY_FACE_SLENDER = SAMPLE_TYPE + 24
val SAMPLE_TYPE_KEY_BIG_HEAD = SAMPLE_TYPE + 25
val SAMPLE_TYPE_KEY_ROTARY_HEAD = SAMPLE_TYPE + 26
val SAMPLE_TYPE_KEY_VISUALIZE_AUDIO = SAMPLE_TYPE + 27
val SAMPLE_TYPE_KEY_SCRATCH_CARD = SAMPLE_TYPE + 28
val SAMPLE_TYPE_KEY_AVATAR = SAMPLE_TYPE + 29
val SAMPLE_TYPE_KEY_SHOCK_WAVE = SAMPLE_TYPE + 30
val SAMPLE_TYPE_KEY_MRT = SAMPLE_TYPE + 31
val SAMPLE_TYPE_KEY_FBO_BLIT = SAMPLE_TYPE + 32
val SAMPLE_TYPE_KEY_TBO = SAMPLE_TYPE + 33
val SAMPLE_TYPE_KEY_UBO = SAMPLE_TYPE + 34
val SAMPLE_TYPE_KEY_RGB2YUYV = SAMPLE_TYPE + 35
val SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER = SAMPLE_TYPE + 36
val SAMPLE_TYPE_KEY_TEXT_RENDER = SAMPLE_TYPE + 37
val SAMPLE_TYPE_KEY_STAY_COLOR = SAMPLE_TYPE + 38
val SAMPLE_TYPE_KEY_TRANSITIONS_1 = SAMPLE_TYPE + 39
val SAMPLE_TYPE_KEY_TRANSITIONS_2 = SAMPLE_TYPE + 40
val SAMPLE_TYPE_KEY_TRANSITIONS_3 = SAMPLE_TYPE + 41
val SAMPLE_TYPE_KEY_TRANSITIONS_4 = SAMPLE_TYPE + 42
val SAMPLE_TYPE_KEY_RGB2NV21 = SAMPLE_TYPE + 43
val SAMPLE_TYPE_KEY_RGB2I420 = SAMPLE_TYPE + 44
val SAMPLE_TYPE_KEY_RGB2I444 = SAMPLE_TYPE + 45
val SAMPLE_TYPE_KEY_COPY_TEXTURE = SAMPLE_TYPE + 46
val SAMPLE_TYPE_KEY_BLIT_FRAME_BUFFER = SAMPLE_TYPE + 47
val SAMPLE_TYPE_KEY_HWBuffer = SAMPLE_TYPE + 50

val SAMPLE_TYPE_SET_TOUCH_LOC = SAMPLE_TYPE + 999
val SAMPLE_TYPE_SET_GRAVITY_XY = SAMPLE_TYPE + 1000


class MyNativeRender {

    companion object {
        init {
            System.loadLibrary("opengles3_0ndk")
        }
    }

    external fun native_OnInit()

    external fun native_OnUnInit()

    external fun native_SetImageData(format: Int, width: Int, height: Int, bytes: ByteArray?)

    external fun native_OnSurfaceCreated()

    external fun native_OnSurfaceChanged(width: Int, height: Int)

    external fun native_OnDrawFrame()

    external fun nativeSetParamsInt(type: Int, first: Int, second: Int)

    external fun updateTransformMatrix(
        rotateX: Float,
        rotateY: Float,
        scaleX: Float,
        scaleY: Float
    )

    external fun setImageDataWithIndex(index: Int, format: Int, width: Int, height: Int, bytes: ByteArray?)
}