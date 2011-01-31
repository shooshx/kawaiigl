#pragma once
#ifndef MESHIO_H_INCLUDED
#define MESHIO_H_INCLUDED

#include "Vec.h"
#include "Mesh.h"

class QString;
class MeshBuilder;


class FileReader
{
public:
	FileReader(MeshBuilder* bld) :m_build(bld) {}
	virtual bool read(const QString& filename) = 0;

protected:
	MeshBuilder* m_build;
};


class MeshBuilder 
{
public:
	MeshBuilder(Mesh* rmesh) : m_rmesh(rmesh), m_countOutFaces(0)
		, m_vtxIndex(0), m_faceIndex(0)
	{}

	virtual void setVtxStart(int startIndex) 
	{ 
		m_vtxIndex = startIndex; 
	}

	struct Face
	{
		Face(int a, int b, int c)
		{
			add(a);
			add(b);
			add(c);
		}
		Face(int *from = NULL, int sz = 0)
		{
			v.resize(sz);
			for (int i = 0; i < sz; ++i)
				v[i] = from[i];
		}

		void add(int i) {  v.append(i); }
		void addTex(int i) { vt.append(i); }

		void reset() {  v.clear(); }
		bool isValid() const { return valid; }
		void setValid(bool b)  { valid = b; }
		int size() const { return v.size(); } 

		bool valid;
		TIndexList v;
		TIndexList vt;

	};

	virtual void startSurface(int v, int f, int e) 
	{
		m_vtxSz = v;
		m_facesSz = f;
		//BufFacesMeshBuilder::startSurface(v, f, e);
	}

	virtual int addVtx(float x, float y, float z)
	{
		Mesh::Vertex &v = m_rmesh->addVertex(Vec3(x, y, z));
		v.index() = m_vtxIndex++;
		return v.index();
	}
	virtual void setNormal(int iv, float x, float y, float z)
	{
		Mesh::Vertex_handle v = m_rmesh->find_vertex(iv);
		v->normal() = Vec3(x, y, z);
	}
	virtual void setVtxProp(EPropName p, int iv, float x, float y, float z)
	{
		Mesh::Vertex_handle v = m_rmesh->find_vertex(iv);
		v->prop<Vec3>(p) = Vec3(x, y, z);
	}
	virtual void setVtxProp(EPropName p, int iv, float x, float y)
	{
		Mesh::Vertex_handle v = m_rmesh->find_vertex(iv);
		v->prop<Vec2>(p) = Vec2(x, y);
	}

	template<typename T>
	void addEachData(EPropName id, const T& val)
	{
		m_rmesh->addEachData(id, val);
	}


	void setHas(bool hasVtxNormals, bool hasFaceNormals)
	{
	//	m_rmesh->setHasExternal(hasVtxNormals, hasFaceNormals);
	}
	template<typename T>
	void createVtxProp(EPropName id)
	{
		m_rmesh->createVtxProperty<T>(id, m_vtxSz);
	}
	template<typename T>
	void createFaceEach(EPropName id)
	{
		m_rmesh->createFaceEachProperty<T>(id, 0);
	}

	virtual void addFace(Face& f);

	virtual void endSurface() 
	{
		if (m_countOutFaces > 0)
		{
			printf("MeshBuilder: %d of %d faces contained invalid vertices\n", m_countOutFaces, m_countOutFaces + m_rmesh->numFaces());
		}
	}

	Mesh *mesh() { return m_rmesh; }

protected:
	Mesh *m_rmesh;
	int m_vtxSz, m_facesSz;

	int m_countOutFaces;
	bool m_needEachTex; // should add texture coordinates for all faces (its all or nothing)

protected:
	int m_vtxIndex;
	int m_faceIndex;

};



class Ply2Reader : public FileReader
{
public:
	Ply2Reader(MeshBuilder *bld) : FileReader(bld) {}
	virtual ~Ply2Reader() {}
	virtual bool read(const QString& filename);

private:
	bool read(QTextStream& file, bool readFacetSize, bool readEdgesNum, bool readNumVertices);

	int m_numVertices;
	int m_numTriangles;
};

class ObjReader : public FileReader
{
public:
	ObjReader(MeshBuilder *bld) : FileReader(bld) {}
	virtual ~ObjReader() {}
	virtual bool read(const QString& filename);

};

// this by no means supports the full behemoth of VRML. only files used for meshes by AIM_Shape.
class VrmlReader : public FileReader
{
public:
	VrmlReader(MeshBuilder *bld) : FileReader(bld) {}
	virtual ~VrmlReader() {}
	virtual bool read(const QString& filename);

};


class GSDReader : public FileReader
{
public:
	GSDReader(MeshBuilder *bld) : FileReader(bld) {}
	virtual ~GSDReader() {}
	virtual bool read(const QString& filename);

};


class MeshIO
{
public:
	static bool read_OffPly2(const QString& filename, Mesh* rmesh);
	static bool read_Obj(const QString& filename, Mesh* rmesh);
	static bool write_Obj(const QString& filename, Mesh* rmesh);

	static bool read_Ext(const QString& filename, Mesh* rmesh); // choose by the extension
};

class ObjWriter
{
public:
	ObjWriter(Mesh* rmesh) : m_rmesh(rmesh) {}

	void writeVertices(QTextStream& out);
	void writeFaces(QTextStream& out);
private:
	Mesh *m_rmesh;
};

class JsonWriter
{
public:
	JsonWriter(Mesh* rmesh) : m_mesh(rmesh) {}
	void write(QTextStream& out);
private:
	Mesh *m_mesh;
};

#endif // MESHIO_H_INCLUDED