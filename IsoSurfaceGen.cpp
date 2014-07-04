#include "Document.h"
#include "KwParser.h"
#include "ParserModel.h"
#include "MyLib/Mesh.h"
#include "ShapeIFS.h"

#include <iostream>

using namespace std;

struct Evaler3D {
    Evaler3D(Model* _model, Evalable<float>* _f) : model(_model), f(_f), xv(0), yv(0), zv(0)
    {
        model->m_numsym.set("x", &xv);
        model->m_numsym.set("y", &yv);
        model->m_numsym.set("z", &zv);
    }
    ~Evaler3D() {
        model->m_numsym.set("x", NULL);
        model->m_numsym.set("y", NULL);
        model->m_numsym.set("z", NULL);
        // using x,y,z after this will give 0
    }

    float eval(float x, float y, float z) {
        xv.val = x;
        yv.val = y;
        zv.val = z;
        return f->eval();
    }
    Model* model;
    Evalable<float>* f;

    FloatVal<float> xv, yv, zv;
};

float toFloat(const string& s) {
    return QString(s.c_str()).toFloat();
}

void sortMinMax(float& a, float& b) {
    if (b > a)
        return;
    float c = a;
    a = b;
    b = c;
}

template<typename T>
struct Mat {
    Mat(int width, int height) :m_width(width), m_height(height) {
        v = new T[width * height];
    }
    ~Mat() {
        delete[] v;
    }
    T get(int x, int y) {
        return v[y * m_width + x];
    }
    const T& getr(int x, int y) {
        return v[y * m_width + x];
    }
    void set(int x, int y, T val) {
        v[y * m_width + x] = val;
    }
    void setr(int x, int y, const T& val) {
        v[y * m_width + x] = val;
    }
    void swap(Mat<T>& o) {
        std::swap(v, o.v);
        // assume the dimentions are the same
    }

private:
    Mat(const Mat&); // disallow copy
    Mat& operator=(const Mat&);

private:
    T *v;
    int m_width, m_height;
};


struct VoxState
{
    VoxState()
        :isect(8,8)
    {}
    void sortLimits() {
        sortMinMax(x0, x1);
        sortMinMax(y0, y1);
        sortMinMax(z0, z1);
        xn = (x1-x0)/voxsz; yn = (y1-y0)/voxsz; zn = (z1-z0)/voxsz;
    }

    Vec3 getCorner(int i);

    float voxsz;  // voxel size
    float x0, x1, y0, y1, z0, z1; // sample box limits
    int xn, yn, zn; // number of samples

    float zp, yp, xp; // coordinates of the previous iteration in each dim (low extreme of the current cube)
    float zv, yv, xv; // coordinates of the current iteration in each dim. (high extreme of the current cube)

    float fval[8];
    bool fneg[8];

    MyObject *obj;
    Vec3 color;

    Mat<Vec3> isect;
};

Vec3 VoxState::getCorner(int i) {
    switch(i) {
    case 0: return Vec3(xp,yp,zv);
    case 1: return Vec3(xv,yp,zv);
    case 2: return Vec3(xv,yp,zp);
    case 3: return Vec3(xp,yp,zp);
    case 4: return Vec3(xp,yv,zv);
    case 5: return Vec3(xv,yv,zv);
    case 6: return Vec3(xv,yv,zp);
    case 7: return Vec3(xp,yv,zp);
    }
    throw std::exception("unexpected corner");
}


void samplePlane(float zv, VoxState& v, Evaler3D& ef, Mat<float>& mat)
{
    for(int yi = 0; yi <= v.yn; ++yi) {
        float yv = v.y0 + v.voxsz * yi;
        for(int xi = 0; xi <= v.xn; ++xi) {
            float xv = v.x0 + v.voxsz * xi;
            float fv = ef.eval(xv, yv, zv);
            mat.set(xi, yi, fv);
        }
    }
}

void runCube(VoxState& v) {
    bool b = v.fneg[0];
    if (v.fneg[1]==b && v.fneg[2]==b && v.fneg[3]==b && v.fneg[4]==b && v.fneg[5]==b && v.fneg[6]==b && v.fneg[7]==b)
        return;


    Vec3 q[3] = { Vec3(v.xp, v.yp, v.zp), 
                  Vec3(v.xv, v.yp, v.zp), 
                  Vec3(v.xv, v.yv, v.zv) };
    v.obj->AddPoly(q, v.color, NULL, NULL, 3);
}

