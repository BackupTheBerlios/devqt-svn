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

#include "matcher.h"
#include "indenter.h"
#include "normalstate.h"
#include "persistentstate.h"

CoreEdit::CoreEdit(QWidget *p, const QString& s)
 : 	QTextEdit(p),
 	pState( NormalState::Instance() ), pMatcher( new ParenMatcher(this) ),
	pIndenter( new Indenter(this) ), iTab(4), iMargin(80), bCursor(false),
	bLine(true), bBlock(false), bIndent(true), bMatchParen(true),
	bMargin(true), bTab(false), bAutoClose(false), bNavCTRL(true)
{
	setMouseTracking(true);
	setLineWrapMode(NoWrap);
	setAcceptRichText(false);
	setFrameStyle(QFrame::NoFrame);
	setWordWrapMode(QTextOption::NoWrap);
	
	setPlainText(s);
	
	connect(this, SIGNAL( cursorPositionChanged() ),
			this, SLOT  ( textCursorPos() ) );
	
	connect(document()	, SIGNAL( modificationChanged(bool) ),
			this		, SLOT  ( docModified(bool) ) );
	
	connect(pState	, SIGNAL( message(const QString&, int) ),
			this	, SIGNAL( message(const QString&, int) ));
	
	blinker.start(QApplication::cursorFlashTime() / 2, this);
}

int CoreEdit::line(const QTextCursor& c)
{
	return DevQt::line(document(), c.block());
}

int CoreEdit::column(const QTextCursor& c)
{
	QTextBlock b = c.block();
	int n = 0, len = c.position() - b.position();
	
	if ( !b.isValid() )
		return -1;
	
	QString s = b.text();
	
	if ( !s.contains("\t") )
		return len + 1;
	
	for ( int i = 0; i < len; i++ )
	{
		if ( s.at(i) == '\t' )
			n = ( ( n / iTab ) + 1 ) * iTab;
		else
			n++;
	}
	
	return n + 1;
}

void CoreEdit::gotoLine(int row, int col)
{
	;
}

bool CoreEdit::autoIndent() const
{
	return bIndent;
}

void CoreEdit::setAutoIndent(bool y)
{
	bIndent = y;
}

bool CoreEdit::parenMatching() const
{
	return bMatchParen;
}

void CoreEdit::setParenMatching(bool y)
{
	bMatchParen = y;
}

bool CoreEdit::drawMargin() const
{
	return bMargin;
}

void CoreEdit::setDrawMargin(bool y)
{
	bMargin = y;
}

int CoreEdit::margin() const
{
	return iMargin;
}

void CoreEdit::setMargin(int n)
{
	iMargin = n;
}

bool CoreEdit::CtrlNavigation() const
{
	return bNavCTRL;
}

void CoreEdit::setCtrlNavigation(bool y)
{
	bNavCTRL = y;
}

bool CoreEdit::autoClose() const
{
	return bAutoClose;
}

void CoreEdit::setAutoClose(bool y)
{
	bAutoClose = y;
}

QFont CoreEdit::font() const
{
	return document()->defaultFont();
}

void CoreEdit::setFont(const QFont& f)
{
	//QTextEdit::setFont(f);
	document()->setDefaultFont(f);
	
	setTabStopWidth(iTab*QFontMetrics(f).width(' '));
}

int CoreEdit::tabstop() const
{
	return iTab;
}

void CoreEdit::setTabstop(int n)
{
	iTab = n;
	
	setTabStopWidth(n*QFontMetrics(document()->defaultFont()).width(' '));
}

bool CoreEdit::replaceTab() const
{
	return bTab;
}

void CoreEdit::setReplaceTab(bool y)
{
	bTab = y;
}

bool CoreEdit::highlightCurrentLine() const
{
	return bLine;
}

void CoreEdit::setHighlightCurrentLine(bool y)
{
	bLine = y;
}

bool CoreEdit::highlightCurrentBlock() const
{
	return bBlock;
}

void CoreEdit::setHighlightCurrentBlock(bool y)
{
	bBlock = y;
}

EditorState* CoreEdit::state() const
{
	return pState;
}

void CoreEdit::setState(EditorState *s)
{
	pState = s;
}

void CoreEdit::paintEvent(QPaintEvent *e)
{
	e->accept();
	pState->paintEvent(this, e);
}

void CoreEdit::timerEvent(QTimerEvent *e)
{
	pState->timerEvent(this, e);
}

void CoreEdit::keyPressEvent(QKeyEvent *e)
{
	pState->keyPressEvent(this, e);
}

void CoreEdit::mouseMoveEvent(QMouseEvent *e)
{
	pState->mouseMoveEvent(this, e);
}

void CoreEdit::mousePressEvent(QMouseEvent *e)
{
	pState->mousePressEvent(this, e);
}

void CoreEdit::mouseReleaseEvent(QMouseEvent *e)
{
	pState->mouseReleaseEvent(this, e);
}

void CoreEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
	pState->mouseDoubleClickEvent(this, e);
}

void CoreEdit::dragEnterEvent(QDragEnterEvent *e)
{
	pState->dragEnterEvent(this, e);
}

void CoreEdit::dragLeaveEvent(QDragLeaveEvent *e)
{
	pState->dragLeaveEvent(this, e);
}

void CoreEdit::dragMoveEvent(QDragMoveEvent *e)
{
	pState->dragMoveEvent(this, e);
}

void CoreEdit::dropEvent(QDropEvent *e)
{
	pState->dropEvent(this, e);
}

void CoreEdit::contextMenuEvent(QContextMenuEvent *e)
{
	viewport()->update();
	e->ignore();
}

QMimeData* CoreEdit::createMimeDataFromSelection() const
{
	return pState->createMimeDataFromSelection(this);
}

void CoreEdit::insertFromMimeData(const QMimeData *source)
{
	return pState->insertFromMimeData(this, source);
}

void CoreEdit::delSelect()
{
	textCursor().removeSelectedText();
}

void CoreEdit::changeState(EditorState *s)
{
	if ( !s )
		return;
	
	pState->disconnect(this);
	pState = s;
	
	connect(s	, SIGNAL( message(const QString&, int) ),
			this, SIGNAL( message(const QString&, int) ));
	
	emit message(tr("Switched to ") + s->name() + "...", (int)DevQt::General);
}

void CoreEdit::textCursorPos()
{
	pState->signalTextCursor(this);
}

void CoreEdit::docModified(bool mod)
{
	if ( mod )
		emit message("Modified", (int)DevQt::Modification);
	else
		emit message("", (int)DevQt::Modification);
}
