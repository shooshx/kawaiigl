/******************************************************************************
*                             gsdparser.h file                                *
*           This source code is property and copyrighted of TyphoonLabs       *
*                 and is released under standard LGPL license                 *
*                            see README.TXT for details                       *
*******************************************************************************/

#pragma once

//#include "global.h"
#include <list>
#include <vector>
#include <stdio.h>
//#include "mathes.h"
//#include "Object3d.h"
//#include "mesh.h"
#include <string>

struct GSDHeader
{
  char id[34]; 
  char version[16];
  bool staticMesh; // para saber si es un mesh estatico o no
  int numberOfObjects;
  int numberOfHelpers;
  unsigned int numberOfFrames;
}; 

struct standardString
{
  char data[256];
};
struct treeData
{
  char name[256];
  char parent[256];
  std::list<standardString *> childs;
};

class MeshBuilder;

class CGSDParser
{
public:
  CGSDParser(const char *fileName);
  ~CGSDParser();    
  const char *CGetGSDFileVersion() {return m_version;};
  bool CCreateCompositeObjectFromGSD(MeshBuilder* bld);

private:
  //void CBuildObjectMaterial(Material *objMaterial,const char *objectName,Material *matTemplate);
  void CBuildTreeData(treeData *dummie); // añade el objeto a la lista y actualiza esta con un hijo nuevo donde corresponda
  char m_FileName[256];
  std::vector<treeData *> m_tree;
  FILE *m_f;
  long m_subObjectsNumber;
  long m_MaterialsNumber;
  char m_version[16];    
};

