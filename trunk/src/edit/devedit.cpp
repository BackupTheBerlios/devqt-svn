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

#include "devedit.h"
#include "devgui.h"
#include "coreedit.h"
#include "devstatus.h"
#include "devlinenumber.h"
#include "devhighlighter.h"

/*

DevEdit : THE ultimate text editor!!! ;-)

*/

DevEdit::DevEdit(QWidget *p)
 : QWidget(p)
{
	setup();
}

DevEdit::DevEdit(const QString& s, DevEdit::OpenType t, QWidget *p)
 : QWidget(p)
{
	setup();
	
	setText(s, t);
}

void DevEdit::setup()
{
	// setup widgets
	e = new CoreEdit;
	l = new DevLineNumber(e);
	
	//setup layout
	QGridLayout *grid = new QGridLayout;
	
	grid->addWidget(l,  0, 0, 48,  1);
	grid->addWidget(e,  0, 1, 48, 63);
	
	//apply layout
	setLayout(grid);
	
	//setup editor properties
	setFont( QFont(DEV_GUI->getFontFamily(), DEV_GUI->getFontSize()) );
	
	hl = new CppHighlighter(e);//, true);
	
	//setup internal connections
	connect(l	, SIGNAL( clicked(QTextCursor&) ),
			this, SLOT  ( toggleBreakPoint(QTextCursor&) ) );
	
	connect(e	, SIGNAL( message(const QString&, int) ),
			this, SIGNAL( message(const QString&, int) ) );
	
	connect(this, SIGNAL( needUndo() ),
			e	, SLOT  ( undo() ) );
	
	connect(this, SIGNAL( needRedo() ),
			e	, SLOT  ( redo() ) );
	
	//setup custom connections (external purpose but specific to DevEdit)
	
	
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
	
}

QString DevEdit::name() const
{
	return n;
}

QString DevEdit::text() const
{
	return e->toPlainText();
}

void DevEdit::setText(const QString& s, DevEdit::OpenType t)
{
	if ( t == string )
	{
		e->setPlainText(s);
	}
	else if ( t == file )
	{
		n = s;
		
		if ( !QFile::exists(s) )
			return;
		
		QFile f(s);
		
		if ( !f.open(QFile::ReadOnly) )
			return (void)QMessageBox::warning(	this,
												"Error!", 
												"Unable to read file : "
												+ n );
		
		e->setPlainText( f.readAll() );
	}
}

void DevEdit::setFont(const QFont& f)
{
	e->document()->setDefaultFont(f);
	
	//temporary tab work around
	e->setTabStopWidth(4*QFontMetrics(f).width(' '));
}

QTextDocument* DevEdit::document() const
{
	return e->document();
}

QTextCursor DevEdit::textCursor() const
{
	return e->textCursor();
}

bool DevEdit::isUndoAvailable() const
{
	return e->document()->isUndoAvailable();
}

bool DevEdit::isRedoAvailable() const
{
	return e->document()->isRedoAvailable();
}

void DevEdit::undo()
{
	emit needUndo();
}

void DevEdit::redo()
{
	emit needRedo();
}

void DevEdit::cut()
{
	e->cut();
}

void DevEdit::copy()
{
	e->copy();
}

void DevEdit::paste()
{
	e->paste();
}

void DevEdit::selectAll()
{
	e->selectAll();
}

void DevEdit::delSelect()
{
	e->textCursor().removeSelectedText();
}

void DevEdit::print()
{
	QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() == QDialog::Accepted)
	{
        e->document()->print(&printer);
    }
    delete dlg;
}

void DevEdit::breakpoint()
{
	QTextCursor c( e->textCursor() );
	toggleBreakPoint(c);
}

void DevEdit::find()
{
	//e->find();
}

void DevEdit::findNext()
{
	//e->findNext();
}

void DevEdit::replace()
{
	//e->replace();
}

void DevEdit::goTo()
{
	//e->gotoDlg->execute();
}

void DevEdit::indent()
{
	;
}

void DevEdit::scrollTo(const QString & /*txt*/, const QString & /*first*/)
{
	//e->scrollTo(txt, first);
}

void DevEdit::readSettings()
{
}

void DevEdit::toggleBreakPoint(int line)
{
	QTextCursor cur = DevQt::gotoLine(e->document(), line);
	if ( cur.isNull() )
		return;
	
	toggleBreakPoint(cur);
}

void DevEdit::toggleBreakPoint(QTextCursor& cur)
{
	QTextBlock blk 	= cur.block();
	if ( !blk.isValid() )
		return;
	
	BlockData *dat = BlockData::data(blk);
		
	if ( !dat )
	{
		dat = new BlockData;
		dat->setToBlock(blk);
	}
	dat->s ^= BlockData::BreakPoint;

	e->document()->markContentsDirty(blk.position(), blk.text().count());
	
	cur.movePosition(QTextCursor::StartOfBlock);
	e->setTextCursor(cur);
}

void DevEdit::toggleError(int line)
{
	QTextCursor cur = DevQt::gotoLine(e->document(), line);
	if ( cur.isNull() )
		return;
	toggleError(cur);
}

void DevEdit::toggleError(QTextCursor& cur)
{
	QTextBlock blk 	= cur.block();
	if ( !blk.isValid() )
		return;
	
	BlockData *dat = BlockData::data(blk);
	
	if ( !dat )
	{
		dat = new BlockData;
		dat->setToBlock(blk);
	}
	
	dat->s |= BlockData::Error;
	
	e->document()->markContentsDirty(blk.position(), blk.text().count());
	
	cur.movePosition(QTextCursor::StartOfBlock);
	e->setTextCursor(cur);
}

void DevEdit::setModified(bool b)
{
	e->document()->setModified(b);
}

bool DevEdit::isModified() const
{
	return e->document()->isModified();
}

int DevEdit::lines()
{
	return DevQt::lines(e->document());
}

int DevEdit::count()
{
	return DevQt::lines(e->document());
}

//  protected slots for signal wrapping

void DevEdit::h_actionTriggered(int action)
{
	emit actionTriggered(action, Qt::Horizontal);
}

void DevEdit::h_rangeChanged(int min, int max)
{
	emit rangeChanged(min, max, Qt::Horizontal);
}

void DevEdit::h_sliderMoved(int value)
{
	emit sliderMoved(value, Qt::Horizontal);
}

void DevEdit::h_sliderPressed()
{
	emit sliderPressed(Qt::Horizontal);
}

void DevEdit::h_sliderReleased()
{
	emit sliderReleased(Qt::Horizontal);
}

void DevEdit::h_valueChanged(int value)
{
	emit valueChanged(value, Qt::Horizontal);
}

void DevEdit::v_actionTriggered(int action)
{
	emit actionTriggered(action, Qt::Vertical);
}

void DevEdit::v_rangeChanged(int min, int max)
{
	emit rangeChanged(min, max, Qt::Vertical);
}

void DevEdit::v_sliderMoved(int value)
{
	emit sliderMoved(value, Qt::Vertical);
}

void DevEdit::v_sliderPressed()
{
	emit sliderPressed(Qt::Vertical);
}

void DevEdit::v_sliderReleased()
{
	emit sliderReleased(Qt::Vertical);
}

void DevEdit::v_valueChanged(int value)
{
	emit valueChanged(value, Qt::Vertical);
}

