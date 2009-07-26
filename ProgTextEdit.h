#ifndef PROGTEXTEDIT_H_INCLUDED
#define PROGTEXTEDIT_H_INCLUDED

#include <QTextEdit>
#include "MyLib/WidgetMenuer.h"

class QMenu;

class ProgTextEdit : public QTextEdit, public WidgetMenuer
{
	Q_OBJECT
public:
	ProgTextEdit(QWidget* parent) : QTextEdit(parent), WidgetMenuer(this) {}
	void setFontSize(int size);

protected:
	virtual void keyPressEvent(QKeyEvent * event);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void contextMenuEvent(QContextMenuEvent *event) { cMenuEvent(event); }

signals:
	void zoomChange(int delta);
};


#endif // PROGTEXTEDIT_H_INCLUDED
