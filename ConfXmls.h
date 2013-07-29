#ifndef CONFXMLS_H_INCLUDED
#define CONFXMLS_H_INCLUDED

#include <QString>
#include <QObject>
#include <QMap>

#include "Pass.h"

class QMenu;
class QDomElement;
class QWidget;
class QAction;
struct ModelData;
class KawaiiGL;
class DisplayConf;
class Document;

class ConfXmls : public QObject
{
	Q_OBJECT
public:
	ConfXmls(KawaiiGL* mainc, const QString& modelsFile, const QString& progFiles);

	void loadModelsFile(const QString& filename, QMenu* menu);
	void loadProgramsFile(const QString& filename, QMenu* menu);

	void loadPassElement(ProgKeep::RenderPassKeep* pass, QDomElement &pe);
    void loadProgramElement(const QString& display, QDomElement &pe, ProgKeep *prog);
	void loadProgramsElement(QMenu* menu, QDomElement &pe);

	void activateAction(const QString& name);

    void parseProgFile(const QString& filename, ProgKeep* prog);
    bool saveProg(const Document& doc, const QString& filename);

private:
	QAction* confAddModel(const QString& display, const QString& filename, bool isMesh);
	QAction* confAddProg(const QString& display, ProgKeep* prog);

private slots:
	void myreadProg();
	void myreadModel();

signals:
	void readProg(ProgKeep* prog);
	void readModel(const QString& action, const ModelData& md);

private:
	DisplayConf &m_conf;
	QWidget *m_parent;

	QList<ProgKeep*> m_progrep;

	typedef QMap<QString, QAction*> TActionMap;
	TActionMap m_userActions;


};

#endif // CONFXMLS_H_INCLUDED
