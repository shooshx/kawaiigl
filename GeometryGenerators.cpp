#define __glu_h__

#include "Document.h"
#include "MyPolygon.h"
#include "ShapeIFS.h"
#include "Renderable.h"

#undef __glu_h__
#include "glu13/glu.h"


class TorusVtxGen
{
public: 
	bool init(const vector<string>& sep, int* _numt, int* _numc) 
	{
		if (sep.size() < 4)
			return false;
	
		rt = QString(sep[0].c_str()).toFloat();
		rc = QString(sep[1].c_str()).toFloat();
		*_numt = numt = QString(sep[2].c_str()).toInt();
		*_numc = numc = QString(sep[3].c_str()).toInt();
		return true;
	}

	void vtx(float s, float t, Vec3* p, Vec3* n) {
		float x,y,z;
		x = (rt + rc * cos(s*TWOPI/numc)) * cos(t*TWOPI/numt);
		z = (rt + rc * cos(s*TWOPI/numc)) * sin(t*TWOPI/numt);
		y = rc * sin(s*TWOPI/numc);
		*p = Vec3(x, y, z);
	//	x = cos(t*TWOPI/numt) * cos(s*TWOPI/numc);
	//	z = sin(t*TWOPI/numt) * cos(s*TWOPI/numc);
	//	y = sin(s*TWOPI/numc);
	//	*n = Vec3(x, y, z);
	}
private:
	float rt, rc;
	int numc, numt;
};


template<typename Gen>
static void generateRotate(Gen& gen, const vector<string>& sep, MyObject& obj, const Vec3& materialCol) {
	int numc = 0, numt = 0;	
	if (!gen.init(sep, &numt, &numc))
		return;
	if (numc == 0 || numt == 0)
		return;

	float s, t;

	//setClipRadius( rt+rc );
	static TexAnchor dummyAncs[4];

	for(int i=0; i<numc; i++ )
	{
		Vec3 q[4];
		//QuadStrip *qs = new QuadStrip();
		for(int j=numt; j>=0; j-- )
		{
			q[0] = q[3];
			q[1] = q[2];
			for(int k=1; k>=0; k-- )
			{
				s = (i + k) % numc + 0.5;
				t = j % numt;

				Vec3 p, n;
				gen.vtx(s, t, &p, &n);

				q[k+2] = p;
				//v->setNormal( Vector( x, y, z ) );
				//qs->insert( v );
			}
			if (!q[0].isZero()) {

				obj.AddPoly(q, materialCol);
			}
		}
		//this->insert( qs );
	}
}

void Document::generateTorus(const vector<string>& sep)
{
	TorusVtxGen gen;
	MyObject& obj = *m_frameObj;
	generateRotate(gen, sep, obj, Vec3(m_conf.materialCol));
}


class SphereVtxGen
{
public: 
	bool init(const vector<string>& sep, int* _numt, int* _numc) 
	{
		if (sep.size() < 3)
			return false;
	
		r = QString(sep[0].c_str()).toFloat();
		*_numt = numt = QString(sep[1].c_str()).toInt();
		*_numc = numc = QString(sep[2].c_str()).toInt();
		return true;
	}

	void vtx(float s, float t, Vec3* p, Vec3* n) {
		float x,y,z;
		x = (0 + r * cos(s*TWOPI/numc)) * cos(t*TWOPI/numt);
		z = (0 + r * cos(s*TWOPI/numc)) * sin(t*TWOPI/numt);
		y = r * sin(s*TWOPI/numc);
		*p = Vec3(x, y, z);

	}
private:
	float r;
	int numc, numt;
};


void Document::generateQuadSphere(const vector<string>& sep)
{
	// This doesn't work because generateRotate is specific for thr generation of the torus.
//	SphereVtxGen gen;
//	MyObject& obj = *m_frameObj;
//	generateRotate(gen, sep, obj, Vec3(m_conf.materialCol));
}


void WINAPI vertexData( GLfloat *vertex, void *userData ) {
	vector<Vec3>* pnts = static_cast<vector<Vec3>*>(userData);
	Vec3 p(vertex, 3);
	pnts->push_back(p);
}


class NurbsCurve : public Renderable
{
public:
	NurbsCurve(int nSamples)
		:Renderable()
	{
		m_nurb = gluNewNurbsRenderer();
		// specify manually how many samples we want
		gluNurbsProperty(m_nurb, GLU_SAMPLING_METHOD, GLU_DOMAIN_DISTANCE); 
		gluNurbsProperty(m_nurb, GLU_U_STEP, nSamples);
	}
	virtual ~NurbsCurve() {
		if (m_nurb != 0)
			gluDeleteNurbsRenderer(m_nurb);
	}
	virtual void render(GLWidget *m_glw) {
		glDisable(GL_LIGHTING);
		gluBeginCurve(m_nurb);
		gluNurbsCurve(m_nurb, m_knots.size(), &m_knots[0], 3, (float*)&m_pnts[0], m_pnts.size(), GL_MAP1_VERTEX_3);
		gluEndCurve(m_nurb);
		glEnable(GL_LIGHTING);
	}

