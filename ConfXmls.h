#ifndef CONFXMLS_H_INCLUDED
#define CONFXMLS_H_INCLUDED

#include <QString>

class QMenu;
class QDomElement;
class QWidget;
class KwEdit;

class ConfXmls
{
public:
	ConfXmls(const QString& modelsFile, const QString& progFiles, KwEdit* host)
		:m_modelsFile(modelsFile), m_progFiles(progFiles), m_host(host)
	{}

	void loadModelsFile(QWidget *w);

	void loadProgramElement(QMenu* menu, const QString& display, QDomElement &pe);
	void loadProgramsElement(QMenu* menu, QDomElement &pe);
	void loadProgramsFile(QMenu* menu);


private:
	QString m_modelsFile, m_progFiles;
	KwEdit* m_host;

};

#endif // CONFXMLS_H_INCLUDED
