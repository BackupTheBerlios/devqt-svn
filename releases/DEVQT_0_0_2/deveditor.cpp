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

#include "deveditor.h"

/*

DevEditor : wrapper of all classes above

*/

DevEditor::DevEditor(QWidget *p)
 : QWidget(p)
{
	setup();
}

DevEditor::DevEditor(const QString& s, DevEdit::OpenType t, QWidget *p)
 : QWidget(p)
{
	setup();
	
	setText(s, t);
}

void DevEditor::setup()
{
	// setup widgets
	e = new DevEdit;
	s = new DevStatus;
	l = new DevLineNumber(e);
	
	//setup layout
	QGridLayout *grid = new QGridLayout;
	
	grid->addWidget(l,  0, 0, 47,  1);
	grid->addWidget(e,  0, 1, 47, 63);
	grid->addWidget(s, 47, 0,  1, 64);
	
	//apply layout
	setLayout(grid);
	
	//setup editor properties
	setFont( QFont("Courier New", 10) );
	
	//setup internal connections
	connect(e, SIGNAL( statusChanged(const QString&) ),
			s, SLOT  ( message(const QString&) ) );
	
	connect(l, SIGNAL( clicked(QTextCursor&) ),
			e, SLOT  ( toggleBreakPoint(QTextCursor&) ) );
	
	//setup custom connections (external purpose but specific to DevEditor)
	
	
	//setup external purpose connections, i.e. signals wrappers
	connect(e	, SIGNAL( copyAvailable(bool) ), 
			this, SIGNAL( copyAvailable(bool) ) );
	connect(e	, SIGNAL( currentCharFormatChanged(const QTextCharFormat&) ), 
			this, SIGNAL( currentCharFormatChanged(const QTextCharFormat&) ) );
	connect(e	, SIGNAL( cursorPositionChanged() ), 
			this, SIGNAL( cursorPositionChanged() ) );
	connect(e	, SIGNAL( redoAvailable(bool) ), 
			this, SIGNAL( redoAvailable(bool) ) );
	connect(e	, SIGNAL( selectionChanged() ), 
			this, SIGNAL( selectionChanged() ) );
	connect(e	, SIGNAL( textChanged() ), 
			this, SIGNAL( textChanged() ) );
	connect(e	, SIGNAL( undoAvailable(bool) ), 
			this, SIGNAL( undoAvailable(bool) ) );
	
	connect(e->horizontalScrollBar(), SIGNAL( actionTriggered(int) ),
			this, SLOT( h_actionTriggered(int) ) );
	connect(e->horizontalScrollBar(), SIGNAL( rangeChanged(int, int) ),
			this, SLOT( h_rangeChanged(int, int) ) );
	connect(e->horizontalScrollBar(), SIGNAL( sliderMoved(int) ),
			this, SLOT( h_sliderMoved(int) ) );
	connect(e->horizontalScrollBar(), SIGNAL( sliderPressed() ),
			this, SLOT( h_sliderPressed() ) );
	connect(e->horizontalScrollBar(), SIGNAL( sliderReleased() ),
			this, SLOT( h_sliderReleased() ) );
	connect(e->horizontalScrollBar(), SIGNAL( valueChanged(int) ),
			this, SLOT( h_valueChanged(int) ) );
	
	connect(e->verticalScrollBar(), SIGNAL( actionTriggered(int) ),
			this, SLOT( v_actionTriggered(int) ) );
	connect(e->verticalScrollBar(), SIGNAL( rangeChanged(int, int) ),
			this, SLOT( v_rangeChanged(int, int) ) );
	connect(e->verticalScrollBar(), SIGNAL( sliderMoved(int) ),
			this, SLOT( v_sliderMoved(int) ) );
	connect(e->verticalScrollBar(), SIGNAL( sliderPressed() ),
			this, SLOT( v_sliderPressed() ) );
	connect(e->verticalScrollBar(), SIGNAL( sliderReleased() ),
			this, SLOT( v_sliderReleased() ) );
	connect(e->verticalScrollBar(), SIGNAL( valueChanged(int) ),
			this, SLOT( v_valueChanged(int) ) );
	
	connect(l	, SIGNAL( clicked(int) ),
			this, SIGNAL( clicked(int) ) );
	
	connect(s	, SIGNAL( messageChanged(const QString&) ),
			this, SIGNAL( messageChanged(const QString&) ) );
	
	//setup temporary
	qobject_cast<DevStatus*>(s)->message("Column : 0  Row : 0");
}

