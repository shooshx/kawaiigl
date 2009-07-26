//#include "stdafx.h"
//#include "windows.h"
#include <QFile>
#include <QTextStream>
#include <QHash>
#include <QProgressDialog>
#include <QCoreApplication>

#include "RMesh.h"
#include "TrMatrix.h"
//#include "curvature_estimator.h"

// going to eat that vector
int RMesh::addFaceTriangulate(RMesh::TIndexList& f)
{
	if (f.size() < 3)
		return 0;

	int count = 0;
	while (1)
	{
		if (f.size() == 3)
		{
			addTriangle(f[0], f[1], f[2]);
			return ++count;
		}
		
		float mind = FLT_MAX;
		int minstart = -1;
		for(int tt = 0; tt < f.size(); ++tt)
		{
			int other = (tt + 2) % f.size();
			float d = Vec::distance(find_vertex(f[tt])->point(), find_vertex(f[other])->point());
			if (d < mind)
			{
				mind = d;
				minstart = tt;
			}
		}
		int o1 = (minstart + 1) % f.size(), o2 = (minstart + 2) % f.size();
		addTriangle(f[minstart], f[o1], f[o2]);
		++count;
		TIndexList left;
		for(int tt = (minstart + 2) % f.size(); tt != (minstart + 1) % f.size(); tt = (tt + 1)%f.size())
			left.append(f[tt]);

		f = left;
	}

}

Vec RMesh::centerOfMass()
{
	// compute for every face
	for(int i = 0; i < numFaces(); ++i)
	{
		Face &f = m_face[i];
		f.m_center.clear();
		for(int i = 0; i <  f.size(); ++i)
			f.m_center += f.vertex(i)->point();
		f.m_center /=  f.size();
	}

	// now for all of the mess seperatly.
	Vec centerOfMass(0,0,0);
	for (RMesh::Vertex_iterator v = this->vertices_begin(); v != this->vertices_end(); v++)
	{
		centerOfMass = centerOfMass + (v->point()); // - CGAL::ORIGIN);
	}

	unsigned int size = this->size_of_vertices();
	m_computedCenterOfMass = Vec(centerOfMass.x/size, centerOfMass.y/size, centerOfMass.z/size);

	return m_computedCenterOfMass;
}


void RMesh::compute_bounding_box()
{
	if (size_of_vertices()	== 0)
		return;

	float xmin, xmax, ymin, ymax, zmin, zmax;

	Vertex_iterator	pVertex	=	vertices_begin();
	xmin = xmax = pVertex->point().x;
	ymin = ymax = pVertex->point().y;
	zmin = zmax = pVertex->point().z;
	for(;pVertex !=	vertices_end();pVertex++)
	{
		const Vec& p = pVertex->point();
		xmin	=	qMin(xmin,p.x);
		ymin	=	qMin(ymin,p.y);
		zmin	=	qMin(zmin,p.z);
		xmax	=	qMax(xmax,p.x);
		ymax	=	qMax(ymax,p.y);
		zmax	=	qMax(zmax,p.z);
	}

	m_min = Vec(xmin, ymin, zmin);
	m_max = Vec(xmax, ymax, zmax);
	m_diagLength = Vec::distance(m_min, m_max);
	m_axisLength = Vec(m_min, m_max);
	m_minMaxCenter = (m_min + m_max) / 2.0f;
}



void RMesh::Face::computeNormal()
{
	Vec v1(vertex(1)->point(), vertex(0)->point());
	Vec v2(vertex(2)->point(), vertex(0)->point());
	m_normal = Vec::crossProd(v1, v2);
	m_normal.unitize();
	
}


void RMesh::Vertex::computeNormal()
{
	for(int i = 0; i < numFaces(); ++i)
	{
		Facet_handle v = face(i);
		m_normal += v->normal();
	}
	m_normal.unitize();
}



void RMesh::buildVerticesInFaces()
{
	for(int i = 0; i < numVtx(); ++i)
	{
		Vertex &v = m_vtx[i];
		v.faceIndexs().clear();
	}
	for(int fi = 0; fi < numFaces(); ++fi)
	{
		Face &face = m_face[fi];
		for(int i = 0; i < face.size(); ++i)
		{
			m_vtx[face.vertexIndex(i)].faceIndexs().push_back(fi);
		}
	}
}


