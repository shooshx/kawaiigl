#ifndef EXGROUPBOX_H_INCLUDED
#define EXGROUPBOX_H_INCLUDED
#pragma once

#include <QGroupBox>

class ExGroupBox : public QGroupBox
{
public:
	ExGroupBox( QWidget *parent = 0 );
	ExGroupBox( const QString &title, QWidget *parent = 0 );

	bool isCollapsed();
	void setCollapse( bool collapse );

protected:
	void mousePressEvent( QMouseEvent *e );
	void mouseReleaseEvent( QMouseEvent *e );
	void paintEvent( QPaintEvent * );

private:
	bool m_clicked;
	bool m_collapsed;
	int m_fullHeight;
};



#endif // EXGROUPBOX_H_INCLUDED