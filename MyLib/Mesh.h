#ifndef RMESH_H__INCLUDED
#define RMESH_H__INCLUDED

#include <QList>
#include <QVector>
#include <QMap>

#include <math.h>
#include <float.h>

#include "Vec.h"
//#include "KDTreeWrap.h"

enum EPropName
{
	Prop_TexCoord = 0,
	Prop_Tangent = 1,
	Prop_BiTangent = 2,
	Prop_Group = 3
};

class TrMatrix;

class Mesh
{
public:
	Mesh(const QString& name) 
		: m_totalSurface(0.0f), m_diagLength(0.0f),
		  m_minFaceVolume(FLT_MAX), m_maxFaceVolume(-FLT_MAX), m_cachedMinMaxFaceVolume(false), m_name(name)
		  ,m_externalVtxNormals(false), m_externalFaceNormals(false)
	{}
	Mesh(const Mesh* vtxFrom, const QString& name) 
		: m_totalSurface(0.0f), m_diagLength(0.0f),
		m_minFaceVolume(FLT_MAX), m_maxFaceVolume(-FLT_MAX), m_cachedMinMaxFaceVolume(false), m_name(name)
		,m_externalVtxNormals(false), m_externalFaceNormals(false)
	{
		// need to copy the vertices because they point back to the mesh
		foreach(const Vertex& v, vtxFrom->m_vtx)
			addVertex(v.point());
	}
	~Mesh() {}

	class Face;
	class Vertex;
	class Edge;
	class ClosenessEdge;

	typedef const Face* Face_const_handle;
	typedef Face* Face_handle;
	typedef const Face* Face_const_handle;
	typedef Face* Face_handle;
	typedef const Vertex* Vertex_const_handle;
	typedef Vertex* Vertex_handle;
	typedef const Edge* Edge_const_handle;
	typedef Edge* Edge_handle;
	typedef const ClosenessEdge* CloseEdge_const_handle;
	typedef ClosenessEdge* CloseEdge_handle;

	typedef QList<Vertex> TVtxList;
	typedef QList<Vertex_handle> TVtxHList;
	typedef QList<Face> TFaceList;
	typedef QVector<int> TIndexList;
	typedef QVector<Edge> TEdgeList;
	typedef QVector<ClosenessEdge> TClosenessEdgeList;

	typedef TVtxList::const_iterator Vertex_const_iterator;
	typedef TVtxList::iterator Vertex_iterator;
	typedef TFaceList::const_iterator Face_const_iterator;
	typedef TFaceList::iterator Face_iterator;
	typedef TEdgeList::const_iterator Edge_const_iterator;
	typedef TEdgeList::iterator Edge_iterator;

	//typedef QVector<RayIntersectCell*> TCellList;
	class PropList 
	{
	public:
		virtual ~PropList() {}
		virtual void addNullElement() = 0;
	};

	// give a value to every vertex or every face.
	template<typename T>
	class PropListT : public PropList
	{
	public:
		PropListT(int size) : list(size) {}
		virtual ~PropListT() {}
		virtual void addNullElement() { list.append(T()); }

		QVector<T> list;
	};

	// give a value to each of the 3 points of every face separatly. 
	template<typename T>
	class PropEachListT : public PropList
	{
	public:
		PropEachListT(int numFaces, int numVals) : faceEach(numFaces * 3), values(numVals) {}
		virtual void addNullElement() 
		{ 
			faceEach.append(-1); faceEach.append(-1); faceEach.append(-1); 
		}

		QVector<int> faceEach; // groups of 3 indexes into the value.
		QVector<T> values;
	};


	class Vertex
	{
	public:
		Vertex() 
			: m_mymesh(NULL), m_index(-1), m_tran(-1)
		{}
		Vertex(Mesh *mesh, const Vec3& p, int index) 
			: m_mymesh(mesh), m_p(p), m_index(index), m_tran(-1)
//			  m_nvolume(FLT_MAX), m_volume(FLT_MAX),
//			  m_centricity(FLT_MAX), m_distance(FLT_MAX)
		{}

