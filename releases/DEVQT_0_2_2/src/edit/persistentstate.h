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

#ifndef _PERSISTENT_STATE_H_
#define _PERSISTENT_STATE_H_

#include "normalstate.h"

class PersistentState : public NormalState
{
    Q_OBJECT
    
	public:
		static PersistentState* Instance();
		
		virtual QString name();
		
		virtual void paintEvent(CoreEdit *ctxt, QPaintEvent *e);
		virtual void keyPressEvent(CoreEdit *ctxt, QKeyEvent *e);
		
		virtual void mouseMoveEvent(CoreEdit *ctxt, QMouseEvent *e);
		virtual void mousePressEvent(CoreEdit *ctxt, QMouseEvent *e);
		virtual void mouseReleaseEvent(CoreEdit *ctxt, QMouseEvent *e);
		virtual void mouseDoubleClickEvent(CoreEdit *ctxt, QMouseEvent *e);
		
		virtual QMimeData* createMimeDataFromSelection(const CoreEdit *ctxt) const;
		
	protected:
		PersistentState();
		virtual ~PersistentState();
		
		virtual void paintSelection(CoreEdit *ctxt, QPainter& p,
								int xOffset, int yOffset,
								QAbstractTextDocumentLayout::PaintContext &cxt);
		
	private:
		static PersistentState *inst;
		
		QTextCursor backup, anchor;
};

#endif
