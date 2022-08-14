package com.zfang.opengles3_0ndk.egl

class NativeEglRender {
    external fun nativeEglRenderInit()
    external fun nativeEglRenderSetImageData(data: ByteArray?, width: Int, height: Int)
    external fun nativeEglRenderSetIntParams(paramType: Int, param: Int)
    external fun nativeEglRenderDraw()
    external fun nativeEglRenderUnInit()
}