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

#ifndef _CORE_EDIT_H_
#define _CORE_EDIT_H_

#include "dev.h"

class QColor;

class CoreEdit : public QTextEdit
{
	Q_OBJECT
	
	public:
		CoreEdit(QWidget *p = 0, const QString& s = QString());
	
	signals:
		//status bar signals
		void message(const QString& msg, int p);
		
	protected slots:
		void textCursorPos();
		void docModified(bool mod);
		void setMarkCurrentLine( bool enable );
		void setCurrentLineColor( QColor color );
		
	protected:
		virtual void paintEvent(QPaintEvent *e);
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void contextMenuEvent(QContextMenuEvent *e);
		
		bool markCurrentLine;
		QColor currentLineColor;
};

#endif
