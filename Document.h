#pragma once
#ifndef DOCUMENT_H_INCLUDED
#define DOCUMENT_H_INCLUDED

#include <QObject>

#include "KwParser.h"
#include "ProgInput.h"
#include "MyLib/Mesh.h"

#include "OpenGL/Shaders.h"
#include "Renderable.h"
#include "ConfXmls.h"
#include "Pass.h"

class MyObject;
class DisplayConf;
class KawaiiGL;
class Prop;
class MyPolygon;

// track interactive adding of points
class AddTracker
{
public:
    void reset()
    {
        m_added.clear();
    }
    // returns true if done
    bool add(IPoint* p)
    {
        if (!m_added.isEmpty() && p == m_added.first())
            return true;
        m_added.append(p);
        return false;
    }

    QVector<IPoint*> m_added;
};



class Document : public QObject
{
    Q_OBJECT
public:
    Document(KawaiiGL* mainc);
    ~Document() {}

    int numPoly() { return m_nPoly; }
    int numPoints() { return m_nPoints; }
    //bool isValid() { return m_kparser.isValid(); }

    void emitProgChanged() { emit progChanged(); }

    template<typename T>
    void directUpdate(const ParamInput& pi, const T& val)
    {
        if (!isEType(pi.type, val))
            return;
        {
            ProgramUser use(&pi.mypass->prog);
            pi.mypass->prog.setUniform(val, pi.index);
        }
        emit progParamChanged(); // causes updateGL
    }

    static QIcon getTypeIcon(ElementType);
    static QString getTypeName(ElementType);

    void addNewShader(RenderPass* pass, ElementType type);
    void loadShaderFile(RenderPass* pass, const QString& filename, ElementType type);
    void removeShader(RenderPass* pass, DocSrc* src);

    void addNewPass();
    void removePass(Pass* pass);
    void movePass(Pass* pass, int delta);

    void addNewSwap();

    bool isProgEnabled() const { return m_shaderEnabled; }

    void addPass(PassPtr pass);
    void clearPasses();
    RenderPassPtr passForModel(); // find the pass where the model should go.
    const DisplayConf& dispConf() { return m_conf; }

    KwSettings& sett();


public slots:
    void calc(DocSrc* qstr, bool doParse = true, bool purgePointCache = false);
    void calcNoParse();
    void calcSave();

    void compileShaders();

    bool parseSingleParam(const ParamInput& in, bool update);

    
    void updateTrack(IPoint* sel);

    bool parseAttrib(const ParamInput &pi);
    void setAttribs(MyPolygon* poly, int vi); // set values to attributes
    void setAttribs(Mesh::Vertex_const_handle vh);
    void initAttribMesh(const Mesh* rmesh);

private slots:
    void setAddTrack();
    void readProg(ProgKeep* prog); // fcom config
    void readModel(const QString& name, const ModelData& md);
    void loadProgramFile();

signals:
    void loaded();
    void modelChanged();
    void progChanged(); // just compiled a program, update the view
    void progParamChanged();
    void addModelLine(const QString& text);

    void goingToClearProg();
    void didReadProg(ProgKeep* prog);
    void didReadModel(DocSrc* src);

private:
    bool parseParam(const ParamInput& in); // if not NULL this prop should get the value as well.
    void parseAllParams(const RenderPassPtr &pass);

    QString dedicatedProcess();
    
    QString generateFromFile();



public:


    // this connects the parser with the program. changes in the parser to the program
    struct ParamAdapter
    {
        ParamAdapter(int _index) :index(_index) {}
        virtual ~ParamAdapter() {}
        virtual void update(GenericShader &prog) const = 0;

        int index; // index of the program parameter we need to update.
    };

    struct VecParamAdapter : public ParamAdapter
    {
        VecParamAdapter(IPoint* _pe, Prop* _prop, int _i) : pe(_pe), toprop(_prop), ParamAdapter(_i) {}
        IPoint* pe;
        Prop* toprop; // if not NULL this prop should get the value as well.
        
        virtual void update(GenericShader &prog) const;
    };

    struct TexParamAdapter : public ParamAdapter
    {
        TexParamAdapter(Document* doc, int _i) : m_doc(doc), ParamAdapter(_i) {}
        virtual void update(GenericShader &prog) const;

        Document* m_doc;
    };

    // this maps passName + paramNumbae (string) to the adapter.
    // the mapping is done to avoid multiple adapters for the same parameter
    // TBD: need to get rid of them when they change type?
    typedef QMap<QString, shared_ptr<ParamAdapter> >  TAdaptersList;
    //TAdaptersList m_onCalcEvals; // things that are re-evaluated when there's a calc() call
    TAdaptersList m_onFrameEvals; // things that are reeveluated every frame

    struct AttribEval
    {
        AttribEval() : m_prog(NULL), index(-1) {}
        void setEnv(GenericShader *prog, int i) 
        {
            m_prog = prog;
            index = i;
        }

        int index; // index of the program parameter we need to update.
        GenericShader *m_prog;

        virtual void setAttribVal(const MyPolygon* poly, int vindex) = 0;
        virtual bool initWithMesh(const Mesh *rmesh) = 0;
        virtual void setAttribVal(Mesh::Vertex_const_handle vh) = 0;
    };


    void updateParams(TAdaptersList& list); // updates the values of all the parsed params according to the new cache.
    void updateFrameParams()
    {
        updateParams(m_onFrameEvals);
    }

    static void readToString(const QString& filename, QString& into);
    static void writeToFile(const QString& text, const QString filename);


    ModelDocument* model() {
        return m_currentModel.get();
    }

public:
    shared_ptr<ModelDocument> m_currentModel;
    QVector<shared_ptr<ModelDocument>> m_allModels;
    DisplayConf& m_conf;

private:
    int m_nPoly, m_nPoints;
    KawaiiGL* m_main;

    QVector<shared_ptr<AttribEval> > m_attribEval;

    //QString m_curtext; // of model

    bool m_inCalc; // prevent recursive calls to calc()

    //GenericShader m_prog;

    //int m_inputUnit, m_outputUnit; // used in Tex2Tex; not any more
    // these are here since they need to be sent as uniforms and we need access to them.


public:
    typedef QList<PassPtr> TPasses;
    TPasses m_passes;
    ConfXmls m_confxmls;
    bool m_shaderEnabled;

};


class ModelDocument 
{
public:
    ModelDocument(Document* doc);

    void generateTorus(const std::vector<std::string>& args);
    void generateCurve(const std::vector<std::string>& args);
    void generateRotObj(const std::vector<std::string>& args);
    void generateQuadSphere(const std::vector<std::string>& sep);
    void generateTriSphere(const std::vector<std::string>& sep);
    void generateIsoSurface(const std::vector<std::string>& args);

public:
    shared_ptr<MyObject> m_frameObj, m_obj;
    QVector<shared_ptr<Mesh> > m_meshs;
    QVector<shared_ptr<Renderable> > m_rends;

    ErrorActor *m_errAct; // for syntax error highlighting
    AddTracker m_addTrack;

    typedef QMap<QString, shared_ptr<Mesh> > TMeshIndex;


    KwParser m_kparser;
    TMeshIndex m_meshIndex;

    DisplayConf& m_conf;
};



#endif // DOCUMENT_H_INCLUDED