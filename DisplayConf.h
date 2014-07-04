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
    Q_ENUMS(ETexAct)
    Q_ENUMS(ETexEdge)
public:
    enum ETexAct { Tex_None = -1, Tex_File0, Tex_File1, Tex_File2, Tex_File3 };
    enum ETexEdge { Clamp, Repeat, Mirror };


    TypeProp<int> numberOfPasses; // 0, 1, 2, 3, 4, 5
    TypeProp<bool> *passRound[N_PASS];
    TypeProp<bool> bVtxNormals;
    TypeProp<bool> bLines, bPoly, bPoints, bUnusedPoints, bAllPolyPoints, bLinesAll, bCull, bCoordNum, bCoordName;
    TypeProp<bool> linePoly; // draw polygons as lines (with lighting)
    TypeProp<bool> bTriangulate;
    TypeProp<bool> bBlend;
    TypeProp<bool> bDepthTest;
    TypeProp<bool> perspective, lighting;
    TypeProp<int> coordFontSize;
    TypeProp<int> pointSize;
    TypeProp<int> lineWidth;
    TypeProp<bool> bLitBackFace;
    TypeProp<bool> bLightMove, isPointLight;
    TypeProp<ETexAct> texAct;
    TypeProp<QString> *texFile[N_TEX];
    TypeProp<QImage> *texImage[N_TEX]; // if the image "isNull()", take the text
//	TypeProp<bool> quadMultiSamp;
    TypeProp<QColor> lightAmb, lightDiff, lightSpec;
    TypeProp<int> materialShine;
    TypeProp<QColor> backCol, materialCol, selectedCol, lineColor;
    TypeProp<int> editFontSize;
    TypeProp<bool> fullFps;
    TypeProp<bool> vSync;

    TypeProp<bool> addFace;
    TypeProp<int> trackForParam; // should the mouse be tracked for param updates
    TypeProp<QString> lastDir;
    TypeProp<bool> texSigned;
    TypeProp<ETexEdge> texEdge;
//	TypeProp<RunType> runType;

    DisplayConf() 
        :numberOfPasses(this, "numberOfPasses", "Number Of Passes", 0)
        ,bVtxNormals(this, "bVtxNormals", "use Vertex Normals", false)
        ,bLines(this, "bLines", "show Lines", false)
        ,bLitBackFace(this, "bLitBackFace", "Lit Backface", false)
        ,bPoly(this, "bPoly", "Show Polygons", true)
        ,bPoints(this, "bPoints", "Show Points", false)
        ,bUnusedPoints(this, "bUnusedPoints", "Show unused Points", false)
        ,bAllPolyPoints(this, "bAllPolyPoints", "Show all points of polygons", false)
        ,bLightMove(this, "bLightMove", "Move Light", false)
        ,bLinesAll(this, "bLinesAll", "Show all Lines", false)
        ,bTriangulate(this, "bTriangulate", "Triangulate Quads", false)
        ,bBlend(this, "bBlend", "Alpha Blending", false)
        ,bDepthTest(this, "bDepthTest", "Enable Depth Test", true)
        ,linePoly(this, "linePoly", "Polygons as lit lines", false)
        ,coordFontSize(this, "coordFontSize", "Font Size", init(10, 1, 72))
        ,pointSize(this, "pointSize", "Point Size", init(14, 1, 200))
        ,lineWidth(this, "lineWidth", "Line Width", init(1, 1, 255))
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
        ,vSync(this, "vSync", "vSync", true)
        ,texSigned(this, "texSigned", "texSigned", false) // not used
        ,texEdge(this, "texEdge", "texEdge", Clamp)
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
            
            texImage[i] = new TypeProp<QImage>(this, "texImage"+r, "Tex Image " + r, init("", QImage()));
            texImage[i]->index() = i;
            texImage[i]->shouldStore(false);
        }
        addFace.shouldStore(false);
        trackForParam.shouldStore(false);
    //	runType.shouldStore(false);
    }
};

Q_DECLARE_METATYPE(DisplayConf::ETexAct);
Q_DECLARE_METATYPE(DisplayConf::ETexEdge);


class GUIParams : public ParamBase
{
public:
    //TypeProp<int> configWindowTab;
    TypeProp<QString> saveDir;
    //TypeProp<int> editWindowTab;

    GUIParams()
    //	:configWindowTab(this, "configWindowTab", 0)
        :saveDir(this, "saveDir", "saveDir", "")
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
        addParam(&gui, "gui");
    }

    DisplayConf disp;
    GUIParams gui;

};

template<typename T>
class UserData : public QObjectUserData
{
public:
    UserData(T _i) : i(_i) {}
    T i;
};


#endif // DISPLAY_CONF_INCLUDED
