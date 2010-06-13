
#include "ShapeIFS.h"
#include "KwParser.h"

int MyPoint::g_ctorCount = 0, MyPoint::g_dtorCount = 0;
int MyPolygon::g_ctorCount = 0, MyPolygon::g_dtorCount = 0;

void MyObject::MyPointWrapper::detach(MyAllocator *m_alloc) const
{ 
	MyPoint *old = ptr;
	if (m_alloc != NULL)
		ptr = m_alloc->m_pointsPool.allocate();
	else
		ptr = new MyPoint; // TBD - happens with lines. remove when lines go display list.
	*ptr = *old;
}

// checks if a point is in m_tmppoints, if not, copy and insert it
// returns the pointer to the permanent point
MyPoint* MyObject::CopyCheckPoint(const Vec3 *c, const string* name)
{
	MyPoint p;
	p.setp(*c);
	p.name = name;
	const MyPointWrapper &realpntw = *m_tmppoints.insert(MyPointWrapper(&p));
	if (realpntw.ptr == &p) // it was inserted
		realpntw.detach(m_alloc);
	return realpntw.ptr;
}

void MyObject::AddLine(Vec3 *inp1, Vec3 *inp2, double inR, double inG, double inB, MyLine::ELineType type)
{	
	MyLine pln(NULL, NULL, inR, inG, inB, type);

	pln.p1 = CopyCheckPoint(inp1);
	pln.p2 = CopyCheckPoint(inp2);

	lnlst.push_back(pln);
}

// copies the points in the points array
MyPolygon* MyObject::AddPoly(Vec3 *inplst, TexAnchor *ancs, Texture *tex)
{
	MyPolygon *nply = m_alloc->m_polyPool.allocate();
	nply->init(ancs, tex);
	for (int lstp = 0; lstp < 4; ++lstp)
	{
		nply->vtx[lstp] = CopyCheckPoint(&inplst[lstp]);
	}

	plylst.push_back(nply);
	return nply;
}

// copies the points in the points array
// returns the polygon inserted for reference and additional tweaking
MyPolygon* MyObject::AddPoly(const IPoint *p1, const IPoint *p2, const IPoint *p3, const IPoint *p4,
							 TexAnchor *ancs, Texture *tex)
{
	MyPolygon *nply = m_alloc->m_polyPool.allocate();
	nply->init(ancs, tex);

	nply->vtx[0] = CopyCheckPoint(&p1->getCoord(), &p1->getName());
	nply->vtx[1] = CopyCheckPoint(&p2->getCoord(), &p2->getName());
	nply->vtx[2] = CopyCheckPoint(&p3->getCoord(), &p3->getName());
	if (p4 != NULL)
	{
		nply->vtx[3] = CopyCheckPoint(&p4->getCoord(), &p4->getName());
		nply->pnum = 4;
	}
	else 
	{
		nply->vtx[3] = NULL;
		nply->pnum = 3;
	}

	plylst.push_back(nply);
	return nply;
}



inline void MyObject::basicAddPoint(MyPoint *pnt) 
{ 
	pntlst.push_back(pnt); 
}

// add a polygon made of points in the repository, without duplicacy checks.
inline void MyObject::basicAddPoly(MyPoint *inparr[], TexAnchor *ancs, Texture *tex)
{ 
	MyPolygon *nply = m_alloc->m_polyPool.allocate();
	nply->init(inparr, ancs, tex);
	plylst.push_back(nply); 
}


uint qHash(const MyObject::MyPointWrapper &pnt)
{
	return pnt.ptr->hash();
}

bool operator==(const MyObject::MyPointWrapper &p1, const MyObject::MyPointWrapper &p2)
{
	return (*p1.ptr == *p2.ptr); // compare the actual values TBD..
}

void MyObject::detachPoints()
{
	for(int i = 0; i < points.size(); ++i)
	{
		MyPoint *p = points[i];
		MyPoint *copy = m_alloc->m_pointsPool.allocate();
		p->copyTo(*copy);
		points[i] = copy;
	}
	// update the polygons
	for(int i = 0; i < poly.size(); ++i)
	{
		MyPolygon *pol = poly[i];
		MyPolygon *copy = m_alloc->m_polyPool.allocate();
		*copy = *pol;
		poly[i] = copy;
		for(int j = 0; j < 4; ++j)
		{
			if (pol->vtx[j] != NULL)
				copy->vtx[j] = points[pol->vtx[j]->index];
			else
				copy->vtx[j] = NULL;
		}
	}
}


