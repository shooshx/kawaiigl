/******************************************************************************
*                             gsdparser.cpp file                              *
*           This source code is property and copyrighted of TyphoonLabs       *
*                 and is released under standard LGPL license                 *
*                            see README.TXT for details                       *
*******************************************************************************/

#include "GSDParser.h"

#pragma warning (disable : 4996)

//-------------------------------------------------------------------------------------
//##ModelId=42305DF20290
CGSDParser::CGSDParser(const char *fileName)
{
  if(fileName!=NULL)
    strcpy(m_FileName,fileName);
  m_f=NULL;

  GSDHeader header;
  m_f=fopen(m_FileName,"rb");
  if(m_f==NULL) 
    return;
  fread(&header,sizeof(GSDHeader),1,m_f);  
  strcpy(m_version,header.version);
  fclose(m_f);
  
  return; 
}

//-------------------------------------------------------------------------------------
//##ModelId=42305DF20292
CGSDParser::~CGSDParser()
{  
  
}

struct GenericObjectData
{
	char Name[256];
	char ParentName[256];
	char MaterialName[256];
	unsigned int *Index;
	float *Geometry;
	float *Normals;
	float *TexCoord;
	float *Tangent;
	float *Binormal;
	unsigned long iC;
	unsigned long vC;  
};

struct DummieData
{
	char name[256];
	char parentName[256];
	float position[3];
	float rotation[4]; //quaternion x,y,z,w
	float scale[3];
};


#include "../MeshIO.h"

