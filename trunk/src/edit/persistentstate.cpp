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

#include "persistentstate.h"

#include "coreedit.h"
#include "normalstate.h"

PersistentState* PersistentState::inst = 0;

PersistentState* PersistentState::Instance()
{
	if ( !inst )
		inst = new PersistentState;
	
	return inst;
}

PersistentState::PersistentState()
{
	s = Persistent;
}

PersistentState::~PersistentState()
{
}

QString PersistentState::name()
{
	return tr("persistent selection");
}

void PersistentState::paintEvent(CoreEdit *ctxt, QPaintEvent *e)
{
	NormalState::paintEvent(ctxt, e);
}

void PersistentState::keyPressEvent(CoreEdit *ctxt, QKeyEvent *e)
{
	if ( e->modifiers() & Qt::ControlModifier )
	{
		switch ( e->key() )
		{
			case Qt::Key_Slash :
				//mixed state not implemented : disallow
				return;
			
			case Qt::Key_Asterisk :
				return ctxt->changeState( NormalState::Instance() );
			
			default:
				break;
		}
	}
	
	if ( ctxt->textCursor().hasSelection() )
	{
		QTextCursor cursor( ctxt->textCursor() );
		cursor.clearSelection();
		ctxt->setTextCursor(cursor);
	}
	
	int oldPos		= ctxt->persistent.position();
	int oldAnchor   = ctxt->persistent.anchor();
	
	NormalState::keyPressEvent(ctxt, e);
	
	int newPos = ctxt->textCursor().position();
	
	if ( (newPos > oldPos) && (newPos > oldAnchor) && 
		 (	(ctxt->persistent.position() > oldPos) ||
			(ctxt->persistent.anchor() > oldAnchor) ) )
	{
		ctxt->persistent.setPosition(oldAnchor);
		ctxt->persistent.setPosition(oldPos, QTextCursor::KeepAnchor);
	}
	
	emit ctxt->copyAvailable( ctxt->persistent.hasSelection() );
	
	ctxt->viewport()->update();
}

void PersistentState::mouseMoveEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	NormalState::mouseMoveEvent(ctxt, e);
	
	if ( ctxt->textCursor().hasSelection() && e->buttons() )
		ctxt->persistent = ctxt->textCursor();
}

void PersistentState::mousePressEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	backup = ctxt->textCursor();
	anchor = ctxt->persistent;
	
	NormalState::mousePressEvent(ctxt, e);
}

void PersistentState::mouseReleaseEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	if ( anchor != ctxt->persistent )
	{
		int hval = ctxt->horizontalScrollBar()->value(),
			vval = ctxt->verticalScrollBar()->value();
		
		ctxt->setTextCursor(backup);
		
		ctxt->horizontalScrollBar()->setValue(hval);
		ctxt->verticalScrollBar()->setValue(vval);
		
		backup = anchor = QTextCursor();
	}
		
	NormalState::mouseReleaseEvent(ctxt, e);
}

void PersistentState::mouseDoubleClickEvent(CoreEdit *ctxt, QMouseEvent *e)
{
	NormalState::mouseDoubleClickEvent(ctxt, e);
}

QMimeData* PersistentState::createMimeDataFromSelection(const CoreEdit *ctxt) const
{
	QMimeData *md = new QMimeData;
	
	md->setText(ctxt->persistent.selectedText());
	
	return md;
}

void PersistentState::paintSelection(CoreEdit *e, QPainter &p,
					int xOffset, int yOffset,
					QAbstractTextDocumentLayout::PaintContext &cxt)
{
	EditorState::paintSelection(e, p, xOffset, yOffset, e->persistent, cxt);
}