void MyObject::vectorify()
{
	int insPoly = 0, insLines = 0, insPoint = 0;
	// points
	nPoints = m_tmppoints.count() + pntlst.count();
	
	points.resize(nPoints);

	for (TPointsSet::const_iterator pit = m_tmppoints.begin(); pit != m_tmppoints.end(); ++pit)
	{
		MyPoint *p = const_cast<MyPoint*>(pit->ptr); // const_iterator is too restrictive
		p->index = insPoint;
		points[insPoint++] = p; 
	}
	m_tmppoints.clear();
	for (TPointsList::const_iterator plit = pntlst.constBegin(); plit != pntlst.constEnd(); ++plit)
	{
		MyPoint *p = *plit;
		p->index = insPoint;
		points[insPoint++] = p; 
	}
	pntlst.clear();

	nPolys = plylst.count();
	poly.resize(nPolys);
	triangles.clear();

	// add the polygons sorted according to their texture, NULL first
	Texture *availTexs[4] = { NULL }; // no more the 4 textues in an object.. actually, no more then 3 in reality.
	int availTexAdd = 1, availTexGet = 0;
	while (insPoly < nPolys) // until we got all the polygons in
	{
		Q_ASSERT(availTexGet < availTexAdd);
		for (TPolyList::iterator lit = plylst.begin(); lit != plylst.end() ; ++lit)
		{
			if ((*lit)->tex == availTexs[availTexGet])
				poly[insPoly++] = *lit;
			else if (((*lit)->tex != NULL) && ((*lit)->tex != availTexs[1]) && ((*lit)->tex != availTexs[2]) && ((*lit)->tex != availTexs[3]))
				availTexs[availTexAdd++] = (*lit)->tex;
		}
		++availTexGet;
	}
	plylst.clear();

	foreach(MyPolygon* p, poly)
	{
		if (p->pnum == 3)
			triangles.append(p);
	}

	nLines = lnlst.count();
	lines.resize(nLines);
	for (TLineList::iterator nit = lnlst.begin(); nit != lnlst.end(); ++nit)
	{
		lines[insLines++] = *nit;
	}
	lnlst.clear();

}

bool MyObject::clacNormals(bool vtxNorms)
{
	verterxNormals = vtxNorms;
	int pn;
	for (pn = 0; pn < nPoints; ++pn)
	{
		points[pn]->n.clear();
	}
	bool ret = true;

	for (int i = 0; i < nPolys; ++i)
	{
		MyPolygon *pol = poly[i];
		pol->calcTangents();

		if (!pol->calcNorm())
			ret = false; // but continue
		else
		{
			if (vtxNorms)
			{
				for(pn = 0; pn < poly[i]->pnum; ++pn)
				{
					MyPoint *p = pol->vtx[pn];
					p->n += pol->center;
					p->tangent += pol->tangent;
					p->bitangent += pol->bitangent;
				}
			}
		}
		
	}

	if (vtxNorms)
	{
		for (pn = 0; pn < nPoints; ++pn)
		{
			MyPoint *p = points[pn];
			p->n.unitize();
			//p->tangent.unitize();
			Vec3 &n = p->n;
			Vec3 &t = p->tangent;
			t = (t - n * Vec3::dotProd(n, t)).unitized(); // this still has a few problems.

			p->bitangent.unitize();

		}
	}
	return ret;
}


void ShapeIFS::vectorify()
{
	for (TObjectsMap::iterator it = m_objects.begin(); it != m_objects.end(); ++it) 
	{
		it->vectorify();
	}
}

struct PointPair
{
	PointPair(MyPoint *_p1, MyPoint *_p2) :p1(_p1), p2(_p2) {}
	// p1 and p2 are interchangable
	MyPoint *p1;
	MyPoint *p2;
};

uint qHash(const PointPair &pp)
{
	uint x = (pp.p1->hash() + pp.p2->hash()); // commutative
//	qDebug("%f,%f,%f:%f,%f,%f - %X", (*pp.p1).p[0], (*pp.p1).p[1], (*pp.p1).p[2], (*pp.p2).p[0], (*pp.p2).p[1], (*pp.p2).p[2], x);
	return x;
}

bool operator==(const PointPair &pp1, const PointPair &pp2)
{	// compare pointers
	return ((pp1.p1 == pp2.p1) && (pp1.p2 == pp2.p2)) || ((pp1.p2 == pp2.p1) && (pp1.p1 == pp2.p2)); // commutative
}
typedef QHash<PointPair, HalfEdge*> TPointPairHash;


