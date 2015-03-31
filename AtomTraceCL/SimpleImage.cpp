#include <cstdlib>
#include <iostream>

#include "SimpleImage.h"


SimpleImage::SimpleImage() : 
    m_pData(NULL),
    m_width(0),
    m_height(0)
{
}

SimpleImage::SimpleImage(unsigned int w, unsigned int h) :
    m_pData(NULL),
    m_width(w),
    m_height(h)
{
    m_pData = new unsigned char[w*h * 3]();
}


SimpleImage::~SimpleImage()
{
    if (NULL != m_pData)
    {
        delete m_pData;
        m_pData = NULL;
    }
}

bool SimpleImage::SetChannelAt(unsigned int pixel, unsigned int channelIndex, unsigned char value)
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

const unsigned char* SimpleImage::GetRawData() const
{
    return m_pData;
}

unsigned int SimpleImage::Width() const
{
    return m_width;
}

unsigned int SimpleImage::Height() const
{
    return m_height;
}

bool SimpleImage::SavePPM(const char *filename) const
{
    FILE *fp = NULL;
    fopen_s(&fp, filename, "wb");
    if (!fp) return false;
    fprintf(fp, "P6\n%d %d\n255\n", m_width, m_height);

    fwrite(m_pData, 3, m_width * m_height, fp);
        
    fclose(fp);
    return true;
}