		int& index() { return m_index; }
		Vec3& point() { return m_p; }
		Vec3& normal() { return m_normal; }

		int index() const { return m_index; }
		const Vec3& point() const { return m_p; }
		const Vec3& normal() const { return m_normal; }

		// property of vertex
		template<typename T>
		const T& prop(EPropName id) const 
		{ 
			return ((PropListT<T>*)m_mymesh->vtxProps[id])->list[m_index];
		}
		template<typename T>
		T& prop(EPropName id)
		{ 
			return ((PropListT<T>*)m_mymesh->vtxProps[id])->list[m_index];
		}


		int& tran() { return m_tran; }
		int tran() const { return m_tran; }


		TIndexList& faceIndexs() { return m_faces; }
		int numFaces() const { return m_faces.size(); }
		//int numEdges() const { return m_edges.size(); }

		Face_handle face(int i)
		{
			Q_ASSERT(i < m_faces.size());
			return m_mymesh->find_facet(m_faces[i]);
		}
		Face_const_handle face(int i) const 
		{ return const_cast<Vertex*>(this)->face(i); }
		int faceIndex(int i) const
		{
			Q_ASSERT(i < m_faces.size());
			return m_faces[i];
		}

	/*	Edge_const_handle edge(int i) const
		{
			Q_ASSERT(i < m_edges.size());
			return m_mymesh->find_edge(m_edges[i]);
		}*/

		void computeNormal();
		TIndexList neiVertices() const;
		TVtxHList neiVerticesH() const;

		// assuming vertices of the same mesh.
		bool operator<(const Vertex& v) {
			return index() < v.index();
		}


	private:
		Vec3 m_p; // oh palllese.

		Mesh *m_mymesh;
		Vec3 m_normal;
		int m_index;

	//	float m_nvolume, m_volume, m_centricity;
	//	float m_distance;

		TIndexList m_faces; // the indexes of faces this Vertex is part of.
		//TIndexList m_edges; // the indexes of *edges* this Vertes is part of

		int m_tran; // transfer to.. while removing

		friend class Mesh;
	};

	class Face
	{
	public:
		Face(Mesh *rmesh = NULL, int p0 = -1, int p1 = -1, int p2 = -1, int index = -1) 
			: m_index(index), m_mymesh(rmesh),
  			  m_surface(0.0f), m_tran(-1)
//			  m_nvolume(FLT_MAX), m_volume(FLT_MAX), m_dispnvolume(FLT_MAX),
//			  m_cluster(0), m_part(0), m_centricity(0.0f)
		{ 
			m_pi[0] = p0, m_pi[1] = p1; m_pi[2] = p2;
			m_edges[0] = -1; m_edges[1] = -1; m_edges[2] = -1;
		}


		int size() const { return 3; }

		int& index() { return m_index; }
		int index() const { return m_index; }

		Vertex_handle vertex(int i)
		{ 
			Q_ASSERT(i < size());
			Q_ASSERT(m_pi[i] != -1);
			return m_mymesh->find_vertex(m_pi[i]); 
		}
		Vertex_const_handle vertex(int i) const
		{ return const_cast<Face*>(this)->vertex(i); }

		Vec3& point(int i)
		{
			Q_ASSERT(i < size());
			Q_ASSERT(m_pi[i] != -1);
			return m_mymesh->find_vertex(m_pi[i])->point(); 
		}
		const Vec3& point(int i) const
		{ return const_cast<Face*>(this)->vertex(i)->point(); }

		int vertexIndex(int i) const
		{
			Q_ASSERT(i < size());
			return m_pi[i];
		}

		Edge_const_handle edge(int i) const
		{
			Q_ASSERT(i < size());
			Q_ASSERT(m_edges[i] != -1);
			return m_mymesh->find_edge(m_edges[i]);
		}