bool MyObject::buildHalfEdges(THalfEdgeList& lst)
{
	int pli, i;

	TPointPairHash m_pntmap;
	m_pntmap.reserve(150);
	// map two points to the pointer is the half edge already between these points

	// create half edges for every polygon
	for(pli = 0; pli < nPolys; ++pli)
	{
		MyPolygon *pol = poly[pli];

		HalfEdge *he = NULL, *lasthe = NULL;
		for(i = pol->pnum - 1; i >= 0; --i)
		{
			//he = new HalfEdge(pol, pol->vtx[i], he);
			HalfEdge *prevhe = he;
			he = m_alloc->m_hePool.allocate();
			he->init(pol, pol->vtx[i], prevhe);
			lst.push_back(he);
			if (lasthe == NULL)
				lasthe = he;

			if (pol->vtx[i]->he == NULL) // this vertex doesn't have a half-edge yet
				pol->vtx[i]->he = he;

		}
		lasthe->next = he;
		pol->he = he;

		for(i = pol->pnum - 1; i >= 0; --i)
		{	
			PointPair pp(he->point, he->next->point);
			TPointPairHash::iterator it = m_pntmap.find(pp);
			if (it == m_pntmap.end()) // its not there, add it
				m_pntmap.insert(pp, he);
			else
			{
				HalfEdge *she = *it;
				if ((she->next->point != he->point) || (he->next->point != she->point))
					return false;
				he->pair = she;
				she->pair = he;
				m_pntmap.erase(it); // no longer needed in the map
			}
			he = he->next;
		}
		// maximum size of m_pntmap is 134
	}

	// fill holes
	while (!m_pntmap.empty())
	{
		HalfEdge *starthe = m_pntmap.begin().value();
		HalfEdge *he = starthe;
		HalfEdge *nprev = NULL, *nhe = NULL;

		HalfEdge *henext = he->next;
		while (henext->pair != NULL)  // search for the next one in the hole
			henext = henext->pair->next; 

		do {

			nhe = m_alloc->m_hePool.allocate();
			nhe->init(he->poly, henext->point, nprev);
			he->pair = nhe;
			nhe->pair = he;
			m_pntmap.remove(PointPair(he->point, henext->point));
			nprev = nhe;

			he = henext;
			henext = he->next;
			while (henext->pair != NULL && he != starthe && henext != starthe)  // search for the next one in the hole
				henext = henext->pair->next; 
		}
		while (he != starthe);

		he->pair->next = nhe;

	}

	return (m_pntmap.empty());
}