//--------------------------------------------------------------------------------------
//##ModelId=42305DF20294
//CBasicDummie 
bool CGSDParser::CCreateCompositeObjectFromGSD(MeshBuilder *bld)
{
  //std::list<GenericObjectData *> meshesList;
  std::list<DummieData *> dummieList;  
  GSDHeader header;

  m_f=fopen(m_FileName,"rb");
  if(m_f==NULL)
    return false;
  fread(&header,sizeof(GSDHeader),1,m_f);    
  int i;
  //===========================================
  //        READING GROUPS
  //===========================================
  DummieData *dummie=NULL;
  treeData *node=NULL;
  dummie = new DummieData; 
  fread(dummie,sizeof(DummieData),1,m_f);
  dummieList.push_back(dummie);
  node=new treeData;
  strcpy(node->name,dummie->name);
  strcpy(node->parent,dummie->parentName);
  CBuildTreeData(node);
    
  //===========================================
  //        END OF READING GROUPS
  //===========================================
   
  //===========================================
  //        READING MESHES
  //===========================================
  GenericObjectData *o=NULL;
  //CBasicMesh *obj=NULL;
  for(i=0;i<header.numberOfObjects;++i)
  { 
    o = new GenericObjectData;
 
    fread(o->Name,sizeof(char)*256,1,m_f);
    fread(o->ParentName,sizeof(char)*256,1,m_f);
    if(!_stricmp(o->ParentName,"Scene Root"))
      strcpy(o->ParentName,dummie->name);
    fread(o->MaterialName,sizeof(char)*256,1,m_f);
    
    fread(&o->iC,sizeof(unsigned long),1,m_f);
    fread(&o->vC,sizeof(unsigned long),1,m_f);
    //fread(&o->tC,sizeof(float),1,m_f);
    //fread(&o->vC,sizeof(float),1,m_f);

    o->Index = new unsigned int[o->iC];
    fread(o->Index,sizeof(unsigned int)*o->iC,1,m_f); // indexes
    //o->Geometry = new float[o->vC*3*sizeof(float)];
    o->Geometry = new float[o->vC*3];
    fread(o->Geometry,o->vC*3*sizeof(float),1,m_f); // vertices

    //o->TexCoord = new float[o->tC*2*sizeof(float)];
    o->TexCoord = new float[o->vC*2];
    fread(o->TexCoord,o->vC*2*sizeof(float),1,m_f);
    
    //o->Normals= new float[o->vC*3*sizeof(float)];
    o->Normals= new float[o->vC*3];
    fread(o->Normals,o->vC*3*sizeof(float),1,m_f);

    o->Tangent = new float[o->vC*3];
    fread(o->Tangent,o->vC*3*sizeof(float),1,m_f);

    o->Binormal = new float[o->vC*3];
    fread(o->Binormal,o->vC*3*sizeof(float),1,m_f);

	bld->startSurface(o->vC, o->iC / 3, 3); // no initial guess
	bld->setVtxStart(0);
	bld->setHas(true, false);
	bld->createVtxProp<Vec2>(Prop_TexCoord); // tex-coord
	bld->createVtxProp<Vec3>(Prop_Tangent); // tangent
	bld->createVtxProp<Vec3>(Prop_BiTangent); // bitangent

	for(uint i = 0; i < o->vC; ++i)
	{
		int ti = 3 * i;
		int index = bld->addVtx(o->Geometry[ti], o->Geometry[ti + 1], o->Geometry[ti + 2]);
		bld->setNormal(index, o->Normals[ti], o->Normals[ti + 1], o->Normals[ti + 2]);
		bld->setVtxProp(Prop_TexCoord, index, o->TexCoord[i * 2], o->TexCoord[i * 2 + 1]);
		bld->setVtxProp(Prop_Tangent, index, o->Tangent[i * 3], o->Tangent[i * 3 + 1], o->Tangent[i * 3 + 2]);
		bld->setVtxProp(Prop_BiTangent, index, o->Binormal[i * 3], o->Binormal[i * 3 + 1], o->Binormal[i * 3 + 2]);
	}

	for(uint i = 0; i < o->iC; i += 3)
	{
		bld->addFace(MeshBuilder::Face(o->Index[i], o->Index[i + 1], o->Index[i + 2]));
	}
	bld->endSurface();
    
    //meshesList.push_back(o);
    node = new treeData;
    strcpy(node->name,o->Name);
    strcpy(node->parent,o->ParentName);
    CBuildTreeData(node);

	break; // read only the first one. Mesh doesn't support multiple meshes.
  }
  //===========================================
  //        END OF READING MESHES
  //===========================================
  //===========================================
  //        READING HELPERS
  //===========================================
/*
  for(i=0;i<header.numberOfHelpers;++i)
  {
    dummie = new DummieData; 
    fread(dummie,sizeof(DummieData),1,m_f);
    dummieList.push_back(dummie);
    node=new treeData;
    strcpy(node->name,dummie->name);
    strcpy(node->parent,dummie->parentName);
    CBuildTreeData(node);
  }*/

  //===========================================
  //        END OF READING HELPERS
  //===========================================
  //===========================================
  //            BUILDING MODEL
  //===========================================
//  CBasicDummie *Model=NULL,*findedModel=NULL,*child=NULL;
  DummieData *d=(*dummieList.begin());//,*p=NULL;
//  Model = new CBasicDummie(*d);
  dummieList.erase( (dummieList.begin()) );
  if(d) delete d;
//  std::list<GenericObjectData *>::iterator mIter=meshesList.begin();
  std::list<DummieData *>::iterator hIter=dummieList.begin();  
  
 /* while((meshesList.size()>0)||(dummieList.size()>0))
  {
    //Busco en las mallas, si no lo encuentro, entonces busco en los helpers, si no lo encuentro, sumo en los dos, 
    //si lo encuentro en las mallas sumo mallas, si lo encuentro en helpers, sumo helpers

    if(meshesList.size()>0)
    {
      o=(*mIter);
      findedModel=Model->CFindChild(o->ParentName);
      if(findedModel)
      {
        child = new CBasicMesh(o);
        findedModel->CInsertChild(child);
        if(o) delete o;
        meshesList.erase(mIter);
        mIter=meshesList.begin();
      }
      else
      {
        mIter++;
        if(mIter==meshesList.end())
          mIter=meshesList.begin();
      }
    }
    if(dummieList.size()>0)
    {
      p=(*hIter);
      findedModel=Model->CFindChild(p->parentName);
      if(findedModel)
      {
        child = new CBasicDummie(*p);
        findedModel->CInsertChild(child);
        if(p) delete p;
        dummieList.erase(hIter);
        hIter=dummieList.begin();
      }
      else
      {
        hIter++;
        if(hIter==dummieList.end())
          hIter=dummieList.begin();        
      }

    }
  }  */

  // si no encuentra al padre, pasar al siguiente nodo
  //===========================================
  //         END OF BUILDING MODEL
  //===========================================
  //===========================================
  //            CLEANING MEMORY
  //===========================================  
 /*
  for(;hIter!=dummieList.end();hIter++)
   {
     dummie = (*hIter);
     if(dummie) delete dummie;
   }*/
 

  
  for(i=0;i<(int)m_tree.size();i++)
  {
    treeData *t=(m_tree[i]);
    if(t) delete t;
  }
  //dummieList.clear();
  m_tree.clear();
  
/*
  mIter=meshesList.begin();  
  for(;mIter!=meshesList.end();mIter++)
  {
    o = (*mIter);
    if(o) delete o;
  }
  meshesList.clear();*/

  //===========================================
  //        END OF CLEANING MEMORY
  //===========================================
  fclose(m_f);
  return true; //Model;
}
//-------------------------------------------------------------------------------------
//##ModelId=42305DF20295
void CGSDParser::CBuildTreeData(treeData *dummie)
{
  int size=(int)m_tree.size();
  int i;
  standardString *d=NULL;
  for(i=0;i<size;++i)
  {
    if(!_stricmp(dummie->parent,m_tree[i]->name))
    {
      d = new standardString; 
      strcpy(d->data,dummie->name);
      m_tree[i]->childs.push_back(d);
    }
  }  
  m_tree.push_back(dummie);

}
//-------------------------------------------------------------------------------------
/*void CGSDParser::CBuildObjectMaterial(Material *objMaterial,const char *objectName,Material *matTemplate)
{    
  objMaterial->Layer1HasEnvMap =  matTemplate->Layer1HasEnvMap;
  objMaterial->HasTexture =  matTemplate->HasTexture;
  objMaterial->Shininnes =  matTemplate->Shininnes;

  memcpy(objMaterial->Ambient,  matTemplate->Ambient,sizeof(float)*4);
  memcpy(objMaterial->Diffuse,  matTemplate->Diffuse,sizeof(float)*4);
  memcpy(objMaterial->Specular,  matTemplate->Specular,sizeof(float)*4);
  memcpy(objMaterial->Emission,  matTemplate->Emission,sizeof(float)*4);
  
  strcpy(objMaterial->TextureLayer1FileName,matTemplate->TextureLayer1FileName);
  
  memcpy(objMaterial->TraslationAxis, matTemplate->TraslationAxis,sizeof(float)*3);
  memcpy(objMaterial->RotationAxis,  matTemplate->RotationAxis,sizeof(float)*3);
  memcpy(objMaterial->ScaleAxis,  matTemplate->ScaleAxis,sizeof(float)*3);
  strcpy(objMaterial->MaterialName,objectName);

  objMaterial->Lighted=true;
  objMaterial->AleatoryRotation=false;
  objMaterial->Continuos=false;
  objMaterial->HasMultiTexture=false;
  objMaterial->Layer2HasEnvMap=false;
  memset(objMaterial->TextureLayer2FileName,0,sizeof(objMaterial->TextureLayer2FileName));
}
*/