		Vec3& normal() { return m_normal; }
		const Vec3& normal() const { return m_normal; }

/*
		template<typename T>
		void addEachProp(int id, int i0, int i1, int i2)
		{
			PropEachListT<T>* idcs = ((PropEachListT<T>*)m_mymesh->faceEachProps[id]);
			idcs->faceEach.append(i0);
			idcs->faceEach.append(i1);
			idcs->faceEach.append(i2);
		}*/


		template<typename T>
		void setEachProp(EPropName id, int i0, int i1, int i2)
		{
			PropEachListT<T>* idcs = ((PropEachListT<T>*)m_mymesh->faceEachProps[id]);
			idcs->faceEach[m_index * 3] = i0;
			idcs->faceEach[m_index * 3 + 1] = i1;
			idcs->faceEach[m_index * 3 + 2] = i2;
		}

		template<typename T>
		T propEach(EPropName id, int i) const
		{
			PropEachListT<T>* idcs = ((PropEachListT<T>*)m_mymesh->faceEachProps[id]);
			int datai = idcs->faceEach[m_index * 3 + i];
			if (datai == -1)
				return T();
			return idcs->values[datai];
		}

		// property of face
		template<typename T>
		const T& prop(EPropName id) const 
		{ 
			return ((PropListT<T>*)m_mymesh->faceProps[id])->list[m_index];
		}
		template<typename T>
		T& prop(int Prop_Group)
		{ 
			return ((PropListT<T>*)m_mymesh->faceProps[id])->list[m_index];
		}

		float& surface() { return m_surface; }
		float surface() const { return m_surface; }
		void surface(float v) { m_surface = v; }

		void computeNormal();
		void reverseOrder()
		{
			qSwap(m_pi[0], m_pi[2]);
		}


		const Vec3& center() const { return m_center; }

		// return face indexes of adjucent faces
		TIndexList neiFaces() const; 
		bool isNeiFace(int findex) const;

		int containsVtx(int vi) const
		{
			for(int i = 0; i < 3; ++i)
				if (m_pi[i] == vi)
					return i;
			return -1;
		}

		//const TCellList& cellList() const { return m_gridCells; }
	private:
		// zero based index to the vertex list
		int m_pi[3];
		int m_index;
		Mesh *m_mymesh;
		int m_edges[3]; // indexes of the edges

		Vec3 m_normal;



		float m_surface;
		Vec3 m_center;

		int m_tran; // transfer to.. white removing

		//TIndexList m_closeFaces;
		//TCellList m_gridCells;

		friend class Mesh;
		//friend class RayIntersect;
	};

	// undirected edge. the lowest index is always first.
	class Edge
	{
	public:
		Edge(int a = -1, int b = -1, int index = -1, Mesh *rmesh = NULL) 
			: m_a(a), m_b(b), m_index(index), m_length(0.0f),
			  m_mymesh(rmesh)
		{}

		int index() const { return m_index; }
		float length() const { return m_length; }
		const Vec3& center() const { return m_center; }
		int a() const { return m_a; }
		int b() const { return m_b; }
		int smalli() const { return m_a; }
		int largei() const { return m_b; }
		Vertex_handle vertexA() { return m_mymesh->find_vertex(m_a); }
		Vertex_const_handle vertexA() const { return m_mymesh->find_vertex(m_a); }
		Vertex_handle vertexB() { return m_mymesh->find_vertex(m_b); }
		Vertex_const_handle vertexB() const { return m_mymesh->find_vertex(m_b); }

		int numFaces() const{ return m_faces.size(); }
		Face_handle face(int i)
		{
			Q_ASSERT(i < m_faces.size());
			return m_mymesh->find_facet(m_faces[i]);
		}
		Face_const_handle face(int i) const 
		{ return const_cast<Edge*>(this)->face(i); }