bool MyObject::subdivide(bool smooth)
{
	int pli, i;

	m_alloc->m_hePool.clear();
	THalfEdgeList helst;
	helst.reserve(5000);
	if (!buildHalfEdges(helst))
		return false;

	// create new face points
	for(pli = 0; pli < nPolys; ++pli)
	{
		MyPolygon &pol = *poly[pli];
		pol.f = m_alloc->m_pointsPool.allocate();
		pol.f->clear();
		pol.f->col.clear();
		for(i = 0; i < pol.pnum; ++i)
		{
			*pol.f += *pol.vtx[i];
			pol.f->col += pol.vtx[i]->col; // color
			pol.vtx[i]->touched = false; // while we're at it, reset the point for the current iteration
		}
		*pol.f /= (float)pol.pnum;
		pol.f->col /= (float)pol.pnum;
		basicAddPoint(pol.f);
	}
	
	// create new edge points
	for(i = 0; i < helst.size(); ++i)
	{
		HalfEdge *he = helst[i];
		if (he->edgePoint != NULL) 
			continue;

		he->edgePoint = m_alloc->m_pointsPool.allocate();
		(*he->edgePoint) = *he->point;
		(*he->edgePoint) += *he->pair->point;
		if (smooth)
		{
			(*he->edgePoint) += *he->poly->f;
			(*he->edgePoint) += *he->pair->poly->f;
			(*he->edgePoint) /= 4.0f;
		}
		else
			(*he->edgePoint) /= 2.0f;
		// average color
		he->edgePoint->col = he->point->col;
		he->edgePoint->col += he->pair->point->col;
		he->edgePoint->col /= 2.0f;

		he->pair->edgePoint = he->edgePoint;
		basicAddPoint(he->edgePoint);
	}

	// create new polygons
	for(int pli = 0; pli < nPolys; ++pli)
	{
		MyPolygon &pol = *poly[pli];
		MyPoint *e[4] = {0};

		i = 0;
		HalfEdge *he = pol.he;
		do {
			e[i++] = he->edgePoint;
			he = he->next;
		} while (he != pol.he);

		
		if (smooth)
		{
			for(i = 0; i < pol.pnum; ++i)
			{
				if (pol.vtx[i]->touched)
					continue;

				Vec3 F, E; // average of all Face points near of op[i]

				double n = 0;
				he = pol.vtx[i]->he;
				do {
					F += he->poly->f->p;
					if (he->point == pol.vtx[i]) // pointer comparison
						E += he->pair->point->p;
					else
						E += he->point->p;

					he = he->pair->next;
					n += 1.0;
				} while (he != pol.vtx[i]->he);

			/*	F /= n;
				E += *poly.vtx[i] * 3.0;
				E /= n * 2.0;
				op[i] = (F + (2.0 * E) + ((n - 3.0)*(*poly.vtx[i])))/n;*/

				// store the result intermediatly in 'n' so we wont affect the on-going calculation process
				double nsqr = n*n;
				pol.vtx[i]->n = (((n - 2.0)/n)*(pol.vtx[i]->p)) + (F/(nsqr)) + (E/(nsqr));

				pol.vtx[i]->he = NULL; // reset the half edge for the next time (and for the halfedge impending death)
				pol.vtx[i]->touched = true;
				basicAddPoint(pol.vtx[i]); // add it again becuse the array is going to be deleted
			}
		}
		else
		{
			for(i = 0; i < pol.pnum; ++i)
			{
				if (pol.vtx[i]->touched)
					continue;

				pol.vtx[i]->he = NULL; // reset the half edge for the next time (and for the halfedge impending death)
				pol.vtx[i]->touched = true;
				basicAddPoint(pol.vtx[i]); // add it again becuse the array is going to be deleted
			}
		}

		pol.he = NULL;		

		MyPoint *laste = e[pol.pnum - 1];
//		MyPoint *lastp = pol.vtx[pol.pnum - 1];

		MyPoint *pl1[] = { pol.vtx[0], e[0], pol.f, laste };
		MyPoint *pl2[] = { e[0], pol.vtx[1], e[1], pol.f };
		MyPoint *pl3[] = { pol.f, e[1], pol.vtx[2], e[2] };
		MyPoint *pl4[] = { e[3], pol.f, e[2], pol.vtx[3] };


		TexAnchor tv[] = { pol.texAncs[0], pol.texAncs[1], pol.texAncs[2], pol.texAncs[3] }; // original texture anchors
		TexAnchor tf = (tv[0] + tv[2]) / 2.0;
		TexAnchor te[] = { (tv[0] + tv[1]) / 2.0f, (tv[1] + tv[2]) / 2.0f, (tv[2] + tv[3]) / 2.0f, (tv[3] + tv[0])/2.0f };

		TexAnchor lastte = te[pol.pnum - 1];
		TexAnchor tx1[] = { tv[0], te[0], tf, lastte };
		TexAnchor tx2[] = { te[0], tv[1], te[1], tf };
		TexAnchor tx3[] = { tf, te[1], tv[2], te[2] };
		TexAnchor tx4[] = { te[3], tf, te[2], tv[3] };

		basicAddPoly(pl1, tx1, pol.tex);
		basicAddPoly(pl2, tx2, pol.tex);
		basicAddPoly(pl3, tx3, pol.tex);
		if (pol.pnum == 4)
			basicAddPoly(pl4, tx4, pol.tex);

	}

	if (smooth)
	{
		// go over all the points the the points array of the object. these are the original points
		for (i = 0; i < nPoints; ++i)
			points[i]->p = points[i]->n;
	}

	vectorify();
	return true;
}

class OrderedPair
{
public:
	OrderedPair(int _a, int _b) :a(_a), b(_b)
	{
		if (a > b)
			qSwap(a,b);
	}
	bool operator==(const OrderedPair& o) const
	{
		return a==o.a && b==o.b;
	}

	bool operator<(const OrderedPair& o) const
	{
		if (a != o.a)
			return a < o.a;
		return b < o.b;

	}

	int a, b; // a<b
};



