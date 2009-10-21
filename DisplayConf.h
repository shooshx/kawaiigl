#pragma once
#ifndef DISPLAY_CONF_INCLUDED
#define DISPLAY_CONF_INCLUDED

#include "MyLib/ParamBase.h"
#include "MyLib/paramSpecialization.h"

#define N_PASS 5
#define N_TEX 4

class DisplayConf : public ParamBase
{
	Q_OBJECT
public:
	enum ETexAct { Tex_None = -1, Tex_File0, Tex_File1, Tex_File2 };
	Q_ENUMS(ETexAct)
		enum RunType
	{
		RunNormal, // 3d Scene rendered to the screen with prog
		RunQuadProcess,  // 3d scene rendered to texture 0, texture 0 rendered to a quad with prog
		RunTex2Tex
	};
	Q_ENUMS(RunType)

	TypeProp<int> numberOfPasses; // 0, 1, 2, 3, 4, 5
	TypeProp<bool> *passRound[N_PASS];
	TypeProp<bool> bVtxNormals;
	TypeProp<bool> bLines, bPoly, bPoints, bUnusedPoints, bLinesAll, bCull, bCoordNum, bCoordName;
	TypeProp<bool> bTriangulate;
	TypeProp<bool> perspective, lighting;
	TypeProp<int> coordFontSize;
	TypeProp<bool> bLitBackFace;
	TypeProp<bool> bLightMove, isPointLight;
	TypeProp<ETexAct> texAct;
	TypeProp<QString> *texFile[N_TEX];
//	TypeProp<bool> quadMultiSamp;
	TypeProp<QColor> lightAmb, lightDiff, lightSpec;
	TypeProp<int> materialShine;
	TypeProp<QColor> backCol, materialCol, selectedCol, lineColor;
	TypeProp<int> editFontSize;
	TypeProp<bool> fullFps;

	TypeProp<bool> addFace;
	TypeProp<int> trackForParam; // should the mouse be tracked for param updates
	TypeProp<QString> lastDir;
//	TypeProp<RunType> runType;

	DisplayConf() 
		:numberOfPasses(this, "numberOfPasses", "Number Of Passes", 0)
		,bVtxNormals(this, "bVtxNormals", "use Vertex Normals", false)
		,bLines(this, "bLines", "show Lines", false)
		,bLitBackFace(this, "bLitBackFace", "Lit Backface", false)
		,bPoly(this, "bPoly", "Show Polygons", true)
		,bPoints(this, "bPoints", "Show Points", false)
		,bUnusedPoints(this, "bUnusedPoints", "Show unused Points", false)
		,bLightMove(this, "bLightMove", "Move Light", false)
		,bLinesAll(this, "bLinesAll", "Show all Lines", false)
		,bTriangulate(this, "bTriangulate", "Triangulate Quads", false)
		,coordFontSize(this, "coordFontSize", "Font Size", 10)
		,bCoordNum(this, "bCoordNum", "Show coordinates", false)
		,bCoordName(this, "bCoordName", "Show Names", false)
		,bCull(this, "bCull", "Show Cull", false)
		,texAct(this, "texAct", "Texture Action", Tex_None)
		//,quadMultiSamp(this, "quadMultiSamp", "Multi Sampling in quad processing", true)
		//,texFile(this, "texFile", "Texture File", "")
		,perspective(this, "perspective", "Perspective", false)
		,lighting(this, "lighting", "Lighting", false)
		,isPointLight(this, "isPointLight", "Is Point Light", false)
		,lightAmb(this, "lightAmb", "Light Ambient Color", QColor(20, 20, 20))
		,lightDiff(this, "lightDiff", "Light Diffuse Color", QColor(255, 255, 255))
		,lightSpec(this, "lightSpec", "Light Specular Color", QColor(100, 100, 100))
		,materialShine(this, "materialShine", "Material Shininess", 120)
		,backCol(this, "backCol", "Background Color", QColor(0,0,0))
		,lineColor(this, "lineColor", "Line Color", QColor(76, 76, 255))
		,materialCol(this, "materialCol", "Material Diffuse Color", QColor(255, 255, 255))
		,selectedCol(this, "selectedCol", "Selected Point Color", QColor(0,0,0))
		,editFontSize(this, "editFontSize", "Edit Font Size", 10)
		,addFace(this, "addFace", "addFace", false)
		,trackForParam(this, "trackForParam", "trackForParam", 0)
		,lastDir(this, "lastDir", "lastDir", "")
		,fullFps(this, "fullFps", "fullFps", false)
	//	,runType(this, "runType", "runType", RunNormal)
	{
		for(int i = 0; i < N_PASS; ++i)
		{
			QString r = QString("%1").arg(i);
			passRound[i] = new TypeProp<bool>(this, "passRound"+r, "Pass round " + r,  (i != 0) );
		}
		for(int i = 0; i < N_TEX; ++i)
		{
			QString r = QString("%1").arg(i);
			texFile[i] = new TypeProp<QString>(this, "texFile"+r, "Tex File " + r,  "" );
			texFile[i]->index() = i;
		}
		addFace.shouldStore(false);
		trackForParam.shouldStore(false);
	//	runType.shouldStore(false);
	}
};

Q_DECLARE_METATYPE(DisplayConf::ETexAct);
Q_DECLARE_METATYPE(DisplayConf::RunType);


class GUIPos : public ParamBase
{
public:
	TypeProp<int> configWindowTab;
	//TypeProp<int> editWindowTab;

	GUIPos()
		:configWindowTab(this, "configWindowTab", 0)
		//,editWindowTab(this, "editWindowTab", 0)
	{}
};

class KwSettings : public MySettings
{
public:
	KwSettings()
		:MySettings("ShyResearch", "KawaiiGL")
	{
		addParam(&disp, "disp");
	//	addParam(&gui, "gui");
	}

	DisplayConf disp;
	//GUIPos gui;

};

template<typename T>
class UserData : public QObjectUserData
{
public:
	UserData(T _i) : i(_i) {}
	T i;
};


#endif // DISPLAY_CONF_INCLUDED