	void addPoint(const Vec3 p) {
		m_pnts.push_back(p);
		m_knots.push_back(0.0f);
	}
	void finalize() {
		for(size_t i = 0; i < m_pnts.size(); ++i) {
			m_knots.push_back(1.0f);
		}
	}
	Vec3 first() { return m_pnts[0]; }
	Vec3 last() { return m_pnts[m_pnts.size()-1]; }
	
	void sample(vector<Vec3>* pnts) {
		gluNurbsProperty(m_nurb, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
		gluNurbsCallbackData(m_nurb, pnts);
		gluNurbsCallback(m_nurb, GLU_NURBS_VERTEX_DATA, (_GLUfuncptr)vertexData);
		gluBeginCurve(m_nurb);
		gluNurbsCurve(m_nurb, m_knots.size(), &m_knots[0], 3, (float*)&m_pnts[0], m_pnts.size(), GL_MAP1_VERTEX_3);
		gluEndCurve(m_nurb);
		gluNurbsProperty(m_nurb, GLU_NURBS_MODE, GLU_NURBS_RENDERER);
	}

private:
	GLUnurbsObj *m_nurb;
	vector<Vec3> m_pnts;
	vector<float> m_knots;

};




void Document::generateCurve(const vector<string>& args)
{	
	if (args.size() < 3) // need atleast 2 points
		return;

	int nSamp = QString(args[0].c_str()).toInt();
	NurbsCurve *curve = new NurbsCurve(nSamp);

	for(size_t i = 1; i < args.size(); ++i) {
		IPoint *p = m_kparser.creator()->lookupSymbol(args[i]);
		if (p != NULL)
			curve->addPoint(p->getCoord());
	}
	curve->finalize();

	m_rends.push_back(shared_ptr<NurbsCurve>(curve));

}

static Vec3 rotateAround(const Vec3 &p, const Vec3& lp, const Vec3& vc, float theta) {
	float cost = cos(theta), sint = sin(theta);
	float x = p.x, y = p.y, z = p.z;
	float a = lp.x, b = lp.y, c = lp.z;
	float u = vc.x, v = vc.y, w = vc.z;
	Vec3 r;
	r.x = (a*(v*v+w*w)-u*(b*v+c*w-u*x-v*y-w*z))*(1-cost)+x*cost+(-c*v+b*w-w*y+v*z)*sint;
	r.y = (b*(u*u+w*w)-v*(a*u+c*w-u*x-v*y-w*z))*(1-cost)+y*cost+(c*u-a*w+w*x-u*z)*sint;
	r.z = (c*(u*u+v*v)-w*(a*u+b*v-u*x-v*y-w*z))*(1-cost)+z*cost+(-b*u+a*v-v*x+u*y)*sint;
	return r;
}


void Document::generateRotObj(const std::vector<std::string>& args)
{
	if (args.size() < 4)
		return;
	int curvSamp = QString(args[0].c_str()).toInt();
	NurbsCurve curve(curvSamp);
	int circSamp = QString(args[1].c_str()).toInt();

	for(size_t i = 2; i < args.size(); ++i) {
		IPoint *p = m_kparser.creator()->lookupSymbol(args[i]);
		if (p != NULL)
			curve.addPoint(p->getCoord());
	}
	curve.finalize();

	Vec3 axisVec = curve.first()-curve.last();
	axisVec.unitize();

	MyObject& obj = *m_frameObj;

	vector<Vec3> samp;
	curve.sample(&samp);
	int sampsz = samp.size();
	
	vector<Vec3> last = samp, cur(samp.size());
	for(int angi = 0; angi < circSamp+1; ++angi) {
		if (angi < circSamp) {
			float theta = (TWOPI/circSamp)*angi;
			for(size_t j = 0; j < samp.size(); ++j) {
				cur[j] = rotateAround(samp[j], curve.first(), axisVec, theta);
			}
		}
		else {
			cur = samp;
		}

		for(size_t j = 0; j < samp.size(); ++j) {
			if (j == 0 || angi == 0)
				continue;
			if (j == 1) {
				// first and last are triangles
				Vec3 q[4] = { samp[0], samp[0], last[j], cur[j] };
				obj.AddPoly(q, Vec3(m_conf.materialCol), NULL, NULL, 4);
			}
			else if (j == sampsz - 1) {
				Vec3 q[4] = { last[j-1], samp[sampsz-1], samp[sampsz-1], cur[j-1] };
				obj.AddPoly(q, Vec3(m_conf.materialCol), NULL, NULL, 4);
			}
			else {
				Vec3 q[4] = { last[j-1], last[j], cur[j], cur[j-1] };
				obj.AddPoly(q, Vec3(m_conf.materialCol));
			}
		}
		last = cur;
	}

/*	for(size_t j = 1; j < samp.size(); ++j) {

		Vec3 q[4] = { last[j-1], last[j], samp[j], samp[j-1] };
		obj.AddPoly(q, Vec3(m_conf.materialCol));
	}*/

}


//*******************************************************************************************************************************************


#pragma warning (disable: 4305) // truncation from 'double' to 'GLfloat'

// coordinates of one of the icosahedron vertex
#define X 0.525731112119133696
#define Z 0.850650808352039932

// icosahedron  vertices
static GLfloat icosahedronvertex[12][3] = {
	{-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
	{0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X},
	{Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

// icosehedron faces
static int icosahedrontriangle[20][3] = {
	{1,4,0}, {4,9,0}, {4,5,9}, {8,5,4}, {1,8,4},
	{1,10,8}, {10,3,8}, {8,3,5}, {3,2,5}, {3,7,2},
	{3,10,7}, {10,6,7}, {6,11,7}, {6,0,11}, {6,1,0},
	{10,1,6}, {11,0,9}, {2,11,9}, {5,2,9}, {11,2,7}
};


// Vertices should belong to the unit sphere.
// so we perform normalization
void Normalize(GLfloat v[3])
{
	GLfloat d = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
	if (d!=0.0)
	{
		v[0]/=d; 
		v[1]/=d; 
		v[2]/=d;
	}
}

class SphereTriGen 
{
public:
	SphereTriGen(MyObject& _obj, const Vec3& materialCol)
		:obj(_obj), col(materialCol)
	{}
	bool init(const vector<string>& sep) 
	{
		if (sep.size() < 2)
			return false;
	
		r = QString(sep[0].c_str()).toFloat();
		subdiv = QString(sep[1].c_str()).toInt();
		return true;
	}

	void gen();
	void oneToFourTriangle(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth);

private:
	float r;
	int subdiv;
	Vec3 col;
	MyObject& obj;
};

// Recursively subdivide the sphere
void SphereTriGen::oneToFourTriangle(GLfloat v1[3], GLfloat v2[3], GLfloat v3[3], int depth)
{
	GLfloat v12[3], v23[3], v31[3];
	int i;

	if (depth == 0) {
		Normalize(v1);
		Normalize(v2);
		Normalize(v3);

		Vec3 q[3] = { Vec3(v1,3), Vec3(v2,3), Vec3(v3,3) };
		obj.AddPoly(q, col, NULL, NULL, 3);

		/*glColor3f(0.5,0.5,0.5);
		glBegin(GL_TRIANGLES);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
		glEnd();

		glColor3f(0,0,0);
		glLineWidth(3);
		glBegin(GL_LINE_LOOP);
		glVertex3fv(v1);
		glVertex3fv(v2);
		glVertex3fv(v3);
		glEnd();*/
	}
	else
	{
		// midpoint
		for (i = 0; i < 3; i++) 
		{
			v12[i] = (v1[i]+v2[i])/2.0;
			v23[i] = (v2[i]+v3[i])/2.0;
			v31[i] = (v3[i]+v1[i])/2.0;
		}


		// lift midpoints on the sphere
		Normalize(v12);
		Normalize(v23);
		Normalize(v31);

		// subdivide new one-to-four triangles 
		oneToFourTriangle(v1, v12, v31, depth-1);
		oneToFourTriangle(v2, v23, v12, depth-1);
		oneToFourTriangle(v3, v31, v23, depth-1);
		oneToFourTriangle(v12, v23, v31, depth-1);
	}
}

void SphereTriGen::gen()
{

	// subdivide each face of the triangle
	for (int i = 0; i < 20; i++)
	{
		oneToFourTriangle(&icosahedronvertex[icosahedrontriangle[i][0]][0],
			&icosahedronvertex[icosahedrontriangle[i][1]][0],
			&icosahedronvertex[icosahedrontriangle[i][2]][0],
			subdiv);
	}

}

void Document::generateTriSphere(const vector<string>& sep)
{
	MyObject& obj = *m_frameObj;
	SphereTriGen gen(obj, Vec3(m_conf.materialCol));
	if (!gen.init(sep))
		return;
	gen.gen();
}