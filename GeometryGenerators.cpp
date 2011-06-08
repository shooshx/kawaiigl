#define __glu_h__

#include "Document.h"
#include "MyPolygon.h"
#include "ShapeIFS.h"
#include "Renderable.h"

#undef __glu_h__
#include "glu13/glu.h"




void Document::generateTorus(const vector<string>& sep)
{
	if (sep.size() < 4)
		return;
	
	const float rt = QString(sep[0].c_str()).toFloat();
	const float rc = QString(sep[1].c_str()).toFloat();
	const int numt = QString(sep[2].c_str()).toInt();
	const int numc = QString(sep[3].c_str()).toInt();

	if (numc == 0 || numt == 0)
		return;

	MyObject& obj = *m_frameObj;

	float s, t, x, y, z;

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

				x = (rt + rc * cos(s*TWOPI/numc)) * cos(t*TWOPI/numt);
				z = (rt + rc * cos(s*TWOPI/numc)) * sin(t*TWOPI/numt);
				y = rc * sin(s*TWOPI/numc);
				//
				//Vertex *v = new Vertex( x, y, z );
				Vec3 p(x, y, z);

				x = cos(t*TWOPI/numt) * cos(s*TWOPI/numc);
				z = sin(t*TWOPI/numt) * cos(s*TWOPI/numc);
				y = sin(s*TWOPI/numc);
				Vec3 n(x, y, z);

				q[k+2] = p;
				//v->setNormal( Vector( x, y, z ) );
				//qs->insert( v );
			}
			if (!q[0].isZero()) {

				obj.AddPoly(q, Vec3(m_conf.materialCol));
			}
		}
		//this->insert( qs );
	}

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
		:Renderable(NULL)
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
	virtual void render() {
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
	
	vector<Vec3> last = samp, cur(samp.size());
	for(int angi = 0; angi < circSamp; ++angi) {
		float theta = (TWOPI/circSamp)*angi;

		for(size_t j = 0; j < samp.size(); ++j) {
			Vec3 p = rotateAround(samp[j], curve.first(), axisVec, theta);
			cur[j] = p;

			if (j == 0 || angi == 0)
				continue;
			Vec3 q[4] = { last[j-1], last[j], cur[j], cur[j-1] };
			obj.AddPoly(q, Vec3(m_conf.materialCol));
		}
		last = cur;
	}

	for(size_t j = 1; j < samp.size(); ++j) {

		Vec3 q[4] = { last[j-1], last[j], samp[j], samp[j-1] };
		obj.AddPoly(q, Vec3(m_conf.materialCol));
	}

}