int RMesh::addEdge(int a, int b)
{
	m_edge.push_back(Edge(a, b, m_edge.size(), this));
	Edge *e = &m_edge.back();
	const Vec& av = find_vertex(e->a())->point();
	const Vec& bv = find_vertex(e->b())->point();
	e->m_length = Vec::distance(av, bv);
	e->m_center = (av + bv) / 2.0f;
	return e->index();
}

// returns the edge added or found
int RMesh::addEdgeOnceUsing(int s, int l, TSmallLargeEdgeI &bld, int (RMesh::*addMethod)(int a, int b))
{
	if (l < s)
		qSwap(s, l);

	TSmallLargeEdgeI::iterator sit = bld.find(s);
	if (sit == bld.end()) // small index is not found
	{
		sit = bld.insert(s, TLargeEdgeI());
	}
	TLargeEdgeI &smap = *sit;
	TLargeEdgeI::iterator lit = smap.find(l);
	if (lit == smap.end()) // large index not found
	{
		int retIndex = (this->*addMethod)(s, l);
		lit = smap.insert(l, retIndex);
	}

	return *lit;
}

void RMesh::buildEdges()
{
	TSmallLargeEdgeI edgeMap; 

	edgeMap.clear();
	// map small index to map large index to the edge in the m_edge list
	for(int fi = 0; fi < numFaces(); ++fi)
	{
		//printf("%d- ", fi);
		Face &face = m_face[fi];

		int v0i = face.vertexIndex(0);
		int v1i = face.vertexIndex(1);
		int v2i = face.vertexIndex(2);

		int e0i = addEdgeOnceUsing(v0i, v1i, edgeMap, &RMesh::addEdge);
		int e1i = addEdgeOnceUsing(v1i, v2i, edgeMap, &RMesh::addEdge);
		int e2i = addEdgeOnceUsing(v2i, v0i, edgeMap, &RMesh::addEdge);

		face.m_edges[0] = e0i;
		face.m_edges[1] = e1i;
		face.m_edges[2] = e2i;

/*		Vertex_handle v0 = find_vertex(v0i);
		v0->m_edges.append(e0i);
		v0->m_edges.append(e2i);
		Vertex_handle v1 = find_vertex(v1i);
		v1->m_edges.append(e0i);
		v1->m_edges.append(e1i);
		Vertex_handle v2 = find_vertex(v2i);
		v2->m_edges.append(e1i);
		v2->m_edges.append(e2i);*/

		///printf("%d %d %d\n", e0i, e1i, e2i);

		find_edge(e0i)->m_faces.append(fi);
		find_edge(e1i)->m_faces.append(fi);;
		find_edge(e2i)->m_faces.append(fi);

		// don't keep pointers since the vector might be reallocated
		// make sure the shortest one is in edges[0]
		/*if (e2->length() < e1->length())
			qSwap(e2, e1);
		if (e1->length() < e0)->length())
			qSwap(e1, e0);
		if (find_edge(e2)->length() < find_edge(e1)->length())
			qSwap(e2, e1); don't need that */ 


	}
}




// for each face according to its vertices orientation
void RMesh::compute_normals_per_facet()
{
	for(int i = 0; i < numFaces(); ++i)
	{
		Face &face = m_face[i];
		face.computeNormal();
	}
}

void RMesh::average_normals_per_facet()
{
	compute_normals_per_facet(); //TBD PATCH
}

// for each vertex according to each faces normals
void RMesh::compute_normals_per_vertex()
{
	for(int i = 0; i < numVtx(); ++i)
	{
		Vertex &v = m_vtx[i];
		v.computeNormal();
	}
}

// only triangles
RMesh::TIndexList RMesh::Vertex::neiVertices() const
{
	TIndexList ret;

	for(int i = 0; i < numFaces(); ++i)
	{
		Face_const_handle f = face(i);
		for(int vi = 0; vi < f->size(); ++vi)
		{
			int cv = f->vertexIndex(vi);
			if (cv != index() && !ret.contains(cv)) 
				ret.append(cv);
		}

	}
	return ret;
}

