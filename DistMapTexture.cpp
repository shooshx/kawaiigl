#include "DistMapTexture.h"
#include <QImage>
#include <stdio.h>


using namespace std;

template<typename T>
class FastImage {
public:
    FastImage(T *p, int width, int height) : m_data(p), m_width(width), m_height(height), m_owned(false) {}
    FastImage(int width, int height) : m_data(new T[width * height]), m_width(width), m_height(height), m_owned(true) {}
    FastImage() :m_data(NULL), m_height(0), m_width(0), m_owned(false) {}
    FastImage& operator=(FastImage<T>&& o) {
        if (&o != this) {
            m_data = o.m_data; m_width = o.m_width; m_height = o.m_height; m_owned = o.m_owned;
            o.m_owned = false;
        }
        return *this;
    }
    ~FastImage() {
        if (m_owned)
            delete[] m_data;
    }
    T get(int x, int y) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height)
            throw 'bla';
        return m_data[y * m_width + x];
    }
    void set(int x, int y, T d) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height)
            throw 'bla';
        m_data[y * m_width + x] = d;
    }
    void inc(int x, int y) {
        if (x < 0 || y < 0 || x >= m_width || y >= m_height)
            throw 'bla';
        ++m_data[y * m_width + x];
    }
    int width() { return m_width; }
    int height() { return m_height; }
    T* ptr() { return m_data; }

private:
    T *m_data;
    int m_width, m_height;
    bool m_owned;
};


int upDiv(int a, int b) {
    return (a + b - 1) / b;
}
template<typename T>
T mmin(T a, T b) {
    return (a < b)?a:b;
}
template<typename T>
T mmax(T a, T b) {
    return (a > b)?a:b;
}
double round(double r) {
    return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

uint RGBA(int v) {
    uint x = *(uint*)&v;
    return ((x & 0xff) | ((x & 0xff)<<8) | ((x & 0xff)<<16) | (255<<24));
}


void DistMapTexture::convert(const QString& filename, int smallSz, float halo)
{
    QImage inf(filename);
    //auto ff = inf.format(); // ARGB32
    FastImage<uint> in((uint*)inf.bits(), inf.width(), inf.height());
    FastImage<uint> out;
    if (in.width() < in.height())
        out = FastImage<uint>(smallSz, ceil(smallSz * (float)in.height()/ in.width()));
    else
        out = FastImage<uint>(ceil(smallSz * (float)in.width()/ in.height()), smallSz);

    int mxInDim = mmin(in.width(), in.height());
    float d = mmin(in.width()/out.width(), in.height()/out.height());
    float hd = d / 2;

    int width = in.width(), height = in.height();

//    int y = out.height()/2;
//    int x = out.width()/2;


    for(int y = 0; y < out.height(); ++y) 
    {
        printf("%d\n", y);
        for(int x = 0; x < out.width(); ++x) 
        {
            int ix = x * d + hd;
            int iy = y * d + hd;
            int psamp = in.get(ix, iy) & 0x00ffffff;

            double foundDist = 0;
            if (psamp == 0 || psamp == 0xa0a0a0 || psamp == 0xffffff) 
            {
                // anything not these two colors is dist 0
                foundDist = mxInDim*mxInDim;
                int maxR = mxInDim;
                int fx, fy;

#define CHECK(chx, chy) { \
                if ((in.get(chx, chy) & 0x00ffffff) != psamp) { \
                    double dist = (chx-ix)*(chx-ix) + (chy-iy)*(chy-iy); \
                    if (dist < foundDist) { \
                        fx = chx; \
                        fy = chy; \
                        foundDist = dist; \
                    } \
                    if (maxR == mxInDim) \
                        maxR = mmin((int)ceil((double)r * 1.42), maxR); \
                } \
            };

                for (int r = 1; r < maxR; ++r) 
                {
                    int cx, cy;
                    int x1 = mmax(0, ix-r), x2 = mmin(ix+r, width-1);
                    cy = iy - r;
                    if (cy >= 0) {
                        for(cx = x1; cx <= x2; ++cx)
                            CHECK(cx, cy);
                    }
                    cy = iy + r;
                    if (cy < height) {
                        for(cx = x1; cx <= x2; ++cx)
                            CHECK(cx, cy);
                    }
                    int y1 = mmax(0, iy-r), y2 = mmin(iy+r, height-1);
                    cx = ix - r;
                    if (cx >= 0) {
                        for(cy = y1; cy < y2; ++cy)
                            CHECK(cx, cy);
                    }
                    cx = ix + r;
                    if (cx < width) {
                        for(cy = y1; cy < y2; ++cy)
                            CHECK(cx, cy);
                    }

                }
                foundDist = sqrt(foundDist);
            }

            int wdist = round(foundDist);
            if (psamp != 0xffffff)
                wdist = -wdist;
            wdist = mmin(127, mmax(-128, wdist));
            wdist += 128;

            out.set(x, y, RGBA( wdist ));

        }
    }

    QImage outf((uchar*)out.ptr(), out.width(), out.height(), QImage::Format_ARGB32);

    if (!outf.save(filename + "_out.png", "PNG")) {
        printf("Failed writing file");
    }
}