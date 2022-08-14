//
// Created by Thinkpad on 2022/8/12.
//

#ifndef OPENGLES3_0NDKDEMO_VAOSAMPLE_H
#define OPENGLES3_0NDKDEMO_VAOSAMPLE_H

#include "../base//GLSampleBase.h"

class VaoSample : public GLSampleBase
{
public:
    VaoSample();

    virtual ~VaoSample();

    virtual void loadImage(NativeImage *pImage);

    virtual void init();
    virtual void draw(int screenW, int screenH);
    virtual void destroy();

private:
    GLuint  m_VaoId;
    GLuint  m_VboIds[2];
};


#endif //OPENGLES3_0NDKDEMO_VAOSAMPLE_H
