#include <QFileInfo>

#include <QTextStream>

#include "RMesh.h"
#include "Vec.h"

#include "MeshIO.h"



#if 0
class FaceList
{
public:
	FaceList() :m_readOff(0), m_size(0) {}
	void reserve(uint res)
	{
		m_data.reserve(res);
	}
	void add(const Face& f)
	{
		m_data.append(f.size());
		for (int i = 0; i < f.size(); ++i)
			m_data.append(f.v[i]);
		++m_size;
	}

	void resetRead() { m_readOff = 0; }

	Face next()
	{
		Face p(&m_data[m_readOff + 1], m_data[m_readOff]);
		m_readOff += p.size() + 1;
		return p;
	}

	bool hasNext()
	{
		return m_readOff < m_data.size();
	}

	int size() { return m_size; }

private:
	QVector<int> m_data;
	int m_readOff;
	int m_size;
};
#endif
//typedef QVector<Face> TFaceList;

void RMeshBuilder::addFace(Face& f)
{
	bool good = true;
	for (int fii = 0; fii < f.size(); ++fii)
	{
		if (f.v[fii] >= m_rmesh->numVtx())
			good = false;
	}
	if (!good)
	{
		++m_countOutFaces;
		return;
	}

	if (f.size() == 3)
	{
		RMesh::Face& addf = m_rmesh->addTriangle(f.v[0], f.v[1], f.v[2]);
		if (m_rmesh->hasEachProp(0))
		{
			if (f.vt.size() == 3 && m_rmesh->hasEachProp(0))
				addf.setEachProp<Vec2>(0, f.vt[0], f.vt[1], f.vt[2]);
		}
	}
	else
	{ // don't support tex coords 
		m_rmesh->addFaceTriangulate(f.v);

	}
}

typedef RMeshBuilder::Face Face;



bool Ply2Reader::read(const QString& filename)
{
	QFileInfo fii(filename);
	QString extension(fii.suffix().toLower());

	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;

	QTextStream in(&file);


	bool readFacetSize = true;
	bool readEdgesNum = false;
	bool readNumVertices = true;

	if (extension == "ply2")	 {
		readFacetSize = true;
	} else if (extension == "simple") {
		readFacetSize = false;
	} else if (extension == "off") {
		QString offhead;
		in >> offhead;
		if (offhead.toUpper() != "OFF") // for some reason the first thing in the file wasn't OFF
		{
			if (offhead != "3") // number of dimentions from qhull, should be ignored
			{
				m_numVertices = offhead.toInt();
				readNumVertices = false;
			}
		}
		readFacetSize = true;
		readEdgesNum = true;
	}
	else
	{
		printf("unknown extension %s\n", extension.toAscii().data());
	}

	return read(in, readFacetSize, readEdgesNum, readNumVertices);
}


bool Ply2Reader::read(QTextStream& file, bool readFacetSize, bool readEdgesNum, bool readNumVertices)
{  
	QString line;
	do {
		line = file.readLine();
	} while ((line.length() == 0) || (line[0] == '#'));

	QTextStream linein(&line);

	if (readNumVertices)
		linein >> m_numVertices;
	linein >> m_numTriangles;
	int edgesNum = 6;
	if (readEdgesNum)
		linein >> edgesNum;

	bool success = false;
//	int times = 0;

	m_build->startSurface(m_numVertices, m_numTriangles, edgesNum);
	m_build->setVtxStart(0);

	// read the vertices
	float x,y,z;
	for (int i = 0; i < m_numVertices; ++i) 
	{

		file >> x;
		file >> y;
		file >> z;
		m_build->addVtx(x, y, z);
	}  

	// read faces
	
	Face f;

	int fieldCount = 3;
	for (int i = 0; i < m_numTriangles; ++i) 
	{
		f.reset();
		for (int j = 0; j < fieldCount; ++j)
		{
			int v;
			file >> v;
			if ((j == 0) && readFacetSize)
				fieldCount = v + 1;
			else
				f.add(v);
		}
		m_build->addFace(f);
	}
	
	m_build->endSurface();
	success = true;

	return success;
}