RMesh::TVtxHList RMesh::Vertex::neiVerticesH() const
{
	TIndexList il = neiVertices();
	TVtxHList ret;
	foreach(int i, il)
		ret.append(m_mymesh->find_vertex(i));
	return ret;
}

// return a list of the faces which have a common edge with this face
RMesh::TIndexList RMesh::Face::neiFaces() const
{
	TIndexList ret;
	for(int i = 0; i < size(); ++i)
	{
		Edge_const_handle e = edge(i);
		for (int fi = 0; fi < e->numFaces(); ++fi)
		{
			int f = e->faceIndex(fi);
			if (f != index())
				ret.append(f);
		}
	}
	return ret;
}

bool RMesh::Face::isNeiFace(int findex) const
{
	for(int i = 0; i < size(); ++i)
	{
		Edge_const_handle e = edge(i);
		for (int fi = 0; fi < e->numFaces(); ++fi)
		{
			int f = e->faceIndex(fi);
			if (f == findex)
				return true;
		}
	}
	return false;
}



bool RMesh::find_faces_with(int vi1, int vi2, TIndexList& res) const
{
	Vertex_const_handle v1 = find_vertex(vi1);
	TIndexList resa, resb;
	res.clear();
	for(int i = 0; i < v1->numFaces(); ++i)
	{
		Face_const_handle f = v1->face(i);
		int fi = f->containsVtx(vi2);
		if (fi != -1)
		{ // what order are the indexes
			if (f->vertexIndex((fi + 1)%3) == vi1)
				resa.append(v1->faceIndex(i));
			else
				resb.append(v1->faceIndex(i));

		}
	}
	res = resa + resb;
	return (res.size() == 2); // could be more or less than 2. return true if 2, this is the condition that it's a regular mesh.
}


void RMesh::finalize(bool needEdges)
{
	buildVerticesInFaces();

	if (!m_externalVtxNormals && !m_externalFaceNormals)
	{
		compute_normals_per_facet();
		compute_normals_per_vertex();
	}
	else if (!m_externalFaceNormals)
	{
		average_normals_per_facet();
	}

	compute_bounding_box();		
	//mesh->estimateCurvature();
	compute_triangle_surfaces();	
	centerOfMass();
	//m_mesh->compute_volume();
	
	if (needEdges)
		buildEdges();
}



number_type RMesh::compute_triangle_surfaces()
{
	m_totalSurface = 0.0;
	for(int i = 0; i < numFaces(); ++i)
	{
		Face &face = m_face[i];

		Vec v1(face.vertex(1)->point(), face.vertex(0)->point());
		Vec v2(face.vertex(2)->point(), face.vertex(0)->point());
		Vec cross = Vec::crossProd(v1, v2);
	
		face.surface() = cross.length() / 2.0f;
		m_totalSurface += face.surface();
	}

	return m_totalSurface;
}



void RMesh::reverseNormals()
{
	RMesh::Vertex_iterator vit = vertices_begin();
	RMesh::Vertex_iterator vit_end = vertices_end();

	for (;vit != vit_end; vit++) {
		vit->normal() *= -1.0f;
	}

	RMesh::Facet_iterator fit = facets_begin();
	RMesh::Facet_iterator fit_end = facets_end();

	for (;fit != fit_end; fit++) {
		fit->normal() *= -1.0f;
	}
}

void RMesh::reverseFaces()
{

	RMesh::Facet_iterator fit = facets_begin();
	RMesh::Facet_iterator fit_end = facets_end();

	for (;fit != fit_end; fit++) {
		fit->reverseOrder();
	}
}

