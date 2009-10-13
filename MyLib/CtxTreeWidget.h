#ifndef CTXTREEWIDGET_H_INCLUDED
#define CTXTREEWIDGET_H_INCLUDED

#include <QTreeWidget>
#include <QContextMenuEvent>
#include <QMenu>

class CtxTreeWidget : public QTreeWidget
{
public:
	CtxTreeWidget(QWidget * parent): QTreeWidget(parent), m_eventItem(NULL)
	{
		setContextMenuPolicy(Qt::DefaultContextMenu);
	}

	void setTypeMenu(int type, QMenu* menu)
	{
		m_menus.insert(type, menu);
	}

	// the item under the context menu
	QTreeWidgetItem* eventItem() { return m_eventItem; }

protected:
	void contextMenuEvent(QContextMenuEvent *event)
	{
		QTreeWidgetItem *item = itemAt(event->pos());
		if (item == NULL)
			return;
		TMenusMap::iterator it = m_menus.find(item->type());
		if (it != m_menus.end())
		{
			m_eventItem = item;
			(*it)->exec(event->globalPos());
			m_eventItem = NULL;
		}
	}

private:
	typedef QMap<int, QMenu*> TMenusMap;
	TMenusMap m_menus;
	QTreeWidgetItem* m_eventItem;
};

#endif // CTXTREEWIDGET_H_INCLUDED