		int faceIndex(int i) const
		{
			Q_ASSERT(i < m_faces.size());
			return m_faces[i];
		}

	private:
		int m_index; // of this in the edge array
		int m_a; // lower index in vertex array
		int m_b; // higher index in vertex array

		float m_length;
		Vec3 m_center;
		Mesh *m_mymesh;

		// faces that are has this edge
		TIndexList m_faces;

		friend class Mesh;
	};

	class ClosenessEdge
	{
	public:
		ClosenessEdge(int f1 = -1, int f2 = -1) : m_f1i(f1), m_f2i(f2) {}
		int f1index() const { return m_f1i; }
		int f2index() const { return m_f2i; }
	private:
		int m_f1i, m_f2i;
	};


	TVtxList& vertices() { return m_vtx; }
	TFaceList& faces() { return m_face; }

	Vertex& addVertex(const Vec3& p)
	{
		m_vtx.push_back(Vertex(this, p, numVtx()));
		return m_vtx.back();
	}

	void removeVertex(int index);
	void commitRemove();


	Face& addTriangle(int p1, int p2, int p3)
	{
		Q_ASSERT(p1 >= 0 && p1 < m_vtx.size());
		Q_ASSERT(p2 >= 0 && p2 < m_vtx.size());
		Q_ASSERT(p3 >= 0 && p3 < m_vtx.size());

		foreach(PropList* p, faceEachProps)
			if (p != NULL)
				p->addNullElement();

		m_face.push_back(Face(this, p1, p2, p3, m_face.size()));
		return m_face.back();
	}
	int addFaceTriangulate(Mesh::TIndexList& f);

	int numVtx() const { return m_vtx.size(); }
	int numFaces() const { return m_face.size(); }
	int numEdges() const { return m_edge.size(); }
	int numCloseEdge() const { return m_closeEdge.size(); }

	int size_of_facets() const { return m_face.size(); }
	int size_of_vertices() const { return m_vtx.size(); }
	int size_of_edges() const { return m_edge.size();; }

	Vertex_const_iterator vertices_begin() const { return m_vtx.constBegin(); }
	Vertex_const_iterator vertices_end() const { return m_vtx.constEnd(); }
	Vertex_iterator vertices_begin() { return m_vtx.begin(); }
	Vertex_iterator vertices_end() { return m_vtx.end(); }

	Face_const_iterator faces_begin() const { return m_face.constBegin(); }
	Face_const_iterator faces_end() const { return m_face.constEnd(); }
	Face_iterator faces_begin() { return m_face.begin(); }
	Face_iterator faces_end() { return m_face.end(); }

	Edge_const_iterator edges_begin() const { return m_edge.constBegin(); }
	Edge_const_iterator edges_end() const { return m_edge.constEnd(); }
	Edge_iterator edges_begin() { return m_edge.begin(); }
	Edge_iterator edges_end() { return m_edge.end(); }

	
	Face_handle find_facet(int index) 
	{ 
		Q_ASSERT(index < m_face.size());
		return &m_face[index]; 
	}
	Face_const_handle find_facet(int index) const 
	{ 
		Q_ASSERT(index < m_face.size());
		return &m_face[index]; 
	}
	Vertex_handle find_vertex(int index) 
	{ 
		Q_ASSERT(index < m_vtx.size());
		return &m_vtx[index]; 
	}
	Vertex_const_handle find_vertex(int index) const
	{ 
		Q_ASSERT(index < m_vtx.size());
		return &m_vtx[index]; 
	}
	Edge_handle find_edge(int index) 
	{ 
		Q_ASSERT(index < m_edge.size());
		return &m_edge[index]; 
	}

	// this doesn't require for edges to be built.
	// find two faces that share two vertices (an edge)
	// return true if there it has exactly two
	bool find_faces_with(int vi1, int vi2, TIndexList& res) const;

	CloseEdge_const_handle find_closeEdge(int index)
	{
		Q_ASSERT(index < m_closeEdge.size());
		return &m_closeEdge[index]; 
	}


