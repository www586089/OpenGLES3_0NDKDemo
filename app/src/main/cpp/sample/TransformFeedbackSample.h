//
// Created by Thinkpad on 2022/8/23.
//

#ifndef OPENGLES3_0NDKDEMO_TRANSFORMFEEDBACKSAMPLE_H
#define OPENGLES3_0NDKDEMO_TRANSFORMFEEDBACKSAMPLE_H


#include "GLSampleBase.h"
#include "../utils/ImageDef.h"

class TransformFeedbackSample : public GLSampleBase
{
public:
    TransformFeedbackSample();

    virtual ~TransformFeedbackSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);

    virtual void destroy();

private:
    GLuint m_ImageTextureId;
    GLuint m_VaoId;
    GLuint m_VboIds[2] = {GL_NONE};;
    GLint m_SamplerLoc;
    GLuint m_TransFeedbackObjId;
    GLuint m_TransFeedbackBufId;
    NativeImage m_RenderImage;

};


#endif //OPENGLES3_0NDKDEMO_TRANSFORMFEEDBACKSAMPLE_H
