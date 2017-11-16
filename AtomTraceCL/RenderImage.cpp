#include <cstdlib>
#include <iostream>

#include "lodepng.h"

#include "RenderImage.h"


RenderImage::RenderImage() : 
    m_pData(NULL),
    m_width(0),
    m_height(0)
{
}

RenderImage::RenderImage(unsigned int w, unsigned int h) :
    m_pData(NULL),
    m_width(w),
    m_height(h)
{
    m_pData = new unsigned char[w*h * 3]();
    memset(m_pData, 0, w*h * 3);
}


RenderImage::~RenderImage()
{
    this->Release();
}

bool RenderImage::Init(unsigned int w, unsigned int h)
{
    if (0 == w || 0 == h)
    {
        return false;
    }

    if (nullptr != m_pData)
    {
        delete[] m_pData;
    }

    m_width = w;
    m_height = h;
    m_pData = new unsigned char[w * h * 3];
    memset(m_pData, 0, w * h * 3);
    return true;
}

bool RenderImage::Release()
{
    if (nullptr != m_pData)
    {
        delete[] m_pData;
        m_pData = nullptr;
        m_width = 0;
        m_height = 0;
    }

    return true;
}

bool RenderImage::SetChannelAt(unsigned int pixel, unsigned int channelIndex, unsigned char value)
{
    using namespace std;
    if (NULL == m_pData)
    {
        cerr << "Image haven't been allocated yet!!.\n";
        return false;
    }

    m_pData[pixel * 3 + channelIndex] = value;

    return true;
}

const unsigned char* RenderImage::GetRawData() const
{
    return m_pData;
}

unsigned char* RenderImage::GetRawData()
{
    return m_pData;
}

unsigned int RenderImage::Width() const
{
    return m_width;
}

unsigned int RenderImage::Height() const
{
    return m_height;
}

bool RenderImage::SavePPM(const char *filename) const
{
    FILE *fp = NULL;
    fopen_s(&fp, filename, "wb");
    if (!fp) return false;
    fprintf(fp, "P6\n%d %d\n255\n", m_width, m_height);

    fwrite(m_pData, 3, m_width * m_height, fp);
        
    fclose(fp);
    return true;
}

bool RenderImage::SavePNG(const char* filename) const
{
    if (nullptr == m_pData || 0 == m_width || 0 == m_height)
    {
        return false;
    }

    unsigned int err = lodepng::encode(filename, m_pData, m_width, m_height, LCT_RGB, 8);
    return (0 == err);
}