	const Vec3& getFacetCenter(Face_const_handle f) const
	{ return f->center(); }

	Vec3 centerOfMass();
	Vec3 computedCenterOfMass() {  return m_computedCenterOfMass; } 
	float diagonalLength() const { return m_diagLength; }
	const Vec3& minMaxCenter() const { return m_minMaxCenter; }

	const Vec3& minp() const { return m_min; } 
	const Vec3& maxp() const { return m_max; } 

	// for each face according to its vertices orientation
	void compute_normals_per_facet();
	// for each vertex according to each faces normals
	void compute_normals_per_vertex();
	// for each face, according to the neighboring vertices
	void average_normals_per_facet();

	void buildVerticesInFaces();
	float compute_triangle_surfaces();
	void compute_bounding_box();

	void finalize(bool needEdges);

	void reverseNormals();
	void reverseFaces();


	float xmin() const { return m_min.x; }
	float ymin() const { return m_min.y; }
	float zmin() const { return m_min.z; }
	float xmax() const { return m_max.x; }
	float ymax() const { return m_max.y; }
	float zmax() const { return m_max.z; }


/*
	void compute_volume_range_on_facets(float& minVal, float& maxVal)
	{
		if (m_cachedMinMaxFaceVolume)
		{
			minVal = m_minFaceVolume;
			maxVal = m_maxFaceVolume;
			return;
		}
		compute_feature_range_on_facets(minVal, maxVal, &Mesh::Face::volume);

		m_minFaceVolume = minFaceFeature;
		m_maxFaceVolume = maxFaceFeature;
		m_cachedMinMaxFaceVolume = true;

	}
*/
	void compute_feature_range_on_facets(float& minVal, float& maxVal, float (Mesh::Face::*datgetter)() const);

	Vec3 polyCenterOfMass() const;
	const Vec3& diagLength() const { return m_axisLength; }

	void buildEdges();

	template<class T>
	void setForAll(T& (Mesh::Face::*method)(), T value)
	{
		for(int i = 0; i < numFaces(); ++i)
		{
			Face_handle f = find_facet(i);
			(f->*method)() = value;
		}
	}

	template<class T>
	void setForAll(T& (Mesh::Vertex::*method)(), T value)
	{
		for(int i = 0; i < numVtx(); ++i)
		{
			Face_handle f = find_vertex(i);
			(f->*method)() = value;
		}
	}

	void makeClosenessEdges();

	// saves as OFF the submesh defined by the faces indexes
	void saveSubMesh(const QString& filename, const TIndexList& facetsList) const;

	// save in a text file the `srcgetter` values of the faces defined by the faces indexex
	void saveSubMeshFaceValuesTxt(const QString& filename, const TIndexList& facesList, float (Mesh::Face::*srcgetter)() const) const;

	void computeCentricity(QWidget *guiParent);
	void dijkstra(Mesh::Vertex_handle v);

	//bool hasCentricity() const { return false; }

	void translateCenter()
	{
		Vec3 c = (m_min + m_max) / 2.0;
		translateCenter(c);
	}
	void translateCenter(const Vec3& c);
	void rescaleAndCenter(float destdialen);

	void transformVertices(const TrMatrix& tr);

/*
	void calcCurvature(int ringSIze);
	Curvature calcCurvatureFor(int vtxIndex); // meh..
	void calcDotCurve();
	void initUseCurve()
	{
		m_vtxUseCurve.resize(numVtx());
	}

	bool hasCurvature() const { return m_vtxCurvature.size() == m_vtx.size(); }
	bool hasDotCurv() const { return m_vtxDotCurve.size() == m_vtx.size(); }
	bool hasUseCurv() const { return m_vtxUseCurve.size() == m_vtx.size(); }

*/
	float min_edge_length_around(Vertex_handle pVertex);

