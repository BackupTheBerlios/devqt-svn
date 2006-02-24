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

#include "coreedit.h"

CoreEdit::CoreEdit(QWidget *p, const QString& s)
 : QTextEdit(p)
{
	setMouseTracking(true);
	setLineWrapMode(NoWrap);
	setWordWrapMode(QTextOption::NoWrap);
	
	setPlainText(s);
	
	connect(this, SIGNAL( cursorPositionChanged() ),
			this, SLOT  ( textCursorPos() ) );
	
	connect(document()	, SIGNAL( modificationChanged(bool) ),
			this		, SLOT  ( docModified(bool) ) );
	
}

void CoreEdit::mousePressEvent(QMouseEvent *e)
{
	viewport()->update();
	QTextEdit::mousePressEvent(e);
}

void CoreEdit::mouseReleaseEvent(QMouseEvent *e)
{
	viewport()->update();
	QTextEdit::mouseReleaseEvent(e);
}

void CoreEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
	viewport()->update();
	QTextEdit::mouseDoubleClickEvent(e);
}

void CoreEdit::keyPressEvent(QKeyEvent *e)
{
	if ( e->key() == Qt::Key_Insert )
	{
		bool m = !overwriteMode();
		
		setOverwriteMode(m);
		
		if ( m )
			message("Overwrite", (int)DevQt::TypingMode);
		else
			message("Insert", (int)DevQt::TypingMode);
		
	} else {
		QTextEdit::keyPressEvent(e);
		viewport()->update();
		
		message(QString::number( DevQt::lines( document() ) ), 
				(int)DevQt::Lines);
	}
}

void CoreEdit::mouseMoveEvent(QMouseEvent *e)
{
	const QFontMetrics fm(document()->defaultFont());
	QString row, col, line;
	const QPoint p = viewport()->mapFromGlobal(e->globalPos());
	
	QTextCursor cur = cursorForPosition(p);
	
	int x, y = DevQt::line(document(), cur.block());
	
	if ( y != -1 )
	{
		line = cur.block().text();
		x = line.length()+1;
		
		do
		{
			if ( (fm.width(line, --x) - fm.charWidth(line, x)) < p.x() )
				break;
		} while ( x > 0 );
		
		col = QString::number(x);
		row = QString::number(y);
	}
	else
	{
		col = QString::number(-1);
		row = QString::number(-1);
	}
	
	message(QString("Mouse :") + " Row " + row + ", Column " + col,
			(int)DevQt::MouseCursor);
	
	if ( e->buttons() )
		viewport()->update();
	
	QTextEdit::mouseMoveEvent(e);
}

void CoreEdit::contextMenuEvent(QContextMenuEvent *e)
{
	viewport()->update();
	e->ignore();
}

void CoreEdit::paintEvent(QPaintEvent *e)
{
	QPainter p( viewport() );
	QRect r(0,
			cursorRect().y(),
			viewport()->width(),
			QFontMetrics( document()->defaultFont() ).lineSpacing() );
	
	p.fillRect(r, QColor(0x00, 0xff, 0xff, 0x30));
	p.end();
	
	QTextEdit::paintEvent(e);
}

void CoreEdit::textCursorPos()
{
	QTextCursor c = textCursor();
	QTextBlock b = c.block();
	
	int x = c.position() - b.position(), y = DevQt::line(document(), b);
	
	QString col = QString::number(x);
	QString row = QString::number(y);
	
	message(QString("Text :") + " Row " + row + ", Column " + col,
			(int)DevQt::TextCursor);
}

void CoreEdit::docModified(bool mod)
{
	if ( mod )
		message("Modified", (int)DevQt::Modification);
	else
		message("", (int)DevQt::Modification);
}