Vec RMesh::polyCenterOfMass() const
{
	number_type c[3] = { 0.0, 0.0, 0.0 };
	number_type centerWeight = 0.0;

	for (RMesh::Facet_const_iterator f = facets_begin(); f != facets_end(); ++f)
	{
		//Mesh::Facet_handle f = mesh->find_facet(m_facets[i]);
		Vec fcenter = f->center();

		number_type facetSurface = f->surface();
		c[0] += fcenter.x * facetSurface;
		c[1] += fcenter.y * facetSurface;
		c[2] += fcenter.z * facetSurface;
		centerWeight += facetSurface;

	}

	c[0] /= centerWeight;
	c[1] /= centerWeight;
	c[2] /= centerWeight;
	return Vec(c[0], c[1], c[2]);

}



void RMesh::saveSubMeshFaceValuesTxt(const QString& filename, const TIndexList& facesList, float (RMesh::Face::*srcgetter)() const) const
{
	QFile file(filename);
	file.open(QFile::WriteOnly | QFile::Truncate);
	file.setTextModeEnabled(true);
	QTextStream out(&file);

	for(TIndexList::const_iterator pfi = facesList.constBegin(); pfi != facesList.constEnd(); ++pfi)
	{
		Face_const_handle f = find_facet(*pfi);
		out << (f->*srcgetter)() << "\n";
	}
}

void RMesh::saveSubMesh(const QString& filename, const TIndexList& facesList) const
{
	QFile file(filename);
	file.open(QFile::WriteOnly | QFile::Truncate);
	file.setTextModeEnabled(true);
	QTextStream out(&file);

	out << "OFF\n";

	int nVtx = numVtx();
	int newIndexCounter = 0;
	// do we need only a subset of the vertices?
	QVector<int> newToOld;
	QHash<int, int> oldToNew; // old index -> new index.
	if (facesList.size() != numFaces())
	{
		for(TIndexList::const_iterator pfi = facesList.constBegin(); pfi != facesList.constEnd(); ++pfi)
		{
			Face_const_handle f = find_facet(*pfi);
			for(int vfi = 0; vfi < f->size(); ++vfi)
			{
				int vi = f->vertexIndex(vfi);
				if (!oldToNew.contains(vi))
					oldToNew.insert(vi, newIndexCounter++);
			}
		}
		newToOld.resize(newIndexCounter);

		// invert the map because we want a map from new to old
		for(QHash<int, int>::iterator hit = oldToNew.begin(); hit != oldToNew.end(); ++hit)
		{
			newToOld[hit.value()] = hit.key();
		}

		nVtx = newIndexCounter;

	}

	// number of vertices, faces, 0
	out << nVtx << " " << facesList.size() << " 0\n";
	for(int i = 0; i < nVtx; ++i)
	{
		Vertex_const_handle v;
		if (newIndexCounter > 0)
			v = find_vertex(newToOld[i]);
		else
			v = find_vertex(i);
		const Vec &p = v->point();
		out << p.x << " " << p.y << " " << p.z << "\n";
	}

	for(TIndexList::const_iterator pfi = facesList.constBegin(); pfi != facesList.constEnd(); ++pfi)
	{
		Face_const_handle f = find_facet(*pfi);
		out << f->size();
		for(int vfi = 0; vfi < f->size(); ++vfi)
		{
			int fii = f->vertexIndex(vfi);
			if (newIndexCounter > 0)
				fii = oldToNew[fii];
			out << " " << fii;
		}
		out << "\n";
	}

}


void RMesh::translateCenter(const Vec& c)
{
	for(int i = 0; i < numVtx(); ++i)
	{
		Vertex_handle v = find_vertex(i);
		v->point() -= c;
	}
	compute_bounding_box();		
	centerOfMass();
}


void RMesh::rescaleAndCenter(float destdialen)
{
	Vec dia = m_max - m_min;
	Vec center = (m_max + m_min) / 2.0;

	float dialen = qMax(dia.x, dia.y);
	float scale = destdialen/dialen;

	for(int i = 0; i < numVtx(); ++i)
	{
		Vertex_handle v = find_vertex(i);
		Vec &p = v->point();
		p -= center;
		p *= scale;
	}

	compute_bounding_box();		
	centerOfMass();
}


