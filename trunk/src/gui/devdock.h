/****************************************************************************
**
** Copyright (C) 2006 FullMetalCoder
**
** This file is part of the DevQt project (beta version)
** 
** This file may be used under the terms of the GNU General Public License
** version 2 as published by the Free Software Foundation and appearing in the
** file GPL.txt included in the packaging of this file.
**
** Notes :	- the DevLineNumber widget is based on the LineNumberWidget provided
** by QSA 1.2.0 (C) Trolltech, as modifications have been done to make it comply
** my requirements, DevLineNumber has to be considered as a derivative work of
** the original widget from the Trolls.
**			- the DevHighlighter class is based on the QSyntaxHighlighter class
** provided by QSA 1.2.0 (C) Trolltech, as modifications have been done to make
** it comply my requirements (and simplify its use!), DevHighlighter has to be
** considered as a derivative work of the original class from the Trolls.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef _DEV_DOCK_H_
#define _DEV_DOCK_H_

#include "dev.h"

typedef struct {
        const char 			*name;
        QWidget 			*child;
        uint 				flags;
        Qt::DockWidgetArea 	area;
} DevDockSetting;

class DevDock : public QDockWidget
{
    Q_OBJECT

    QAction *movableAction;
    QAction *floatableAction;
    QAction *floatingAction;

    QActionGroup *allowedAreasActions;
    QAction *allowLeftAction;
    QAction *allowRightAction;
    QAction *allowTopAction;
    QAction *allowBottomAction;

    QActionGroup *areaActions;
    QAction *leftAction;
    QAction *rightAction;
    QAction *topAction;
    QAction *bottomAction;

public:
    DevDock(const QString &name,
			Qt::DockWidgetArea area,
			QMainWindow *parent,
			Qt::WFlags flags = 0);

	QTabWidget* Tab() const;
	QMenu* 	Menu() const;
	
protected:
    QMenu	*menu;
    QTabWidget *tabW;
    
    virtual void contextMenuEvent(QContextMenuEvent *event);
    virtual bool event(QEvent *);

private:
    void allow(Qt::DockWidgetArea area, bool allow);
    void place(Qt::DockWidgetArea area, bool place);

private slots:
    void changeMovable(bool on);
    void changeFloatable(bool on);
    void changeFloating(bool on);

    void allowLeft(bool a);
    void allowRight(bool a);
    void allowTop(bool a);
    void allowBottom(bool a);

    void placeLeft(bool p);
    void placeRight(bool p);
    void placeTop(bool p);
    void placeBottom(bool p);
};


#endif
