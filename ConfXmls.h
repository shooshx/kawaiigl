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

class ConfXmls : public QObject
{
	Q_OBJECT
public:
	ConfXmls(KawaiiGL* mainc);

	void init(const QString& modelsFile, const QString& progFiles);

	void loadModelsFile();

	void loadPassElement(ProgKeep::PassKeep* pass, QDomElement &pe);
	void loadProgramElement(QMenu* menu, const QString& display, QDomElement &pe);
	void loadProgramsElement(QMenu* menu, QDomElement &pe);
	void loadProgramsFile(QMenu* menu);

	void activateAction(const QString& name);

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

	QString m_modelsFile, m_progFiles;
	
	
	QList<ProgKeep*> m_progrep;

	typedef QMap<QString, QAction*> TActionMap;
	TActionMap m_userActions;

public:
	QMenu *m_progMenu, *m_modelsMenu;

};

#endif // CONFXMLS_H_INCLUDED
