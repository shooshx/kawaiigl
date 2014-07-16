
#include "math.h"
#include "MyPolygon.h"

bool MyPolygon::calcNorm() //use the regular point and not the temp one
{
    // assume all points are on the same plane;
    bool normAgain = true;
    int pntA = 0, pntB = 1, pntC = 2;
    double nx, ny, nz;
    double xa, ya, za, xb, yb, zb, xc, yc, zc;

    while (normAgain)
    {	
        xa = vtx[pntA]->p[0], ya = vtx[pntA]->p[1], za = vtx[pntA]->p[2];
        xb = vtx[pntB]->p[0], yb = vtx[pntB]->p[1], zb = vtx[pntB]->p[2];
        xc = vtx[pntC]->p[0], yc = vtx[pntC]->p[1], zc = vtx[pntC]->p[2];
        
        nx = (ya-yb)*(za-zc)-(ya-yc)*(za-zb);
        ny = -((xa-xb)*(za-zc)-(xa-xc)*(za-zb));
        nz = (xa-xb)*(ya-yc)-(xa-xc)*(ya-yb);

        
        normAgain = ((fabs(nx) < 0.00000001) && (fabs(ny) < 0.00000001) && (fabs(nz) <0.00000001));
        if (normAgain)
        {
            if (pntC < pnum - 1)
            {
                pntC++;
            }
            else
            {
                if (pntB < pnum - 2)
                {
                    pntB++;
                    pntC = pntB + 1;
                }
                else
                {
                    if (pnum == 3)
                        return false;
                    if (pntA < pnum - 3)
                    {
                        pntA++;
                        pntB = pntA + 1;
                        pntC = pntA + 2;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        Q_ASSERT((pntB < pnum) && (pntC < pnum));
        
    }
    double lng = sqrt(nx*nx + ny*ny + nz*nz);


    center[0] = nx / lng;
    center[1] = ny / lng;
    center[2] = nz / lng;
    
    return true;

}

float MyPolygon::triangleArea()
{
    const Vec3& v1 = vtx[0]->p;
    const Vec3& v2 = vtx[1]->p;
    const Vec3& v3 = vtx[2]->p;

    float a = v1.to(v2).length();
    float b = v2.to(v3).length();
    float c = v3.to(v1).length();

    float p = (a+b+c)/2;
    float s = p*(p-a)*(p-b)*(p-c);
    s = sqrt(s);
    return s;
}

// not the general case. just the simple case of a square
bool MyPolygon::calcTangents()
{
    const Vec3& v1 = vtx[0]->p;
    const Vec3& v2 = vtx[1]->p;
    const Vec3& v3 = vtx[2]->p;

    const Vec2& w1 = texAncs[0];
    const Vec2& w2 = texAncs[1];
    const Vec2& w3 = texAncs[2];

    float x1 = v2.x - v1.x;
    float x2 = v3.x - v1.x;
    float y1 = v2.y - v1.y;
    float y2 = v3.y - v1.y;
    float z1 = v2.z - v1.z;
    float z2 = v3.z - v1.z;

    float s1 = w2.x - w1.x;
    float s2 = w3.x - w1.x;
    float t1 = w2.y - w1.y;
    float t2 = w3.y - w1.y;

    float r = 1.0F / (s1 * t2 - s2 * t1);
    Vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
        (t2 * z1 - t1 * z2) * r);
    Vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
        (s1 * z2 - s2 * z1) * r);

    tangent = sdir;

    bitangent = tdir;
    return true;
}



#if 0
{
    Vec3 base, tos, tot;
    Vec2 baseTex(100.0, 100.0);
    for(int i = 0; i < 4; ++i)
    {
        if (texAncs[i].x < baseTex.x || texAncs[i].y < baseTex.y)
        {
            base = vtx[i]->p;
            baseTex = texAncs[i];
        }
    }
    for(int i = 0; i < 4; ++i)
    {
        if (texAncs[i].x == baseTex.x && texAncs[i].y != baseTex.y)
            tos = vtx[i]->p;
        else if (texAncs[i].x != baseTex.x && texAncs[i].y == baseTex.y)
            tot = vtx[i]->p;
    }
    tangent = tos - base;
    bitangent = tot - base;
    return true;
}
#endif