void RMesh::transformVertices(const TrMatrix& tr)
{
	for(Vertex_iterator it = vertices_begin(); it != vertices_end(); ++it)
	{
		Vec& p = it->m_p;
		p = tr.multVec(Vec4(p)).toVec();
	}

	// do some of the things done in finalize since all points were changed.
	compute_normals_per_facet();
	compute_normals_per_vertex();
	compute_bounding_box();	
	compute_triangle_surfaces();	
	centerOfMass();
}


void RMesh::removeVertex(int index)
{
	find_vertex(index)->m_tran = 0;
}
void RMesh::commitRemove()
{
	// first go over all the points and tell them where they go
	int count = 0;
	for(int i = 0; i < numVtx(); ++i)
	{
		Vertex_handle v = find_vertex(i);
		if (v->m_tran == -1)
			v->m_index = count++;
		else
			v->m_index = -1; // to be removed
		v->m_tran = -1;
	}
	// no go over the faces and transfer their point references
	count = 0;
	for(int i = 0; i < numFaces(); ++i)
	{
		Face_handle f = find_facet(i);
		bool remove = false;
		for(int ii = 0; ii < f->size(); ++ii)
		{
			int newvi = find_vertex(f->m_pi[ii])->m_index;
			f->m_pi[ii] = newvi;
			remove |= (newvi == -1);
		}
		if (!remove)
			f->m_index = count++;
		else 
			f->m_index = -1;
	}
	// actually remove the vertices
	Vertex_iterator vit = vertices_begin();
	while(vit != vertices_end())
	{
		if (vit->m_index == -1)
			vit = m_vtx.erase(vit);
		else
			++vit;
	}

	// remove faces
	Facet_iterator fit = facets_begin();
	while(fit != facets_end())
	{
		if (fit->m_index == -1)
			fit = m_face.erase(fit);
		else
			++fit;
	}
}


// compute average edge length around a vertex
float RMesh::min_edge_length_around(Vertex_handle pVertex)
{
	float min_edge_length = FLT_MAX;

	RMesh::TIndexList circVtx = pVertex->neiVertices();
	for(int i = 0; i < circVtx.size(); ++i)
	{
		RMesh::Vertex_handle cVertex = find_vertex(circVtx[i]);
		Vec vec = pVertex->point() - cVertex->point();
		float len = vec.length();
		if (len < min_edge_length)
			min_edge_length = len;
	}
	return min_edge_length;
}

#if 0


void RMesh::calcCurvature(int ringSize)
{
	m_vtxCurvature.resize(numVtx());

	CCurvature_estimator ce(this);
	ce.run(ringSize);

}


Curvature RMesh::calcCurvatureFor(int vtxIndex)
{
	m_vtxCurvature.resize(numVtx());

	CCurvature_estimator ce(this);
	Vertex_handle v = find_vertex(vtxIndex);
	ce.run(v);
	return v->curvature();
}


void RMesh::calcDotCurve()
{
	m_vtxDotCurve.resize(numVtx());
	for(Vertex_iterator it = vertices_begin(); it != vertices_end(); ++it)
	{
		Vertex_handle vh = &*it;
		TIndexList nei = vh->neiVertices();
		int negdot = 0;
		float dotsum = 0.0f;
		for(int i = 0; i < nei.size(); ++i)
		{
			Vertex_handle vc = find_vertex(nei[i]);
			Vec vv = vc->point() - vh->point();
			vv.unitize();
		//	if (Vec::dotProd(vv, vh->normal()) < 0.0f)
		//		++negdot;
			dotsum += Vec::dotProd(vv, vh->normal());
		}
		//vh->dotCurve() = (((float)nei.size() / 2.0) - negdot) / (float)nei.size();
		vh->dotCurve() = dotsum;
		
	}

}


void RMesh::buildKdTree()
{
	m_kdtree.createKdTree(m_vtx, &RMesh::Vertex::point);
}


float RMesh::distFromMesh(const Vec& v)
{
	int i = m_kdtree.findNearest(v);
	RMesh::Vertex_handle vh = find_vertex(i);
	const Vec &cvtx = vh->point();
	float d = Vec::distance(v, cvtx);
	Vec tov = cvtx - v;
	if (Vec::dotProd(tov, vh->normal()) < 0)
		d = -d;
	return d;
}



