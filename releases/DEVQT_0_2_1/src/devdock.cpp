/****************************************************************************
**
** Copyright (C) 2005-2005 Full Metal Coder. All rights reserved.
**
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "devdock.h"

DevDock::DevDock(	const QString& name, 
					Qt::DockWidgetArea area, 
					QMainWindow *p, 
					Qt::WFlags f)
    : QDockWidget(p, f)
{
	setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	
	//setObjectName(name);
    setWindowTitle(name);

    movableAction = new QAction(tr("Movable"), this);
    movableAction->setCheckable(true);
    connect(movableAction, SIGNAL(triggered(bool)), SLOT(changeMovable(bool)));

    floatableAction = new QAction(tr("Floatable"), this);
    floatableAction->setCheckable(true);
    connect(floatableAction, SIGNAL(triggered(bool)), SLOT(changeFloatable(bool)));

    floatingAction = new QAction(tr("Floating"), this);
    floatingAction->setCheckable(true);
    connect(floatingAction, SIGNAL(triggered(bool)), SLOT(changeFloating(bool)));

    allowedAreasActions = new QActionGroup(this);
    allowedAreasActions->setExclusive(false);

	allowLeftAction = new QAction(tr("Allow on Left"), this);
	allowLeftAction->setCheckable(true);
	connect(allowLeftAction, SIGNAL(triggered(bool)), SLOT(allowLeft(bool)));
	
	allowRightAction = new QAction(tr("Allow on Right"), this);
	allowRightAction->setCheckable(true);
	connect(allowRightAction, SIGNAL(triggered(bool)), SLOT(allowRight(bool)));

	allowTopAction = new QAction(tr("Allow on Top"), this);
	allowTopAction->setCheckable(true);
	connect(allowTopAction, SIGNAL(triggered(bool)), SLOT(allowTop(bool)));

	allowBottomAction = new QAction(tr("Allow on Bottom"), this);
	allowBottomAction->setCheckable(true);
	connect(allowBottomAction, SIGNAL(triggered(bool)), SLOT(allowBottom(bool)));
	
	
    areaActions = new QActionGroup(this);
    areaActions->setExclusive(true);
	
	leftAction = new QAction(tr("Place on Left") , this);
    leftAction->setCheckable(true);
    connect(leftAction, SIGNAL(triggered(bool)), SLOT(placeLeft(bool)));

    rightAction = new QAction(tr("Place on Right") , this);
    rightAction->setCheckable(true);
    connect(rightAction, SIGNAL(triggered(bool)), SLOT(placeRight(bool)));
    
	topAction = new QAction(tr("Place on Top") , this);
    topAction->setCheckable(true);
    connect(topAction, SIGNAL(triggered(bool)), SLOT(placeTop(bool)));

   	bottomAction = new QAction(tr("Place on Bottom") , this);
   	bottomAction->setCheckable(true);
   	connect(bottomAction, SIGNAL(triggered(bool)), SLOT(placeBottom(bool)));
    
    if ( area & 3 )
	{
		allowedAreasActions->addAction(allowLeftAction);
		allowedAreasActions->addAction(allowRightAction);
    	
    	areaActions->addAction(leftAction);
    	areaActions->addAction(rightAction);
	}
	
	if ( area & 12 )
	{
		allowedAreasActions->addAction(allowTopAction);
		allowedAreasActions->addAction(allowBottomAction);
    	
    	areaActions->addAction(topAction);
    	areaActions->addAction(bottomAction);
	}
	
    connect(movableAction, SIGNAL(triggered(bool)), areaActions, SLOT(setEnabled(bool)));
	
    connect(movableAction, SIGNAL(triggered(bool)), allowedAreasActions, SLOT(setEnabled(bool)));
	
    connect(floatableAction, SIGNAL(triggered(bool)), floatingAction, SLOT(setEnabled(bool)));
	
    connect(floatingAction, SIGNAL(triggered(bool)), floatableAction, SLOT(setDisabled(bool)));
    connect(movableAction, SIGNAL(triggered(bool)), floatableAction, SLOT(setEnabled(bool)));
	
    menu = new QMenu(name, this);
    menu->addAction(movableAction);
    menu->addAction(floatableAction);
    menu->addAction(floatingAction);
    menu->addSeparator();
    menu->addActions(allowedAreasActions->actions());
    menu->addSeparator();
    menu->addActions(areaActions->actions());
    
    tabW = new QTabWidget;
    tabW->setMinimumSize(150, 75);
    tabW->setWindowState(Qt::WindowMaximized);
    setWidget(tabW);
    
    if (p)
		p->addDockWidget(area, this);
}

QTabWidget* DevDock::Tab()	const { return tabW; }
QMenu* 	DevDock::Menu()		const { return menu; }

void DevDock::contextMenuEvent(QContextMenuEvent *event)
{
    event->accept();
    menu->exec(event->globalPos());
}

bool DevDock::event(QEvent *e)
{
    if (e->type() != QEvent::Polish)
        return QDockWidget::event(e);

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    const Qt::DockWidgetArea area = mainWindow->dockWidgetArea(this);
    const Qt::DockWidgetAreas areas = allowedAreas();
    
    if (windowType() == Qt::Drawer) {
        floatableAction->setEnabled(false);
        floatingAction->setEnabled(false);
        movableAction->setEnabled(false);
    } else {
        floatableAction->setChecked(features() & QDockWidget::DockWidgetFloatable);
        floatingAction->setChecked(isWindow());
        // done after floating, to get 'floatable' correctly initialized
        movableAction->setChecked(features() & QDockWidget::DockWidgetMovable);
    }

    allowLeftAction->setChecked(isAreaAllowed(Qt::LeftDockWidgetArea));
    allowRightAction->setChecked(isAreaAllowed(Qt::RightDockWidgetArea));
    allowTopAction->setChecked(isAreaAllowed(Qt::TopDockWidgetArea));
    allowBottomAction->setChecked(isAreaAllowed(Qt::BottomDockWidgetArea));

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftDockWidgetArea);
        allowRightAction->setEnabled(area != Qt::RightDockWidgetArea);
        allowTopAction->setEnabled(area != Qt::TopDockWidgetArea);
        allowBottomAction->setEnabled(area != Qt::BottomDockWidgetArea);
    }

    leftAction->blockSignals(true);
    rightAction->blockSignals(true);
    topAction->blockSignals(true);
    bottomAction->blockSignals(true);

    leftAction->setChecked(area == Qt::LeftDockWidgetArea);
    rightAction->setChecked(area == Qt::RightDockWidgetArea);
    topAction->setChecked(area == Qt::TopDockWidgetArea);
    bottomAction->setChecked(area == Qt::BottomDockWidgetArea);

    leftAction->blockSignals(false);
    rightAction->blockSignals(false);
    topAction->blockSignals(false);
    bottomAction->blockSignals(false);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftDockWidgetArea);
        rightAction->setEnabled(areas & Qt::RightDockWidgetArea);
        topAction->setEnabled(areas & Qt::TopDockWidgetArea);
        bottomAction->setEnabled(areas & Qt::BottomDockWidgetArea);
    }
    return QDockWidget::event(e);
}

void DevDock::allow(Qt::DockWidgetArea area, bool a)
{
    Qt::DockWidgetAreas areas = allowedAreas();
    areas = a ? areas | area : areas & ~area;
    setAllowedAreas(areas);

    if (areaActions->isEnabled()) {
        leftAction->setEnabled(areas & Qt::LeftDockWidgetArea);
        rightAction->setEnabled(areas & Qt::RightDockWidgetArea);
        topAction->setEnabled(areas & Qt::TopDockWidgetArea);
        bottomAction->setEnabled(areas & Qt::BottomDockWidgetArea);
    }
}

void DevDock::place(Qt::DockWidgetArea area, bool p)
{
    if (!p) return;

    QMainWindow *mainWindow = qobject_cast<QMainWindow *>(parentWidget());
    mainWindow->addDockWidget(area, this);

    if (allowedAreasActions->isEnabled()) {
        allowLeftAction->setEnabled(area != Qt::LeftDockWidgetArea);
        allowRightAction->setEnabled(area != Qt::RightDockWidgetArea);
        allowTopAction->setEnabled(area != Qt::TopDockWidgetArea);
        allowBottomAction->setEnabled(area != Qt::BottomDockWidgetArea);
    }
}

void DevDock::changeMovable(bool on)
{ setFeatures(on ? features() | DockWidgetMovable : features() & ~DockWidgetMovable); }

void DevDock::changeFloatable(bool on)
{ setFeatures(on ? features() | DockWidgetFloatable : features() & ~DockWidgetFloatable); }

void DevDock::changeFloating(bool floating)
{ setFloating(floating); }

void DevDock::allowLeft(bool a)
{ allow(Qt::LeftDockWidgetArea, a); }

void DevDock::allowRight(bool a)
{ allow(Qt::RightDockWidgetArea, a); }

void DevDock::allowTop(bool a)
{ allow(Qt::TopDockWidgetArea, a); }

void DevDock::allowBottom(bool a)
{ allow(Qt::BottomDockWidgetArea, a); }

void DevDock::placeLeft(bool p)
{ place(Qt::LeftDockWidgetArea, p); }

void DevDock::placeRight(bool p)
{ place(Qt::RightDockWidgetArea, p); }

void DevDock::placeTop(bool p)
{ place(Qt::TopDockWidgetArea, p); }

void DevDock::placeBottom(bool p)
{ place(Qt::BottomDockWidgetArea, p); }
