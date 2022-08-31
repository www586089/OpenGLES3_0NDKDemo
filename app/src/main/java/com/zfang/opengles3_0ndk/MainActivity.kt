package com.zfang.opengles3_0ndk

import android.Manifest
import android.app.AlertDialog
import android.content.Intent
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.Log
import android.view.*
import android.view.ViewTreeObserver.OnGlobalLayoutListener
import android.widget.Button
import android.widget.RelativeLayout
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.zfang.opengles3_0ndk.MyGLSurfaceView.Companion.IMAGE_FORMAT_GARY
import com.zfang.opengles3_0ndk.MyGLSurfaceView.Companion.IMAGE_FORMAT_NV21
import com.zfang.opengles3_0ndk.MyGLSurfaceView.Companion.IMAGE_FORMAT_RGBA
import com.zfang.opengles3_0ndk.adapter.MyRecyclerViewAdapter
import com.zfang.opengles3_0ndk.audio.AudioCollector
import com.zfang.opengles3_0ndk.egl.EGLActivity
import java.io.IOException
import java.io.InputStream
import java.nio.ByteBuffer

class MainActivity : AppCompatActivity(), AudioCollector.Callback, OnGlobalLayoutListener,
    SensorEventListener {
    private val TAG = "MainActivity"
    private val REQUEST_PERMISSIONS = arrayOf(
        Manifest.permission.WRITE_EXTERNAL_STORAGE,
        Manifest.permission.RECORD_AUDIO
    )
    private val PERMISSION_REQUEST_CODE = 1
    private val SAMPLE_TITLES = listOf<String>(
        "DrawTriangle",
        "TextureMap",
        "YUV Rendering",
        "VBO",
        "VAO&VBO",
        "Geometry Instanced",
        "FBO Offscreen Rendering",
        "EGL Background Rendering",
        "FBO Stretching",
        "Coordinate System",
        "Basic Lighting",
        "Transform Feedback",
        "Learn OpenGL Colors",
        "Learn OpenGL Phong Basic",
        "Learn OpenGL Materials",
        "Learn OpenGL Diffuse Texture",
        "Learn OpenGL Directional Light",
        "Learn OpenGL Point Light",
        "Learn OpenGL SpotLight",
        "Learn OpenGL MultiLights",
        "Complex Lighting",
        "Assimp Load 3D Model",
        "Depth Buffer Test",
        "Stencil Buffer Test",
        "Color Blend Test",
        "Blending",
        "Face Culling",
        "Frame Buffer",
        "Depth Testing",
        "Instancing",
        "Stencil Testing",
        "Particles",
        "SkyBox",
        "PBO",
        "Beating Heart",
        "Cloud",
        "Time Tunnel",
        "Bezier Curve",
        "Big Eyes",
        "Face Slender",
        "Big Head",
        "Rotary Head",
        "Visualize Audio",
        "Scratch Card",
        "3D Avatar",
        "Shock Wave",
        "MRT",
        "FBO Blit",
        "Texture Buffer",
        "Uniform Buffer",
        "RGB to YUYV",
        "Multi-Thread Render",
        "Text Render",
        "Portrait stay color",
        "GL Transitions_1",
        "GL Transitions_2",
        "GL Transitions_3",
        "GL Transitions_4",
        "RGB to NV21",
        "RGB to I420",
        "RGB to I444",
        "Copy Texture",
        "Blit Frame Buffer"
    )

    private lateinit var mGLSurfaceView: MyGLSurfaceView
    private lateinit var mRootView: ViewGroup
    private val defaultSampleType = SAMPLE_TYPE_TRIANGLE - SAMPLE_TYPE
    private var mSampleSelectedIndex: Int = defaultSampleType
    private lateinit var mAudioCollector: AudioCollector
    private val mGLRender: MyGLRender = MyGLRender()
    private var mSensorManager: SensorManager? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        mRootView = findViewById<View>(R.id.rootView) as ViewGroup
        mRootView.viewTreeObserver.addOnGlobalLayoutListener(this)
        mSensorManager = getSystemService(SENSOR_SERVICE) as SensorManager
        mGLRender.init()
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) {
//            initDefaultSample()
        }
    }

    private fun initDefaultSample() {
        supportActionBar?.title = SAMPLE_TITLES[mSampleSelectedIndex]
        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY)
        mGLRender.setParamsInt(SAMPLE_TYPE, mSampleSelectedIndex + SAMPLE_TYPE, 0)
    }

    override fun onGlobalLayout() {
        mRootView.viewTreeObserver.removeOnGlobalLayoutListener(this)
        val lp = RelativeLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT)
        lp.addRule(RelativeLayout.CENTER_IN_PARENT)
        mGLSurfaceView = MyGLSurfaceView(this, mGLRender)
        mRootView.addView(mGLSurfaceView, lp)
        mGLSurfaceView.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_main, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        if (id == R.id.action_change_sample) {
            showGLSampleDialog()
        }
        return true
    }

    override fun onAccuracyChanged(p0: Sensor?, p1: Int) {
    }

    override fun onSensorChanged(p0: SensorEvent?) {

    }

    override fun onAudioBufferCallback(buffer: ShortArray?) {

    }

    private fun showGLSampleDialog() {
        val builder = AlertDialog.Builder(this)
        val inflater = LayoutInflater.from(this)
        val rootView = inflater.inflate(R.layout.sample_selected_layout, null)
        val dialog = builder.create()
        val confirmBtn = rootView.findViewById<Button>(R.id.confirm_btn)
        confirmBtn.setOnClickListener { dialog.cancel() }
        val resolutionsListView: RecyclerView = rootView.findViewById(R.id.resolution_list_view)
        val myPreviewSizeViewAdapter = MyRecyclerViewAdapter(this, SAMPLE_TITLES)
        myPreviewSizeViewAdapter.selectIndex = mSampleSelectedIndex

        myPreviewSizeViewAdapter.addOnItemClickListener(object :
            MyRecyclerViewAdapter.OnItemClickListener {
            override fun onItemClick(view: View?, position: Int) {
                mRootView.removeView(mGLSurfaceView)
                val lp = RelativeLayout.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT
                )
                lp.addRule(RelativeLayout.CENTER_IN_PARENT)
                mGLSurfaceView = MyGLSurfaceView(this@MainActivity, mGLRender)
                mRootView.addView(mGLSurfaceView, lp)
                val selectIndex: Int = myPreviewSizeViewAdapter.getSelectIndex()
                myPreviewSizeViewAdapter.setSelectIndex(position)
                myPreviewSizeViewAdapter.notifyItemChanged(selectIndex)
                myPreviewSizeViewAdapter.notifyItemChanged(position)
                mSampleSelectedIndex = position
                supportActionBar?.title = SAMPLE_TITLES[mSampleSelectedIndex]

                mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY)
                if (mRootView.getWidth() != mGLSurfaceView.getWidth()
                    || mRootView.getHeight() != mGLSurfaceView.getHeight()
                ) {
                    mGLSurfaceView.setAspectRatio(mRootView.getWidth(), mRootView.getHeight())
                }
                mGLRender.setParamsInt(SAMPLE_TYPE, position + SAMPLE_TYPE, 0)
                val sampleType: Int = position + SAMPLE_TYPE
                val tmp: Bitmap?
                when (sampleType) {
                    SAMPLE_TYPE_TRIANGLE -> {
                    }
                    SAMPLE_TYPE_TEXTURE_MAP -> loadRGBAImage(R.drawable.dzzz)
                    SAMPLE_TYPE_YUV_TEXTURE_MAP -> loadNV21Image()
                    SAMPLE_TYPE_VBO, SAMPLE_TYPE_VAO -> {
                    }
                    SAMPLE_TYPE_FBO, SAMPLE_TYPE_KEY_COPY_TEXTURE, SAMPLE_TYPE_KEY_BLIT_FRAME_BUFFER -> {
                        val bitmap = loadRGBAImage(R.drawable.lye)
                        mGLSurfaceView.setAspectRatio(bitmap!!.width, bitmap.height)
                    }
                    SAMPLE_TYPE_FBO_LEG -> loadRGBAImage(R.drawable.leg)
                    SAMPLE_TYPE_EGL -> startActivity(Intent(this@MainActivity, EGLActivity::class.java))
                    SAMPLE_TYPE_COORD_SYSTEM, SAMPLE_TYPE_BASIC_LIGHTING, SAMPLE_LEARN_OG_COLOR, SAMPLE_LEARN_PHONG_BASIC,
                    SAMPLE_LEARN_PHONG_MATERIALS, SAMPLE_LEARN_PHONG_TEXTURE, SAMPLE_LEARN_LIGHT_DIRECTIONAL,SAMPLE_POINT_LIGHT,
                    SAMPLE_SPOTLIGHT,SAMPLE_MULTILIGHTS,
                    SAMPLE_TYPE_TRANS_FEEDBACK, SAMPLE_TYPE_MULTI_LIGHTS,
                    SAMPLE_TYPE_DEPTH_TESTING, SAMPLE_TYPE_INSTANCING,
                    SAMPLE_TYPE_STENCIL_TESTING -> {
                        loadRGBAImage(R.drawable.texture_diffuse)
                        if (sampleType in (SAMPLE_LEARN_PHONG_TEXTURE..SAMPLE_MULTILIGHTS)) {
                            loadRGBAImage(R.drawable.texture_specular, 0)
                        }
                    }
                    SAMPLE_TYPE_3D_MODEL -> {
                        Log.e("zfang", "load 3d model")
                    }
                    SAMPLE_DEPTH_BUFFER_TEST, SAMPLE_STENCIL_BUFFER_TEST, SAMPLE_COLOR_BLEND_TEST -> {
                        loadRGBAImage(R.drawable.marble)
                        loadRGBAImage(R.drawable.metal, 0)
                        if (sampleType == SAMPLE_COLOR_BLEND_TEST) {
                            loadRGBAImage(R.drawable.blending_transparent_window, 1)
                        }
                    }
                    SAMPLE_FRAME_BUFFER -> {
                        loadRGBAImage(R.drawable.marble)
                        loadRGBAImage(R.drawable.metal, 0)
                    }
                    SAMPLE_TYPE_BLENDING -> {
                        loadRGBAImage(R.drawable.board_texture, 0)
                        loadRGBAImage(R.drawable.floor, 1)
                        loadRGBAImage(R.drawable.window, 2)
                    }
                    SAMPLE_FACE_CULLING -> {
                        loadRGBAImage(R.drawable.texture_diffuse)
                        Log.d("zfang", "Face culling")
                    }
                    SAMPLE_TYPE_PARTICLES -> {
                        loadRGBAImage(R.drawable.board_texture)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_SKYBOX -> {
                        loadRGBAImage(R.drawable.right, 0)
                        loadRGBAImage(R.drawable.left, 1)
                        loadRGBAImage(R.drawable.top, 2)
                        loadRGBAImage(R.drawable.bottom, 3)
                        loadRGBAImage(R.drawable.back, 4)
                        loadRGBAImage(R.drawable.front, 5)
                    }
                    SAMPLE_TYPE_PBO -> {
                        loadRGBAImage(R.drawable.front)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_BEATING_HEART -> mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    SAMPLE_TYPE_KEY_CLOUD -> {
                        loadRGBAImage(R.drawable.noise)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_TIME_TUNNEL -> {
                        loadRGBAImage(R.drawable.front)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_BEZIER_CURVE ->                         //loadRGBAImage(R.drawable.board_texture);
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    SAMPLE_TYPE_KEY_BIG_EYES, SAMPLE_TYPE_KEY_FACE_SLENDER -> {
                        val bitmap = loadRGBAImage(R.drawable.yifei)
                        mGLSurfaceView.setAspectRatio(bitmap!!.width, bitmap.height)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_BIG_HEAD, SAMPLE_TYPE_KEY_ROTARY_HEAD -> {
                        val b = loadRGBAImage(R.drawable.huge)
                        mGLSurfaceView.setAspectRatio(b!!.width, b.height)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_VISUALIZE_AUDIO -> {
                        if (mAudioCollector == null) {
                            mAudioCollector = AudioCollector()
                            mAudioCollector.addCallback(this@MainActivity)
                            mAudioCollector.init()
                        }
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                        val b1 = loadRGBAImage(R.drawable.yifei)
                        mGLSurfaceView.setAspectRatio(b1!!.width, b1.height)
                    }
                    SAMPLE_TYPE_KEY_SCRATCH_CARD -> {
                        val b1 = loadRGBAImage(R.drawable.yifei)
                        mGLSurfaceView.setAspectRatio(b1!!.width, b1.height)
                    }
                    SAMPLE_TYPE_KEY_AVATAR -> {
                        val b2 = loadRGBAImage(R.drawable.avatar_a, 0)
                        mGLSurfaceView.setAspectRatio(b2!!.width, b2.height)
                        loadRGBAImage(R.drawable.avatar_b, 1)
                        loadRGBAImage(R.drawable.avatar_c, 2)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_SHOCK_WAVE, SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER, SAMPLE_TYPE_KEY_TEXT_RENDER -> {
                        val b3 = loadRGBAImage(R.drawable.lye)
                        mGLSurfaceView.setAspectRatio(b3!!.width, b3.height)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_MRT, SAMPLE_TYPE_KEY_FBO_BLIT, SAMPLE_TYPE_KEY_TBO, SAMPLE_TYPE_KEY_UBO -> {
                        val b4 = loadRGBAImage(R.drawable.lye)
                        mGLSurfaceView.setAspectRatio(b4!!.width, b4.height)
                    }
                    SAMPLE_TYPE_KEY_RGB2YUYV, SAMPLE_TYPE_KEY_RGB2NV21, SAMPLE_TYPE_KEY_RGB2I420, SAMPLE_TYPE_KEY_RGB2I444 -> {
                        tmp = loadRGBAImage(R.drawable.sk)
                        mGLSurfaceView.setAspectRatio(tmp!!.width, tmp.height)
                    }
                    SAMPLE_TYPE_KEY_STAY_COLOR -> {
                        loadGrayImage()
                        val b5 = loadRGBAImage(R.drawable.lye2)
                        loadRGBAImage(R.drawable.ascii_mapping, 1)
                        mGLSurfaceView.setAspectRatio(b5!!.width, b5.height)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    SAMPLE_TYPE_KEY_TRANSITIONS_1, SAMPLE_TYPE_KEY_TRANSITIONS_2, SAMPLE_TYPE_KEY_TRANSITIONS_3, SAMPLE_TYPE_KEY_TRANSITIONS_4 -> {
                        loadRGBAImage(R.drawable.lye, 0)
                        loadRGBAImage(R.drawable.lye4, 1)
                        loadRGBAImage(R.drawable.lye5, 2)
                        loadRGBAImage(R.drawable.lye6, 3)
                        loadRGBAImage(R.drawable.lye7, 4)
                        tmp = loadRGBAImage(R.drawable.lye8, 5)
                        mGLSurfaceView.setAspectRatio(tmp!!.width, tmp.height)
                        mGLSurfaceView.setRenderMode(GLSurfaceView.RENDERMODE_CONTINUOUSLY)
                    }
                    else -> {
                    }
                }
                mGLSurfaceView.requestRender()
//                if (sampleType != SAMPLE_TYPE_KEY_VISUALIZE_AUDIO && mAudioCollector != null) {
//                    mAudioCollector.unInit()
//                    mAudioCollector = null
//                }
                dialog.cancel()
            }
        })
        val manager = LinearLayoutManager(this)
        manager.orientation = LinearLayoutManager.VERTICAL
        resolutionsListView.layoutManager = manager
        resolutionsListView.adapter = myPreviewSizeViewAdapter
        resolutionsListView.scrollToPosition(mSampleSelectedIndex)
        dialog.show()
        dialog.window!!.setContentView(rootView)
    }

    private fun loadRGBAImage(resId: Int): Bitmap? {
        val inputStream = this.resources.openRawResource(resId)
        val sourceBitmap: Bitmap?
        try {
            sourceBitmap = BitmapFactory.decodeStream(inputStream)?.also {
                val bytesCount = it.byteCount
                val dstBuffer = ByteBuffer.allocate(bytesCount)
                it.copyPixelsToBuffer(dstBuffer)
                val dstByteArray = dstBuffer.array()
                mGLRender.setImageData(IMAGE_FORMAT_RGBA, it.width, it.height, dstByteArray)
            }
        } finally {
            try {
                inputStream.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
        return sourceBitmap
    }

    private fun loadRGBAImage(resId: Int, index: Int): Bitmap? {
        val inputStream = this.resources.openRawResource(resId)
        val bitmap: Bitmap?
        try {
            bitmap = BitmapFactory.decodeStream(inputStream)
            if (bitmap != null) {
                val bytes = bitmap.byteCount
                val buf = ByteBuffer.allocate(bytes)
                bitmap.copyPixelsToBuffer(buf)
                val byteArray = buf.array()
                mGLRender.setImageDataWithIndex(
                    index,
                    IMAGE_FORMAT_RGBA,
                    bitmap.width,
                    bitmap.height,
                    byteArray
                )
            }
        } finally {
            try {
                inputStream.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
        return bitmap
    }

    private fun loadNV21Image() {
        var inputStream: InputStream? = null
        try {
            inputStream = assets.open("YUV_Image_840x1074.NV21")
        } catch (e: IOException) {
            e.printStackTrace()
        }
        var lenght = 0
        try {
            lenght = inputStream!!.available()
            val dstBuffer = ByteArray(lenght)
            inputStream.read(dstBuffer)
            mGLRender.setImageData(IMAGE_FORMAT_NV21, 840, 1074, dstBuffer)
        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            try {
                inputStream!!.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    private fun loadGrayImage() {
        var inputStream: InputStream? = null
        try {
            inputStream = assets.open("lye_1280x800.Gray")
        } catch (e: IOException) {
            e.printStackTrace()
        }
        var lenght = 0
        try {
            lenght = inputStream!!.available()
            val buffer = ByteArray(lenght)
            inputStream.read(buffer)
            mGLRender.setImageDataWithIndex(0, IMAGE_FORMAT_GARY, 1280, 800, buffer)
        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            try {
                inputStream!!.close()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }
}