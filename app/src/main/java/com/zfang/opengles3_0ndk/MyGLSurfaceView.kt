package com.zfang.opengles3_0ndk

import android.content.Context
import kotlin.jvm.JvmOverloads
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import android.view.ScaleGestureDetector.OnScaleGestureListener
import android.view.ScaleGestureDetector
import android.view.MotionEvent
import android.view.View.MeasureSpec

class MyGLSurfaceView @JvmOverloads constructor(
    context: Context?,
    glRender: MyGLRender,
    attrs: AttributeSet? = null
) : GLSurfaceView(context, attrs), OnScaleGestureListener {
    private val TOUCH_SCALE_FACTOR = 180.0f / 320
    private var mPreviousY = 0f
    private var mPreviousX = 0f
    private var mXAngle = 0
    private var mYAngle = 0
    val gLRender: MyGLRender
    private var mRatioWidth = 0
    private var mRatioHeight = 0
    private val mScaleGestureDetector: ScaleGestureDetector
    private var mPreScale = 1.0f
    private var mCurScale = 1.0f
    private var mLastMultiTouchTime: Long = 0


    override fun onTouchEvent(e: MotionEvent): Boolean {
        if (e.pointerCount == 1) {
            consumeTouchEvent(e)
            val currentTimeMillis = System.currentTimeMillis()
            if (currentTimeMillis - mLastMultiTouchTime > 200) {
                val y = e.y
                val x = e.x
                when (e.action) {
                    MotionEvent.ACTION_MOVE -> {
                        val dy = y - mPreviousY
                        val dx = x - mPreviousX
                        mYAngle += (dx * TOUCH_SCALE_FACTOR).toInt()
                        mXAngle += (dy * TOUCH_SCALE_FACTOR).toInt()
                    }
                }
                mPreviousY = y
                mPreviousX = x
                when (gLRender.sampleType) {
                    SAMPLE_TYPE_FBO_LEG, SAMPLE_TYPE_COORD_SYSTEM, SAMPLE_TYPE_BASIC_LIGHTING, SAMPLE_LEARN_PHONG_BASIC, SAMPLE_LEARN_PHONG_MATERIALS,
                    SAMPLE_LEARN_PHONG_TEXTURE,
                    SAMPLE_TYPE_TRANS_FEEDBACK, SAMPLE_TYPE_MULTI_LIGHTS, SAMPLE_TYPE_DEPTH_TESTING,
                    SAMPLE_TYPE_INSTANCING, SAMPLE_TYPE_STENCIL_TESTING, SAMPLE_TYPE_PARTICLES,
                    SAMPLE_TYPE_SKYBOX, SAMPLE_TYPE_3D_MODEL, SAMPLE_TYPE_PBO, SAMPLE_TYPE_KEY_VISUALIZE_AUDIO,
                    SAMPLE_TYPE_KEY_UBO, SAMPLE_TYPE_KEY_TEXT_RENDER -> {
                        gLRender.updateTransformMatrix(
                            mXAngle.toFloat(),
                            mYAngle.toFloat(),
                            mCurScale,
                            mCurScale
                        )
                        requestRender()
                    }
                    else -> {
                    }
                }
            }
        } else {
            mScaleGestureDetector.onTouchEvent(e)
        }
        return true
    }

    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val width = MeasureSpec.getSize(widthMeasureSpec)
        val height = MeasureSpec.getSize(heightMeasureSpec)
        if (0 == mRatioWidth || 0 == mRatioHeight) {
            setMeasuredDimension(width, height)
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth)
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height)
            }
        }
    }

    fun setAspectRatio(width: Int, height: Int) {
        Log.d(TAG, "setAspectRatio() called with: width = [$width], height = [$height]")
        require(!(width < 0 || height < 0)) { "Size cannot be negative." }
        mRatioWidth = width
        mRatioHeight = height
        requestLayout()
    }

    override fun onScale(detector: ScaleGestureDetector): Boolean {
        when (gLRender.sampleType) {
            SAMPLE_TYPE_COORD_SYSTEM, SAMPLE_TYPE_BASIC_LIGHTING, SAMPLE_TYPE_INSTANCING, SAMPLE_TYPE_3D_MODEL, SAMPLE_TYPE_KEY_VISUALIZE_AUDIO, SAMPLE_TYPE_KEY_TEXT_RENDER -> {
                val preSpan = detector.previousSpan
                val curSpan = detector.currentSpan
                mCurScale = if (curSpan < preSpan) {
                    mPreScale - (preSpan - curSpan) / 200
                } else {
                    mPreScale + (curSpan - preSpan) / 200
                }
                mCurScale = Math.max(0.05f, Math.min(mCurScale, 80.0f))
                gLRender.updateTransformMatrix(
                    mXAngle.toFloat(),
                    mYAngle.toFloat(),
                    mCurScale,
                    mCurScale
                )
                requestRender()
            }
            else -> {
            }
        }
        return false
    }

    override fun onScaleBegin(detector: ScaleGestureDetector): Boolean {
        return true
    }

    override fun onScaleEnd(detector: ScaleGestureDetector) {
        mPreScale = mCurScale
        mLastMultiTouchTime = System.currentTimeMillis()
    }

    fun consumeTouchEvent(e: MotionEvent) {
        dealClickEvent(e)
        var touchX = -1f
        var touchY = -1f
        when (e.action) {
            MotionEvent.ACTION_MOVE -> {
                touchX = e.x
                touchY = e.y
            }
            MotionEvent.ACTION_UP, MotionEvent.ACTION_CANCEL -> {
                touchX = -1f
                touchY = -1f
            }
            else -> {
            }
        }
        when (gLRender.sampleType) {
            SAMPLE_TYPE_KEY_SCRATCH_CARD -> {
                gLRender.setTouchLoc(touchX, touchY)
                requestRender()
            }
            else -> {
            }
        }
        when (e.action) {
            MotionEvent.ACTION_MOVE -> {
            }
            MotionEvent.ACTION_UP -> {
            }
            MotionEvent.ACTION_CANCEL -> {
            }
            else -> {
            }
        }
    }

    fun dealClickEvent(e: MotionEvent) {
        var touchX = -1f
        var touchY = -1f
        when (e.action) {
            MotionEvent.ACTION_UP -> {
                touchX = e.x
                touchY = e.y
                run {
                    when (gLRender.sampleType) {
                        SAMPLE_TYPE_KEY_SHOCK_WAVE -> gLRender.setTouchLoc(touchX, touchY)
                        else -> {
                        }
                    }
                }
            }
            else -> {
            }
        }
    }

    companion object {
        private const val TAG = "MyGLSurfaceView"
        const val IMAGE_FORMAT_RGBA = 0x01
        const val IMAGE_FORMAT_NV21 = 0x02
        const val IMAGE_FORMAT_NV12 = 0x03
        const val IMAGE_FORMAT_I420 = 0x04
        const val IMAGE_FORMAT_YUYV = 0x05
        const val IMAGE_FORMAT_GARY = 0x06
    }

    //    public MyGLSurfaceView(Context context) {
    //        this(context, null);
    //    }
    //
    //    public MyGLSurfaceView(Context context, AttributeSet attrs) {
    //        super(context, attrs);
    //        this.setEGLContextClientVersion(2);
    //        mGLRender = new MyGLRender();
    //        /*If no setEGLConfigChooser method is called,
    //        then by default the view will choose an RGB_888 surface with a depth buffer depth of at least 16 bits.*/
    //        setEGLConfigChooser(8, 8, 8, 8, 16, 8);
    //        setRenderer(mGLRender);
    //        setRenderMode(RENDERMODE_WHEN_DIRTY);
    //        mScaleGestureDetector = new ScaleGestureDetector(context, this);
    //
    //    }
    init {
        setEGLContextClientVersion(3)
        gLRender = glRender
        /*If no setEGLConfigChooser method is called,
        then by default the view will choose an RGB_888 surface with a depth buffer depth of at least 16 bits.*/
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(gLRender)
        renderMode = RENDERMODE_WHEN_DIRTY
        mScaleGestureDetector = ScaleGestureDetector(context, this)
    }
}