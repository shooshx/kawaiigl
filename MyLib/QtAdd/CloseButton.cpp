#include "CloseButton.h"

#include <QStyle>
#include <QPainter>
#include <QStyleOption>

// lifted from src/gui/widgets/qtabbar_p.h


CloseButton::CloseButton(QWidget *parent)
: QAbstractButton(parent)
{
	setFocusPolicy(Qt::NoFocus);
#ifndef QT_NO_CURSOR
	setCursor(Qt::ArrowCursor);
#endif
#ifndef QT_NO_TOOLTIP
	setToolTip(tr("Close Tab"));
#endif
	resize(sizeHint());
}

QSize CloseButton::sizeHint() const
{
	ensurePolished();
	int width = 22; //style()->pixelMetric(QStyle::PM_TabCloseIndicatorWidth, 0, this);
	int height = 22; //style()->pixelMetric(QStyle::PM_TabBarBaseHeight, 0, this);
	//int heightx = style()->pixelMetric(QStyle::PM_TabCloseIndicatorHeight, 0, this);
	return QSize(width, height);  
}

void CloseButton::enterEvent(QEvent *event)
{
	if (isEnabled())
		update();
	QAbstractButton::enterEvent(event);
}

void CloseButton::leaveEvent(QEvent *event)
{
	if (isEnabled())
		update();
	QAbstractButton::leaveEvent(event);
}

void CloseButton::paintEvent(QPaintEvent *)
{
	QPainter p(this);
	QStyleOption opt;
	opt.init(this);
	opt.state |= QStyle::State_AutoRaise;
	if (isEnabled() && /*underMouse() &&*/ !isChecked() && !isDown())
		opt.state |= QStyle::State_Raised;
	if (isChecked())
		opt.state |= QStyle::State_On;
	if (isDown())
		opt.state |= QStyle::State_Sunken;

	if (const QTabBar *tb = qobject_cast<const QTabBar *>(parent())) {
		int index = tb->currentIndex();
		QTabBar::ButtonPosition position = (QTabBar::ButtonPosition)style()->styleHint(QStyle::SH_TabBar_CloseButtonPosition, 0, tb);
		if (tb->tabButton(index, position) == this)
			opt.state |= QStyle::State_Selected;
	}

	style()->drawPrimitive(QStyle::PE_IndicatorTabClose, &opt, &p, this);
}