	void buildKdTree();
	// this is only approximate because its the distance to the points, not the faces.
	float distFromMesh(const Vec3& v);
	const QString& name() const { return m_name; }

	template<typename T>
	void createVtxProperty(EPropName id, int size = -1)
	{
		while (id >= vtxProps.size())
			vtxProps.append(NULL);
		if (size == -1 || size == 0)
			size = m_vtx.size();
		vtxProps[id] = new PropListT<T>(size);
	}
	template<typename T>
	void createFaceProperty(EPropName id, int size = -1)
	{
		while (id >= faceProps.size())
			faceProps.append(NULL);
		if (size == -1 || size == 0)
			size = m_face.size();
		faceProps[id] = new PropListT<T>(size);
	}

	template<typename T>
	void createFaceEachProperty(EPropName id, int numValues)
	{
		while (id >= faceEachProps.size())
			faceEachProps.append(NULL);
		faceEachProps[id] = new PropEachListT<T>(m_face.size(), numValues);
	}

	template<typename T>
	void addEachData(EPropName id, const T& val)
	{
		((PropEachListT<T>*)faceEachProps[id])->values.append(val);
	}
	template<typename T>
	void addFaceData(EPropName id, const T& val)
	{
		((PropListT<T>*)faceProps[id])->list.append(val);
	}

	bool hasVtxProp(EPropName id) const
	{
		return (vtxProps.size() > id && vtxProps[id] != NULL);
	}
	bool hasFaceProp(EPropName id) const
	{
		return (vtxProps.size() > id && vtxProps[id] != NULL);
	}
	bool hasEachProp(EPropName id) const
	{
		return (faceEachProps.size() > id && faceEachProps[id] != NULL);
	}

	void setHasExternal(bool vtxNormal, bool faceNormal)
	{
		m_externalVtxNormals = vtxNormal;
		m_externalFaceNormals = faceNormal;
	}

	//ANNKDTreeWrap m_kdtree;

	class Material
	{
	public:
		Material(const QString& nm = "") : name(nm) {}
		QString name;
		Vec3 diffuseCol;
	};
	QVector<Material>& mtl() { return m_mtl; }
	const QVector<Material>& mtl() const { return m_mtl; }

private:
	Mesh(const Mesh&);
	Mesh& operator=(const Mesh&);

	typedef QMap<int, int> TLargeEdgeI;
	typedef QMap<int, TLargeEdgeI> TSmallLargeEdgeI;

	int addEdge(int a, int b);
	int addCloseEdge(int a, int b);

	int addEdgeOnceUsing(int s, int l, TSmallLargeEdgeI &bld, int (Mesh::*addMethod)(int a, int b));

	bool areFacesClose(Face_handle a, Face_handle b, float thresh);
	
	QString m_name;
	TVtxList m_vtx;
	TFaceList m_face;
	TEdgeList m_edge;
	TClosenessEdgeList m_closeEdge;

	Vec3 m_computedCenterOfMass;
	Vec3 m_minMaxCenter;
	Vec3 m_min, m_max;
	Vec3 m_axisLength;
	float m_diagLength;
	float m_totalSurface;

	bool m_externalVtxNormals, m_externalFaceNormals;

	float m_minFaceVolume, m_maxFaceVolume;
	bool m_cachedMinMaxFaceVolume;


	QVector<PropList*> vtxProps;
	QVector<PropList*> faceProps;
	QVector<PropList*> faceEachProps; // a property for every vertex in a polygon
	// not paying for something I don't want
	// vertex properties

	QVector<Material> m_mtl;
};

class MeshDist : public FuncFloatVec
{
public:
	MeshDist(Mesh* mesh) : m_rmesh(mesh) {}
	virtual float operator()(const Vec3& v) const
	{
		return m_rmesh->distFromMesh(v);
	}
	virtual bool init()
	{
		m_rmesh->buildKdTree();
		return true;
	}

private:
	Mesh* m_rmesh;

};


#endif