void MyObject::saveAs(const QString& filename, const QString& format, ESaveWhat saveWhat)
{

	QFile file(filename);
	file.open(QFile::WriteOnly);
	file.setTextModeEnabled(true);
	QTextStream out(&file);

	QString fmt = format.toLower();

	if (fmt == "obj")
	{
		for(int i = 0; i < nPoints; ++i)
		{
			MyPoint *p = points[i];
			out << "v " << p->p[0] << " " << p->p[1] << " " << p->p[2] << "\n";
		}
		out << "\n";

		for(int i = 0; i < nPolys; ++i)
		{
			MyPolygon *p = poly[i];
			out << "f ";
			for(int i = 0; i < 4; ++i)
				out << p->vtx[i]->index + 1<< " ";
			out << "\n";
		}
		out << "\n";
	}
	else if (fmt == "json")
	{
		out << "{\n";
		out << " \"vertexPositions\" : [";
		MyPoint *p = points[0];
		out << p->p[0] << "," << p->p[1] << "," << p->p[2];
		for(int i = 1; i < nPoints; ++i)
		{
			p = points[i];
			out << "," << p->p[0] << "," << p->p[1] << "," << p->p[2];
		}
		out << "],\n";
		out << " \"vertexNormals\" : [";
		p = points[0];
		out << p->n[0] << "," << p->n[1] << "," << p->n[2];
		for(int i = 1; i < nPoints; ++i)
		{
			p = points[i];
			out << "," << p->n[0] << "," << p->n[1] << "," << p->n[2];
		}
		out << "],\n";

		if (saveWhat == SaveFaces)
		{
			if (false)
			{
				out << " \"quads\" : [";
				MyPolygon *pl = poly[0];
				out << pl->vtx[0]->index << "," << pl->vtx[1]->index << "," << pl->vtx[2]->index << "," << pl->vtx[3]->index;
				for(int i = 1; i < nPolys; ++i)
				{
					pl = poly[i];
					out << ",";
					out << pl->vtx[0]->index << "," << pl->vtx[1]->index << "," << pl->vtx[2]->index << "," << pl->vtx[3]->index;
				}
				out << "]\n";
			}
			else
			{
				out << " \"triangles\" : [";
				MyPolygon *pl = poly[0];
				out << pl->vtx[0]->index << "," << pl->vtx[1]->index << "," << pl->vtx[2]->index << ",";
				out << pl->vtx[0]->index << "," << pl->vtx[2]->index << "," << pl->vtx[3]->index;
				for(int i = 1; i < nPolys; ++i)
				{
					pl = poly[i];
					out << ",";
					out << pl->vtx[0]->index << "," << pl->vtx[1]->index << "," << pl->vtx[2]->index << ",";
					out << pl->vtx[0]->index << "," << pl->vtx[2]->index << "," << pl->vtx[3]->index;
				}
				out << "]\n";
			}
		}		
		else if (saveWhat == SaveEdges)
		{
			// find unique pairs
			typedef map<OrderedPair, int> TUniqIndexes;
			TUniqIndexes uniq;
			for(int i = 0; i < nPolys; ++i)
			{
				MyPolygon *pl = poly[i];
				for(int pli = 0; pli < 4; ++pli)
				{
					OrderedPair o(pl->vtx[pli]->index, pl->vtx[(pli+1)%4]->index);
					TUniqIndexes::iterator fit = uniq.find(o);
					if (fit != uniq.end())
						++(fit->second);
					else
						uniq[o] = 1;
				}
			}

			out << " \"lines\" : [";
			TUniqIndexes::iterator it = uniq.begin();
			out << it->first.a << "," << it->first.b;
			for(++it; it != uniq.end(); ++it)
			{
				out << "," << it->first.a << "," << it->first.b;
			}
			out << "]\n";

		}
		out << "}\n";
	}


}


void MyAllocator::init(int points, int poly, int he)
{
	// never shrinks... 
	if (points > m_pointsPool.size())
		m_pointsPool.init(points);
	if (poly > m_polyPool.size())
		m_polyPool.init(poly);
	if (he > m_hePool.size())
		m_hePool.init(he);
}

void MyAllocator::checkMaxAlloc() const
{
	m_pointsPool.calcMax();
	m_polyPool.calcMax();
	m_hePool.calcMax();
	if ((m_pointsPool.getMaxAlloc() > m_pointsPool.size()) ||
		(m_polyPool.getMaxAlloc() > m_polyPool.size()) ||
		(m_hePool.getMaxAlloc() > m_hePool.size()))
		qDebug("warning! memory pools not optimal. max:%d, %d, %d", m_pointsPool.getMaxAlloc(),  m_polyPool.getMaxAlloc(),  m_hePool.getMaxAlloc());
}