void addTri(VoxState& v, int from, int t1, int t2, int t3) 
{
    Vec3 q[3] = { v.isect.get(from, t1), 
                  v.isect.get(from, t2), 
                  v.isect.get(from, t3) };
    v.obj->AddPoly(q, v.color, NULL, NULL, 3);
}
void addTri(VoxState& v, int f1, int t1, int f2, int t2, int f3, int t3) 
{
    Vec3 q[3] = { v.isect.get(f1, t1), 
                  v.isect.get(f2, t2), 
                  v.isect.get(f3, t3) };
    v.obj->AddPoly(q, v.color, NULL, NULL, 3);
}



void runTetra(VoxState& v, int a, int b, int c, int d)
{
    bool na = v.fneg[a], nb = v.fneg[b], nc = v.fneg[c], nd = v.fneg[d];
    if (nb==na && nc==na && nd==na)
        return;

   /* Vec3 p = v.getCorner(a) + v.getCorner(b) + v.getCorner(c) + v.getCorner(d);
    p /= 4;
    Vec3 q[3] = {p,p+Vec3(0.05,0,0),p+Vec3(0.05,0.05,0)};
    v.obj->AddPoly(q, v.color, NULL, NULL, 3);


    return;*/

    bool d_ab = na!=nb, d_ba=d_ab;
    bool d_ac = na!=nc, d_ca=d_ac;
    bool d_ad = na!=nd, d_da=d_ad;
    bool d_bc = nb!=nc, d_cb=d_bc;
    bool d_bd = nb!=nd, d_db=d_bd;
    bool d_cd = nc!=nd, d_dc=d_cd;

    if (d_ab && d_ac && d_ad) {// a is different
        if (na)
            addTri(v, a, b, d, c);
        else
            addTri(v, a, c, d, b);
    }
    else if (d_ba && d_bc && d_bd) { // b is different
        if (nb)
            addTri(v, b, c, d, a); 
        else
            addTri(v, b, a, d, c); 
    }
    else if (d_ca && d_cb && d_cd) { // c is different
        if (nc)
            addTri(v, c, a, d, b); 
        else
            addTri(v, c, b, d, a); 
    }
    else if (d_da && d_db && d_cd) { // d is different
        if (nd)
            addTri(v, d, b, c, a); 
        else
            addTri(v, d, a, c, b); 
    }
    else if (d_ab && d_bc && d_ad && d_cd) { // bd is cut
        if (nb) {
            addTri(v, a, b, b, c, a, d); //ab bc ad
            addTri(v, b, c, c, d, a, d); //bc cd ad
        }
        else {
            addTri(v, a, b, a, d, b, c); //ab ad bc
            addTri(v, b, c, a, d, c, d); //bc ad cd 
        }
    }
    else if (d_ad && d_ac && d_bc && d_bd) { //ab is cut
        if (nb) {
            addTri(v, a, c, b, c, b, d); //ac bc bd
            addTri(v, a, c, b, d, a, d); //ac bd ad
        }
        else {
            addTri(v, a, c, b, d, b, c); //ac bd bc
            addTri(v, a, c, a, d, b, d); //ac ad bd
        }
    }
    else if (d_ab && d_ac && d_bd && d_cd) { //bc is cut
        if (nb) {
            addTri(v, a, b, a, c, c, d); // ab ac cd
            addTri(v, a, b, c, d, b, d); // ab cd bd
        }
        else {
            addTri(v, a, b, c, d, a, c); // ab cd ac
            addTri(v, a, b, b, d, c, d); // ab bd cd
        }
    }

}

// pairs of cube corners that might have a root we want to add
struct { int first, second; } g_doIsects[] = 
{ {0,1}, {1,5}, {4,5}, {1,4},
  {1,2}, {2,6}, {5,6}, {1,6},
  {2,3}, {3,7}, {6,7}, {3,6},
  {3,0}, {0,4}, {7,4}, {3,4},
  {4,6}, {3,1} 
};

