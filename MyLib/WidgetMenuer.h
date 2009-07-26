#pragma once
#ifndef WIDGETMENUER_H_INCLUDED
#define WIDGETMENUER_H_INCLUDED

#include <QMenu>
#include <QContextMenuEvent>


class UnProtectWidget : public QWidget
{
public:
	using QWidget::contextMenuEvent;
};

class WidgetMenuer
{
public:
	WidgetMenuer(QWidget *w) :m_w(w), m_menu(NULL) 
	{}
	
	void setMenu(QMenu *menu) 
	{
		m_menu = menu;
		if (menu != NULL)
			m_w->setContextMenuPolicy(Qt::DefaultContextMenu);
		else
			m_w->setContextMenuPolicy(Qt::NoContextMenu);
	}
	void cMenuEvent(QContextMenuEvent *event)
	{
		if (m_menu == NULL)
		{
			((UnProtectWidget*)m_w)->contextMenuEvent(event);
			return;
		}
		m_menu->exec(event->globalPos());
	}

private:
	QWidget *m_w;
	QMenu *m_menu;
};

#endif // WIDGETMENUER_H_INCLUDED