int RMesh::addCloseEdge(int a, int b)
{
	m_closeEdge.push_back(ClosenessEdge(a, b));
	return m_closeEdge.size() - 1;
}


void RMesh::makeClosenessEdges()
{
	int hasCount = 0;
	TSmallLargeEdgeI makec;
	for(int fi = 0; fi < numFaces(); ++fi)
	{
		Face_const_handle f = find_facet(fi);
		const TIndexList& clsf = f->closeFaces();
		if (clsf.size() == 0)
			continue;

		++hasCount;
		for(int cfi = 0; cfi < clsf.size(); ++cfi)
		{
			addEdgeOnceUsing(fi, clsf[cfi], makec, &RMesh::addCloseEdge);
		}
	}

	printf("Faces with close edges=%d\n", hasCount);

}


#endif


////////////////////////// SDF stuff /////////////////////////////////////////////////////////
//******************************************************************************************//

// 
// Ray_3 get_normal_opposite(const RMesh::Vertex_handle& v)
// {
// 	//direction opposite the normal
// 	Vec direction = v->normal().toCgal() * -1;	
// 
// 	//construct segment from vertex pointing towards direction
// 	Ray_3 ray(
// 		v->point().toCgal() + direction * 1e-5,
// 		direction);
// 
// 	return ray;
// }
// 
// Ray_3 get_normal_opposite(const Vec& p, const Vec& n)
// {
// 	//direction opposite the normal
// 	Vec direction = n * -1;	
// 
// 	//construct segment from vertex pointing towards direction
// 	Ray_3 ray(
// 		p + direction * 1e-5,
// 		direction);
// 
// 	return ray;
// }
// 



void RMesh::compute_feature_range_on_facets(number_type& minVal, number_type& maxVal, float (RMesh::Face::*datgetter)() const)
{
	float minFaceFeature = FLT_MAX;
	float maxFaceFeature = -FLT_MAX;

	RMesh::Facet_const_iterator it = facets_begin();
	RMesh::Facet_const_iterator it_end = facets_end();
	for (;it != it_end; it++) 
	{
		if (((*it).*datgetter)() < minFaceFeature) 
			minFaceFeature = ((*it).*datgetter)();
		if (((*it).*datgetter)() > maxFaceFeature) 
			maxFaceFeature = ((*it).*datgetter)();
	}

	minVal = minFaceFeature;
	maxVal = maxFaceFeature;
}

#if 0

struct RVertexCompare 
{
	bool operator()(const RMesh::Vertex_handle& a, const RMesh::Vertex_handle& b) const
	{	// apply operator< to operands
		return (*a < *b);
	}
};
struct RVertexDistanceLess 
{
	bool operator() (const RMesh::Vertex_handle& v1, const RMesh::Vertex_handle& v2) {
		return v1->distance() > v2->distance();
	}
};


typedef std::priority_queue<RMesh::Vertex_handle, std::vector<RMesh::Vertex_handle>, RVertexDistanceLess> TVertexDistanceHeap; // vertexDistanceHeap_t


void RMesh::dijkstra(RMesh::Vertex_handle v)
{
	for (RMesh::Vertex_iterator it = vertices_begin(); it != vertices_end(); ++it)
	{
		it->distance(FLT_MAX);
		//it->m_visited = false;
	}

	TVertexDistanceHeap vdh;

	int sanityCounter = 0;

	v->distance(0.0);	
	vdh.push(v);
	do {
		RMesh::Vertex_handle t = vdh.top();
		vdh.pop();
		//t->visited = true;

		TIndexList vnei = t->neiVertices();

		for (int vii = 0; vii < vnei.size(); ++vii)
		{
			RMesh::Vertex_handle n = find_vertex(vnei[vii]);
			float alt = t->distance() + Vec::distance(t->point(), n->point());
			if (alt < n->distance()) 
			{
				n->distance() = alt;
				// we don't have the edge
				// not using closeness.
				vdh.push(n);
			}
		}

		sanityCounter++;

// 		if (sanityCounter > size_of_vertices() * 2)
// 		{
// 			printf("dijkstra sanity failed!\n");
// 			break;
// 		}
	} while (!vdh.empty());
}