void DevEditor::load()
{
	e->load();
}

void DevEditor::save()
{
	e->save();
}

void DevEditor::saveAs()
{
	e->saveAs();
}

void DevEditor::print()
{
    e->print();
}

const QTextDocument* DevEditor::document() const
{
	return e->document();
}

QTextCursor DevEditor::textCursor() const
{
	return e->textCursor();
}

void DevEditor::setFont(const QFont& f)
{
	e->document()->setDefaultFont(f);
	
	//temporary tab work around
	e->setTabStopWidth(4*QFontMetrics(f).width(' '));
}

void DevEditor::setText(const QString& s, DevEdit::OpenType t)
{
	e->setText(s, t);
}

QString DevEditor::text() const
{
	return e->toPlainText();
}

QString DevEditor::name() const
{
	return e->name();
}

bool DevEditor::isUndoAvailable() const
{
	return e->document()->isUndoAvailable();
}

bool DevEditor::isRedoAvailable() const
{
	return e->document()->isRedoAvailable();
}

void DevEditor::undo()
{
	e->document()->undo();
}

void DevEditor::redo()
{
	e->document()->redo();
}

void DevEditor::cut()
{
	e->cut();
}

void DevEditor::copy()
{
	e->copy();
}

void DevEditor::paste()
{
	e->customPaste();
}

void DevEditor::selectAll()
{
	e->selectAll();
}

void DevEditor::find()
{
	e->find();
}

void DevEditor::findNext()
{
	e->findNext();
}

void DevEditor::replace()
{
	e->replace();
}

void DevEditor::gotoLine(int line)
{
	e->gotoDlg->execute();
}

void DevEditor::indent()
{
	;
}

void DevEditor::scrollTo(const QString &txt, const QString &first)
{
	e->scrollTo(txt, first);
}

void DevEditor::setContext(QObject *ctx)
{
	e->setContextObject(ctx);
}

void DevEditor::readSettings()
{
}

void DevEditor::toggleBreakPoint(int line)
{
	e->toggleBreakPoint(line);
}

void DevEditor::toggleError(int line)
{
	e->toggleError(line);
}

void DevEditor::setModified(bool b)
{
	e->document()->setModified(b);
}

bool DevEditor::isModified() const
{
	return e->document()->isModified();
}

int DevEditor::lines()
{
	return e->count();
}

int DevEditor::count()
{
	return e->count();
}

//  protected slots for signal wrapping

void DevEditor::h_actionTriggered(int action)
{
	actionTriggered(action, Qt::Horizontal);
}

void DevEditor::h_rangeChanged(int min, int max)
{
	rangeChanged(min, max, Qt::Horizontal);
}

void DevEditor::h_sliderMoved(int value)
{
	sliderMoved(value, Qt::Horizontal);
}

void DevEditor::h_sliderPressed()
{
	sliderPressed(Qt::Horizontal);
}

void DevEditor::h_sliderReleased()
{
	sliderReleased(Qt::Horizontal);
}

void DevEditor::h_valueChanged(int value)
{
	valueChanged(value, Qt::Horizontal);
}

void DevEditor::v_actionTriggered(int action)
{
	actionTriggered(action, Qt::Vertical);
}

void DevEditor::v_rangeChanged(int min, int max)
{
	rangeChanged(min, max, Qt::Vertical);
}

void DevEditor::v_sliderMoved(int value)
{
	sliderMoved(value, Qt::Vertical);
}

void DevEditor::v_sliderPressed()
{
	sliderPressed(Qt::Vertical);
}

void DevEditor::v_sliderReleased()
{
	sliderReleased(Qt::Vertical);
}

void DevEditor::v_valueChanged(int value)
{
	valueChanged(value, Qt::Vertical);
}
