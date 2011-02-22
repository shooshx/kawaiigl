#include <QFileInfo>

#include <QTextStream>

#include "Mesh.h"
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

void MeshBuilder::addFace(Face& f)
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
		Mesh::Face& addf = m_rmesh->addTriangle(f.v[0], f.v[1], f.v[2]);
		if (m_rmesh->hasEachProp(Prop_TexCoord))
		{
			if (f.vt.size() == 3 && m_rmesh->hasEachProp(Prop_TexCoord))
				addf.setEachProp<Vec2>(Prop_TexCoord, f.vt[0], f.vt[1], f.vt[2]);
		}
	}
	else
	{ // don't support tex coords 
		m_rmesh->addFaceTriangulate(f.v);

	}
}

typedef MeshBuilder::Face Face;



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

	QMap<QString, int> materials;
	int mtlCount = 0, curMtl = -1;
	QString mtlfileName;

	int addedV = 0, addedVt = 0;
	QString line;
	do {
		line = in.readLine();
		if (line.length() == 0)
			continue;
		QStringList flds = line.split(' ', QString::SkipEmptyParts);
		if (flds[0] == "v" && flds.size() >= 4)
		{
			m_build->addVtx(flds[1].toFloat(), flds[2].toFloat(), flds[3].toFloat());
			++addedV;
		}

		else if (flds[0] == "vt" && flds.size() >= 3)
		{
			if (!seenTex)
			{
				m_build->createFaceEach<Vec2>(Prop_TexCoord);
				seenTex = true;
			}
			m_build->addEachData<Vec2>(Prop_TexCoord, Vec2(flds[1].toFloat(), flds[2].toFloat()));
			++addedVt;
		}
		else if (flds[0] == "f" && flds.size() >= 4)
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
			if (curMtl != -1)
				m_build->mesh()->addFaceData(Prop_Group, curMtl);
		}
		else if (flds[0] == "mtllib" && flds.size() >= 2)
		{ // going to have materials
			if (false) // disable materials for now , problem with AA_6_Star.obj
			{
				m_build->mesh()->createFaceProperty<int>(Prop_Group);
				mtlfileName = flds[1];
				curMtl = 0;
			}
		}
		else if (flds[0] == "usemtl" && flds.size() >= 2)
		{
			if (!mtlfileName.isEmpty())
			{
				QString mtl = flds[1];
				if (!materials.contains(mtl))
				{
					materials[mtl] = mtlCount;
					curMtl = mtlCount;
					mtlCount++;
				}
				else
				{
					curMtl = materials[mtl];
				}
			}
		}

	} while (!line.isNull()); 

	m_build->endSurface();
	Mesh& mesh = *m_build->mesh();
	printf("Loaded mesh %d vertices, %d faces, %d materials\n", mesh.numVtx(), mesh.numFaces(), mtlCount);

	if (mtlCount > 0)
	{
		QVector<Mesh::Material> &mmtl = m_build->mesh()->mtl();
		mmtl.resize(materials.size());
		foreach(const QString& name, materials.keys())
		{
			mmtl[materials[name]] = Mesh::Material(name);
		}

		// parse the material file
		mtlfileName = QFileInfo(filename).absolutePath()+ "/" + mtlfileName;
		QFile mtlfile(mtlfileName);
		if (mtlfile.open(QIODevice::ReadOnly))
		{
			QTextStream tin(&mtlfile);
			int curmtl = -1;
			do {
				line = tin.readLine();
				if (line.length() == 0)
					continue;
				QStringList flds = line.split(' ', QString::SkipEmptyParts);
				if (flds[0] == "newmtl" && flds.size() >= 2)
				{
					curmtl = -1;
					if (materials.contains(flds[1]))
					{
						curmtl = materials[flds[1]];
					}
				}
				else if (curmtl != -1)
				{
					if (flds[0] == "Kd" && flds.size() >= 4) 
					{
						mmtl[curmtl].diffuseCol = Vec3(flds[1].toFloat(), flds[2].toFloat(), flds[3].toFloat());
					}
				}
			} while (!tin.atEnd()); 
		}
		else
		{
			printf("unable to open materials file %s\n", mtlfileName.toAscii().data());
		}


	}

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
	for(Mesh::Vertex_const_iterator it = m_rmesh->vertices_begin(); it != m_rmesh->vertices_end(); ++it)
	{
		Mesh::Vertex_const_handle v = &(*it);
		out << "v " << v->point().x << " " << v->point().y << " " << v->point().z << "\n";
	}
}
void ObjWriter::writeFaces(QTextStream& out)
{
	for(Mesh::Face_const_iterator it = m_rmesh->faces_begin(); it != m_rmesh->faces_end(); ++it)
	{
		Mesh::Face_const_handle f = &(*it);
		out << "f ";
		for(int i = 0; i < f->size(); ++i)
		{
			out << f->vertexIndex(i) + 1 << " ";
		}
		out << "\n";
	}
}