void RMesh::computeCentricity(QWidget* guiParent)
{
	//	number_type maxCentricity = -FLT_MAX;
	//	number_type minCentricity = FLT_MAX;

	QProgressDialog *prog = NULL;
	if (guiParent)
		prog = new QProgressDialog("Building centricity", "Stop", 0, numVtx(), guiParent);
		
	int counter = 0;
	int total = size_of_vertices();
	for (RMesh::Vertex_iterator it = vertices_begin(); it != vertices_end(); ++it)
	{
		if (guiParent)
		{
			prog->setValue(counter);
			QCoreApplication::processEvents();
			if (prog->wasCanceled())
				return;
		}


		counter++;

		double distanceSum = 0.0;
		int count = 0;

		dijkstra(&*it);

		for (RMesh::Vertex_iterator n = vertices_begin(); n != vertices_end(); ++n)
		{
			if (n->distance() != FLT_MAX) 
			{
				count++;
				distanceSum += n->distance();
			}
		}
		it->centricity(distanceSum/(double)count);

		//	if (it->centricity() > maxCentricity) maxCentricity = it->centricity();
		//	if (it->centricity() < minCentricity) minCentricity = it->centricity();
		//	printf("%d = %f\n", counter, it->centricity());
	}

	delete prog;


	//if (m_normalize ) 
	//{
	//report("normalizing values");
	/*for (Mesh::Vertex_iterator it = m_mesh->vertices_begin();
	it != m_mesh->vertices_end(); 
	it++)
	{
	if (it->centricity() != FLT_MAX)
	it->centricity() = (it->centricity() - minCentricity) / (maxCentricity - minCentricity);
	}*/
	//normalizeByComponents();
	//}

	// facet centricity is the average of the vertices.

	float featureMin = FLT_MAX;
	float featureMax = -FLT_MAX;

	for (RMesh::Facet_iterator fit = facets_begin(); fit != facets_end(); ++fit)
	{
		Facet_handle fh = &*fit;
		float val = 0.0;
		for(int fii = 0; fii < fh->size(); ++fii)
		{
			Vertex_handle cv = fh->vertex(fii);
			val += cv->centricity();
		}
		val /= fh->size();

		fh->centricity() = val;

		if (val < featureMin) 
			featureMin = val;
		if (val > featureMax)
			featureMax = val;
	}

	float divwith = 1.0 / (featureMax - featureMin);
	for (RMesh::Facet_iterator fit = facets_begin(); fit != facets_end(); ++fit)
	{
		Facet_handle fh = &*fit;
		fh->centricity() = (fh->centricity() - featureMin) * divwith;
	}

	m_hasCentricity = true;
}


void RMesh::normalizeByComponents()
{
	vertexSet_t allVertices;	

	for (RMesh::Vertex_iterator vit = vertices_begin(); vit != vertices_end(); vit++) {
		vit->tag(0);
		allVertices.insert(vit);
	}	

	do {
		Mesh::Vertex_handle v = *allVertices.begin();
		vertexSet_t connectedGroup;
		int groupSize = growFrom(v, connectedGroup);

		float featureMin = FLT_MAX;
		float featureMax = -FLT_MAX;

		for (vertexSet_t::iterator it = connectedGroup.begin(); it != connectedGroup.end(); it++)
		{
			allVertices.erase(*it);

			float val;
			if (m_feature == FEATURE_VOLUME) 
				val = (*it)->volume();
			else
				val = (*it)->centricity();

			if (val < featureMin) 
				featureMin = val;
			if (val > featureMax)
				featureMax = val;
		}

		for (vertexSet_t::iterator it = connectedGroup.begin(); it != connectedGroup.end(); it++)
		{
			if (m_feature == FEATURE_VOLUME) 
				(*it)->volume() = ((*it)->volume() - featureMin) / (featureMax - featureMin);
			else
				(*it)->centricity() = ((*it)->centricity() - featureMin) / (featureMax - featureMin);
		}
	} while (allVertices.size());
}
#endif