// isosurface(func, voxSz, x0,x1, y0,y1, z0,z1)
void Document::generateIsoSurface(const vector<string>& args)
{
    if (args.size() != 8) {
        cout << "isosurface reuquires 8 arguments" << endl;
        return;
    }


    // prepare function
    string name = "isofunc666";

    string fs = name + "=" + args[0];
    const char* iter = fs.data();
    const char* end = fs.data() + fs.size();
    const char* nameend = fs.data() + name.size();

    m_kparser.defineFloatSym("x");
    m_kparser.defineFloatSym("y");
    m_kparser.defineFloatSym("z");

    Evalable<float>* f = m_kparser.kparseFloat(iter, end, nameend);
    if (f == NULL)
        return;
    Evaler3D ef(m_kparser.model(), f);

    float p = ef.eval(1,1,1);

    // voxel sample
    VoxState v;
    v.obj = m_frameObj;
    v.voxsz = toFloat(args[1]);
    v.x0 = toFloat(args[2]); v.x1 = toFloat(args[3]);
    v.y0 = toFloat(args[4]); v.y1 = toFloat(args[5]);
    v.z0 = toFloat(args[6]); v.z1 = toFloat(args[7]);
    v.sortLimits();
    v.color = Vec3(m_conf.materialCol);

    Mat<float> m0(v.xn+1, v.yn+1), m1(v.xn+1, v.yn+1);
    samplePlane(v.z0, v, ef, m0);

    v.zp = v.z0;
    for(int zi = 1; zi <= v.zn; ++zi) {
        v.zv = v.z0 + v.voxsz * zi;
        samplePlane(v.zv, v, ef, m1);
        v.yp = v.y0;
        for(int yi = 1; yi <= v.yn; ++yi) 
        {
            v.yv = v.y0 + v.voxsz * yi;
            v.xp = v.x0;
            for(int xi = 1; xi <= v.xn; ++xi) 
            {
                v.xv = v.x0 + v.voxsz * xi;
                
                v.fval[0] = m1.get(xi-1, yi - 1); // xp,yp,zv
                v.fval[1] = m1.get(xi, yi - 1);   // xv,yp,zv
                v.fval[2] = m0.get(xi, yi - 1);   // xv,yp,zp
                v.fval[3] = m0.get(xi-1, yi - 1); // xp,yp,zp
                v.fval[4] = m1.get(xi-1, yi);     // xp,yv,zv
                v.fval[5] = m1.get(xi, yi);       // xv,yv,zv
                v.fval[6] = m0.get(xi, yi);       // xv,yv,zp
                v.fval[7] = m0.get(xi-1, yi);     // xp,yv,zp
                for(int i = 0; i < 8; ++i)
                    v.fneg[i] = v.fval[i] < 0;

                bool b = v.fneg[0];
                if (v.fneg[1]!=b || v.fneg[2]!=b || v.fneg[3]!=b || v.fneg[4]!=b || v.fneg[5]!=b || v.fneg[6]!=b || v.fneg[7]!=b)
                {
                    for(int ii = 0; ii < _countof(g_doIsects); ++ii) {
                        int i = g_doIsects[ii].first, j = g_doIsects[ii].second;
                        float fi = v.fval[i];
                        float fj = v.fval[j];
                        float t = abs(fi - fj);
                        fi = abs(fi)/t;
                        fj = abs(fj)/t;
                        Vec3 ci = v.getCorner(i);
                        Vec3 cj = v.getCorner(j);
                        Vec3 vis = (fj * ci + fi * cj);
                        v.isect.setr(i,j, vis);
                        v.isect.setr(j,i, vis);
                    }

                    if (true) {
                        runTetra(v, 0, 1, 4, 3);
                        runTetra(v, 2, 3, 6, 1);
                        runTetra(v, 5, 4, 1, 6);
                        runTetra(v, 7, 6, 3, 4);

                        runTetra(v, 4, 6, 3, 1);
                    }
                    else {
                        runCube(v);
                    }
                }
                v.xp = v.xv;
            }
            v.yp = v.yv;
        }

        v.zp = v.zv;
        m0.swap(m1);
    }


}