void JsonWriter::write(QTextStream& out)
{
	if (m_mesh->numVtx() == 0 || m_mesh->numFaces() == 0)
		return;

	Mesh::Vertex_const_handle v = m_mesh->find_vertex(0);
	out << "{\n \"vertexPositions\":[" << v->point().x << "," << v->point().y << "," << v->point().z;
	for(int i = 1; i < m_mesh->numVtx(); ++i)
	{
		Mesh::Vertex_const_handle v = m_mesh->find_vertex(i);
		out << "," << v->point().x << "," << v->point().y << "," << v->point().z;
	}
	out << "],\n \"vertexNormals\":[" << v->normal().x << "," << v->normal().y << "," << v->normal().z;
	for(int i = 1; i < m_mesh->numVtx(); ++i)
	{
		Mesh::Vertex_const_handle v = m_mesh->find_vertex(i);
		out << "," << v->normal().x << "," << v->normal().y << "," << v->normal().z;
	}
	out << "],\n";
	
	if (m_mesh->hasEachProp(Prop_Group))
	{
		int numGrp = 1;
		QString indent;
		if (!m_mesh->mtl().isEmpty())
		{
			indent = " ";
			numGrp = m_mesh->mtl().size();
			out << " \"groups\":{\n";
		}
		for(int gi = 0; gi < numGrp; ++gi)
		{
			out << indent << " \"g" << gi << "\":{\n";
			out << indent << "  \"triangles\":[";
			bool first = true;
			for(int fi = 0; fi < m_mesh->numFaces(); ++fi)
			{
				Mesh::Face_const_handle f = m_mesh->find_facet(fi);
				if (f->prop<int>(Prop_Group) != gi)
					continue;
				if (!first)
					out << ",";
				first = false;
				out << f->vertexIndex(0) << "," << f->vertexIndex(1) << "," << f->vertexIndex(2);
			}
			out << "],\n";
			const Mesh::Material& mtl = m_mesh->mtl()[gi];
			out << indent << "  \"diffuseColor\":[" << mtl.diffuseCol.r << "," << mtl.diffuseCol.g << "," << mtl.diffuseCol.b << "]\n";
			out << indent << " }"; // closing the "g"
			if (gi < numGrp-1)
				out << ",";
			out << "\n";
		}
		out << " }\n"; // closing "groups"
		out << "}\n"; // closing the json
	}
	else
	{
		out << "  \"triangles\":[";
		bool first = true;
		for(int fi = 0; fi < m_mesh->numFaces(); ++fi)
		{
			Mesh::Face_const_handle f = m_mesh->find_facet(fi);
			if (!first)
				out << ",";
			first = false;
			out << f->vertexIndex(0) << "," << f->vertexIndex(1) << "," << f->vertexIndex(2);
		}
		out << "]\n";
		out << "}\n";
	}

}




bool MeshIO::write_Obj(const QString& filename, Mesh* rmesh)
{
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	QTextStream out(&file);
	ObjWriter writer(rmesh);
	writer.writeVertices(out);
	writer.writeFaces(out);
	return true;
}


bool MeshIO::read_OffPly2(const QString& filename, Mesh* rmesh)
{
	MeshBuilder b(rmesh);
	Ply2Reader r(&b);
	return r.read(filename);
}
bool MeshIO::read_Obj(const QString& filename, Mesh* rmesh)
{
	MeshBuilder b(rmesh);
	ObjReader r(&b);
	return r.read(filename);
}


bool MeshIO::read_Ext(const QString& filename, Mesh* rmesh)
{
	QFileInfo fii(filename);
	QString extension(fii.suffix().toLower());
	if (extension == "obj")
		return read_Obj(filename, rmesh);
	else if (extension == "gsd")
	{
		MeshBuilder b(rmesh);
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