bool ObjReader::read(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QTextStream in(&file);

	bool seenTex = false;
	
	m_build->startSurface(1, 1, 3); // no initial guess
	m_build->setVtxStart(0);

	int addedV = 0, addedVt = 0;
	QString line;
	do {
		line = in.readLine();
		if (line.length() == 0)
			continue;
		QStringList flds = line.split(' ', QString::SkipEmptyParts);
		if (flds[0] == "v")
		{
			m_build->addVtx(flds[1].toFloat(), flds[2].toFloat(), flds[3].toFloat());
			++addedV;
		}
		else if (flds[0] == "vt")
		{
			if (!seenTex)
			{
				m_build->createFaceEach<Vec2>(0);
				seenTex = true;
			}
			m_build->addEachData<Vec2>(0, Vec2(flds[1].toFloat(), flds[2].toFloat()));
			++addedVt;
		}
		else if (flds[0] == "f")
		{
			//int numpnts = flds.size() - 1;

			Face f;
			for(int i = 1; i < flds.size(); ++i)
			{ // cut away texture and normal information
				QStringList nums = flds[i].split('/', QString::SkipEmptyParts);
				
				int ni = nums[0].toInt();
				if (ni < 0)
					ni += addedV;
				else
					ni -= 1;
				f.add(ni); // 1 based indexes
				if (nums.size() > 1)
				{
					ni = nums[1].toInt();
					if (ni < 0)
						ni += addedVt;
					else
						ni -= 1;
					f.addTex(ni); // 1 based indexes
				}
			}

			m_build->addFace(f);
		}

	} while (!line.isNull()); 

	m_build->endSurface();

	return true;

}


bool VrmlReader::read(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
		return false;
	QTextStream in(&file);

	m_build->startSurface(1, 1, 3); // no initial guess
	m_build->setVtxStart(0);

	bool inPoints = false, inFaces = false;
	QString line;
	do {
		line = in.readLine();
		if (line.length() == 0)
			continue;
		QStringList sl = line.split(QRegExp("[\\s,]+"), QString::SkipEmptyParts);
		if (sl.contains("point"))
		{
			inPoints = true; inFaces = false;
			continue;
		}
		if (sl.contains("coordIndex"))
		{
			inPoints = false; inFaces = true;
			continue;
		}

		if (sl.size() < 3)
			continue;

		bool okA = false, okB = false, okC = false;
		if (inPoints)
		{
			float a = sl[0].toFloat(&okA);
			float b = sl[1].toFloat(&okB);
			float c = sl[2].toFloat(&okC);
			if (okA && okB && okC)
				m_build->addVtx(a, b, c);
		}
		if (inFaces)
		{
			Face f;
			for(int i = 0; i < sl.size(); ++i)
			{
				int a = sl[i].toInt(&okA);
				if (!okA) 
					break;
				if (a > 0)
					f.add(a);
			}
			if (okA)
			{
				m_build->addFace(f);
			}
		}
		
	} while (!line.isNull()); 

	m_build->endSurface();


	return true;
}


void ObjWriter::writeVertices(QTextStream& out)
{
	for(RMesh::Vertex_const_iterator it = m_rmesh->vertices_begin(); it != m_rmesh->vertices_end(); ++it)
	{
		RMesh::Vertex_const_handle v = &(*it);
		out << "v " << v->point().x << " " << v->point().y << " " << v->point().z << "\n";
	}
}
void ObjWriter::writeFaces(QTextStream& out)
{
	for(RMesh::Facet_const_iterator it = m_rmesh->facets_begin(); it != m_rmesh->facets_end(); ++it)
	{
		RMesh::Facet_const_handle f = &(*it);
		out << "f ";
		for(int i = 0; i < f->size(); ++i)
		{
			out << f->vertexIndex(i) + 1 << " ";
		}
		out << "\n";
	}
}



bool MeshIO::write_Obj(const QString& filename, RMesh* rmesh)
{
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);
	ObjWriter writer(rmesh);
	writer.writeVertices(out);
	writer.writeFaces(out);
	return true;
}


bool MeshIO::read_OffPly2(const QString& filename, RMesh* rmesh)
{
	RMeshBuilder b(rmesh);
	Ply2Reader r(&b);
	return r.read(filename);
}
bool MeshIO::read_Obj(const QString& filename, RMesh* rmesh)
{
	RMeshBuilder b(rmesh);
	ObjReader r(&b);
	return r.read(filename);
}


bool MeshIO::read_Ext(const QString& filename, RMesh* rmesh)
{
	QFileInfo fii(filename);
	QString extension(fii.suffix().toLower());
	if (extension == "obj")
		return read_Obj(filename, rmesh);
	else if (extension == "gsd")
	{
		RMeshBuilder b(rmesh);
		GSDReader r(&b);
		return r.read(filename);
	}
	else
		return read_OffPly2(filename, rmesh);
}


#include "GSDFile/GSDParser.h"

bool GSDReader::read(const QString& filename)
{
	CGSDParser p(filename.toAscii().data());
	return p.CCreateCompositeObjectFromGSD(m_build);
}