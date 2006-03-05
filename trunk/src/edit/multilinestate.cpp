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

#include "multilinestate.h"

#include "coreedit.h"
#include "normalstate.h"

MultilineState* MultilineState::inst = 0;

MultilineState* MultilineState::Instance()
{
	if ( !inst )
		inst = new MultilineState;
	
	return inst;
}

MultilineState::MultilineState()
{
	s = Multiline;
}

MultilineState::~MultilineState()
{
}

QString MultilineState::name()
{
	return tr("multiline edition");
}

void MultilineState::paintEvent(CoreEdit *ctxt, QPaintEvent *e)
{
	NormalState::paintEvent(ctxt, e);
}

void MultilineState::keyPressEvent(CoreEdit *ctxt, QKeyEvent *e)
{
	if ( e->modifiers() & Qt::ControlModifier )
	{
		switch ( e->key() )
		{
			case Qt::Key_Slash :
				return ctxt->changeState( NormalState::Instance() );
			
			case Qt::Key_Asterisk :
				//mixed state not implemented : disallow
				return;
			
			default:
				break;
		}
	}
	
	NormalState::keyPressEvent(ctxt, e);
}

void MultilineState::paintSelection(CoreEdit *e, QPainter &p,
					int xOffset, int yOffset,
					QAbstractTextDocumentLayout::PaintContext &cxt)
{
	EditorState::paintSelection(e, p, xOffset, yOffset, e->persistent, cxt);
}
