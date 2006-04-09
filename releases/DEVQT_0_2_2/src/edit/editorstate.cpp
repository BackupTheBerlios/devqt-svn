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

#include "editorstate.h"

#include "coreedit.h"

EditorState::EditorState()
 : QObject(0)
{
	// dummy constructor
}

EditorState::~EditorState()
{
	// dummy destructor
}

bool EditorState::persistentSelection()
{
	return (bool)(s & Persistent);
}

bool EditorState::multilineEdition()
{
	return (bool)(s & Multiline);
}

void EditorState::paintSelection(CoreEdit *e, QPainter& p,
							int xOffset, int yOffset, QTextCursor cursor,
							QAbstractTextDocumentLayout::PaintContext& cxt)
{
	if ( !cursor.hasSelection() )
		return;
		
	QRect toPaint;
	bool notSingleSelection;
	QRect firstRect, lastRect;
	QPalette palette = e->palette();
	QTextDocument *doc = e->document();
	QTextCursor first(doc), last(doc);
	
	QAbstractTextDocumentLayout::Selection selection;
	selection.cursor = cursor;
	selection.format.setBackground(palette.brush(QPalette::Highlight));
	selection.format.setForeground(palette.brush(QPalette::HighlightedText));
	
	int  start      = cursor.selectionStart();
	int  end        = cursor.selectionEnd();
	int  lineHeight = QFontMetrics(e->document()->defaultFont()).lineSpacing();

	first.setPosition(start);
	firstRect = e->cursorRect(first);
	firstRect.translate(xOffset, yOffset);
	last.setPosition(end);
	lastRect = e->cursorRect(last);
	lastRect.translate(xOffset, yOffset);
	
	notSingleSelection = (firstRect.unite(lastRect).height() >= (2*lineHeight));
	
	if ( notSingleSelection )
	{
		toPaint = firstRect;
		toPaint.setHeight(lineHeight);
		toPaint.setWidth(e->viewport()->width() - toPaint.x());
		p.fillRect(toPaint, palette.brush(QPalette::Highlight));
	
		for (	int y = firstRect.y() + lineHeight;
				y < (lastRect.y() - lineHeight);
				y += lineHeight)
		{
			toPaint = QRect(0, y, e->viewport()->width(), lineHeight);
			p.fillRect(toPaint, palette.brush(QPalette::Highlight));
		}
	
		toPaint = lastRect;
		toPaint.setHeight(lineHeight);
		toPaint.setX(0);
		toPaint.setWidth(lastRect.x());
		p.fillRect(toPaint, palette.brush(QPalette::Highlight));
	}
	
	cxt.selections.append(selection);
	
}

