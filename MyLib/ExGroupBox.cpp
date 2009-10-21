#include "ExGroupBox.h"

#include <QMouseEvent>
#include <QStyleOptionGroupBox>
#include <QStylePainter>

ExGroupBox::ExGroupBox(QWidget *parent)
	: QGroupBox(parent), m_collapsed(false), m_clicked(false), m_fullHeight(0)
{}
ExGroupBox::ExGroupBox(const QString &title, QWidget *parent)
	: QGroupBox(title, parent), m_collapsed(false), m_clicked(false), m_fullHeight(0)
{}

bool ExGroupBox::isCollapsed() { return m_collapsed; }

void ExGroupBox::mousePressEvent( QMouseEvent *e )
{
	if( e->button() == Qt::LeftButton )
	{
		QStyleOptionGroupBox option;
		initStyleOption( &option );
		QRect buttonArea( 0, 0, 16, 16 );
		buttonArea.moveTopRight(option.rect.adjusted(0, 0, -10, 0).topRight() );
		if (buttonArea.contains(e->pos()))
		{
			m_clicked = true;
			return;
		}
	}
	QGroupBox::mousePressEvent(e);
}

void ExGroupBox::mouseReleaseEvent( QMouseEvent *e )
{
	if (e->button() == Qt::LeftButton && m_clicked)
	{
		m_clicked = false;

		QStyleOptionGroupBox option;
		initStyleOption( &option );
		QRect buttonArea( 0, 0, 16, 16 );
		buttonArea.moveTopRight(option.rect.adjusted(0, 0, -10, 0).topRight() );
		if (buttonArea.contains(e->pos()))
		{
			setCollapse( !isCollapsed() );
			update();
			return;
		}
		
	}
	QGroupBox::mouseReleaseEvent(e);
}

void ExGroupBox::paintEvent( QPaintEvent * )
{
	QStylePainter paint( this );
	QStyleOptionGroupBox option;
	initStyleOption( &option );
	paint.drawComplexControl( QStyle::CC_GroupBox, option );
	paint.drawItemPixmap(
		option.rect.adjusted( 0, 0, -10, 0 ),
		Qt::AlignTop | Qt::AlignRight,
		QPixmap( m_collapsed ?
			":/trolltech/styles/commonstyle/images/down-16.png" :
			":/trolltech/styles/commonstyle/images/up-16.png" ) );

	// PE_IndicatorArrowUp
}

void ExGroupBox::setCollapse(bool collapse)
{
	m_collapsed = collapse;
	foreach (QWidget *widget, findChildren<QWidget*>())
		widget->setHidden(collapse);
	if (m_collapsed)
	{
		m_fullHeight = minimumHeight();
		setMinimumHeight(20);
	}
	else
	{
		setMinimumHeight(m_fullHeight);
		m_fullHeight = 0;
	}
}

