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

/*

DevEdit : the editor itself with custom context menu, indentation...

*/

DevEdit::DevEdit(QWidget *parent)
 : QTextEdit(parent)
{
	//disable wrapping!!!
	setLineWrapMode(NoWrap);
	
	//setup default syntax highlighter
	hl = new CppHighlighter(this, true);
	
	//cursor stuffs
	prev = textCursor();
	connect(document()	, SIGNAL( cursorPositionChanged(const QTextCursor&) ),
			this		, SLOT( highlight(const QTextCursor&) ) );
	
	//setup utility dialogs
	gotoDlg		= new DevGotoDialog(this);
	findDlg 	= new DevFindDialog(this);
	replaceDlg 	= new DevReplaceDialog(this);
    
	connect(gotoDlg	, SIGNAL( color(QTextCursor&) ),
			this	, SLOT  ( highlight(const QTextCursor&) ) );
	connect(findDlg	, SIGNAL( color(QTextCursor&) ),
			this	, SLOT  ( highlight(const QTextCursor&) ) );
	connect(replaceDlg	, SIGNAL( color(QTextCursor&) ),
			this		, SLOT  ( highlight(const QTextCursor&) ) );
	
	//setup context menu
	menu = new QMenu(this);
	
	QAction *a;
	
	a = aUndo		= new QAction("&Undo", this);
	connect(a			, SIGNAL( triggered() ),
			document()	, SLOT( undo() ) );
	connect(document()	, SIGNAL( undoAvailable(bool) ),
			a			, SLOT( setEnabled(bool) ) );
	a->setEnabled( document()->isUndoAvailable() );
	menu->addAction(a);
	
	a = aRedo		= new QAction("&Redo", this);
	connect(a			, SIGNAL( triggered() ),
			document()	, SLOT( redo() ) );
	connect(document()	, SIGNAL( redoAvailable(bool) ),
			a			, SLOT( setEnabled(bool) ) );
	a->setEnabled( document()->isRedoAvailable() );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aCut		= new QAction("Cu&t", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( cut() ) );
	menu->addAction(a);
	
	a = aCopy		= new QAction("Cop&y", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( copy() ) );
	menu->addAction(a);
	
	a = aPaste		= new QAction("&Paste", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( customPaste() ) );
	menu->addAction(a);
	
	a = aSelectAll	= new QAction("&Select All", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( selectAll() ) );
	menu->addAction(a);
	
	a = aDelete	= new QAction("&Delete", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( delSelect() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aFind = new QAction("&Find", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( find() ) );
	menu->addAction(a);
	
	a = aFindNext = new QAction("Find &Next", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( findNext() ) );
	menu->addAction(a);
	
	a = aReplace = new QAction("R&eplace", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( replace() ) );
	menu->addAction(a);
	
	a = aGoto = new QAction("&Goto", this);
	connect(a		, SIGNAL( triggered() ),
			gotoDlg , SLOT  ( execute() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aBreakPoint = new QAction("Toggle Brea&kpoint", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( toggleBPt() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aLoad		= new QAction("&Load", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( load() ) );
	menu->addAction(a);
	
	a = aSave		= new QAction("&Save", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( save() ) );
	menu->addAction(a);
	
	a = aSaveAs		= new QAction("Save as", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( saveAs() ) );
	menu->addAction(a);
	
	a = aPrint		= new QAction("Print", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( print() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aProp		= new QAction("Pr&operties", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( properties() ) );
	menu->addAction(a);
    
	//allow mouse tracking for statusbar maintenance
	setMouseTracking(true);
	
	//scrollbars stuffs (QTextEdit seem to forget adjusting them sometimes)
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

DevEdit::~DevEdit()
{
	delete hl;
	delete menu;
}

QString DevEdit::name() const
{
	return _name;
}

void DevEdit::load()
{
	if ( document()->isModified() )
		save();
	
	QString f = QFileDialog::getOpenFileName(	this,
												"Open File...",
												QString(),
												DevQt::supportedFiles );
	
    if ( !f.isEmpty() )
        setText(f, fromFile);
}

void DevEdit::save()
{
	QString ext;
	
	if ( QFile::exists(_name) )
	{
		int state = QMessageBox::warning(this,
									"Warning !", 
									"File already exists! Overwrite?",
									QMessageBox::Ok,
									QMessageBox::No,
									QMessageBox::Cancel );
		if ( state == QMessageBox::Cancel )
			return;
		
		if ( state == QMessageBox::No )
			_name = QFileDialog::getSaveFileName( 	this,
													"Save as...",
													_name, 
													DevQt::supportedFiles,
													&ext);
	} else if ( _name.startsWith("noname_") ) {
		return saveAs();
	}
	
	if ( _name.isEmpty() )
		return;
	
	checkExtension(_name, ext);
	
	QFile f(_name);
	
	if ( !f.open(QFile::WriteOnly) )
		return (void)QMessageBox::warning(this, "Error!",
										  "Unable to write in file : "+_name );
	QTextStream cout(&f);
	cout<<document()->toPlainText();
	document()->setModified(false);
}

void DevEdit::saveAs()
{
	_name = QFileDialog::getSaveFileName( 	this, "Save as...",
        									_name, DevQt::supportedFiles);
	save();
}

void DevEdit::print()
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setFullPage(true);

    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() == QDialog::Accepted)
	{
        document()->print(&printer);
    }
    delete dlg;
}

void DevEdit::setText(const QString& s, OpenType t)
{
	if ( t == fromString )
	{
		setPlainText(s);
	}
	else if ( t == fromFile )
	{
		_name = s;
		
		if ( !QFile::exists(s) )
			return;
		
		QFile f(s);
		
		if ( !f.open(QFile::ReadOnly) )
			return (void)QMessageBox::warning(	this,
												"Error!", 
												"Unable to read file : "
												+ _name );
		
		setPlainText( f.readAll() );
	}
}

void DevEdit::checkExtension(QString& f, QString& ext)
{
	int count=1;
	QString e, fn = f.section('/', -1);
	if ( !fn.contains('.') )
	{
		while (1)
		{
            e = ext.section(" *", count, count);
            count++;
			if ( e.isNull() )
			    break;
            if ( e.endsWith(')') )
                e.resize(e.size()-1);
			if ( fn.contains(e) )
				return;
		}
		f += ext.section(" *", 1, 1);
	}
}

int DevEdit::line(const QTextBlock& b)
{
	return DevQt::line(document(), b);
}

int DevEdit::count()
{
	return DevQt::lines(document());
}

QObject* DevEdit::contextObject() const
{
	return context;
}

void DevEdit::setContextObject(QObject *o)
{
	context = o;
}

void DevEdit::toggleBPt()
{
	QTextCursor c( textCursor() );
	toggleBreakPoint(c);
}

void DevEdit::toggleBreakPoint(int line)
{
	QTextCursor cur = DevQt::gotoLine(document(), line);
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

	document()->markContentsDirty(blk.position(), blk.text().count());
	
	cur.movePosition(QTextCursor::StartOfBlock);
	setTextCursor(cur);
}

void DevEdit::toggleError(int line)
{
	QTextCursor cur = DevQt::gotoLine(document(), line);
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
	
	document()->markContentsDirty(blk.position(), blk.text().count());
	
	cur.movePosition(QTextCursor::StartOfBlock);
	setTextCursor(cur);
}

void DevEdit::clearError()
{
	BlockData *dat;
	
	for (QTextBlock blk = document()->begin();
		blk.isValid(); blk = blk.next())
	{
		if ( !(dat = BlockData::data(blk)) )
			continue;
		
		if ( dat->s & BlockData::Error )
		{
			dat->s &= ~BlockData::Error;
			document()->markContentsDirty(blk.position(), blk.text().count());
		}
	}
}

void DevEdit::customPaste()
{
	QTextCursor cur, qcur = cur = textCursor();
	QString		txt = QApplication::clipboard()->text();

	cur.insertText(txt);
	
	qcur.movePosition(QTextCursor::EndOfBlock, QTextCursor::KeepAnchor);
	document()->markContentsDirty(qcur.selectionStart(), txt.count());
	
	setTextCursor(qcur);
}

void DevEdit::delSelect()
{
	textCursor().removeSelectedText();
}

void DevEdit::properties()
{
}

void DevEdit::find()
{
	findDlg->execute();
}

void DevEdit::findNext()
{
	findDlg->process();
}

void DevEdit::replace()
{
	replaceDlg->execute();
}

void DevEdit::indent()
{
}

void DevEdit::scrollTo(const QString &txt, const QString &first)
{
}

void DevEdit::highlight()
{
	viewport()->update();
}

/*

custom events handlers

*/

void DevEdit::contextMenuEvent(QContextMenuEvent *e)
{
	e->accept();
	
	highlight();
	
	menu->exec(e->globalPos());
}

void DevEdit::mouseMoveEvent(QMouseEvent *e)
{
	const QFontMetrics fm(document()->defaultFont());
	QString col("  Column : "), row("  Row : "), line;
	const QPoint p = viewport()->mapFromGlobal(e->globalPos());
	
	QTextCursor cur = cursorForPosition(p);
	
	int x, y = this->line(cur.block());
	
	if ( y != -1 )
	{
		line = cur.block().text();
		x = line.length()+1;
		
		do
		{
			if ( (fm.width(line, --x) - fm.charWidth(line, x)) < p.x() )
				break;
		} while ( x > 0 );
		
		col += QString::number(x);
		row += QString::number(y);
	}
	else
	{
		col += QString::number(-1);
		row += QString::number(-1);
	}
	
	statusChanged( col + row );
	
	if ( e->buttons() != Qt::NoButton )
	{
		highlight();
	}
	
	QTextEdit::mouseMoveEvent(e);
}


void DevEdit::mousePressEvent(QMouseEvent *e)
{
	highlight();
	
	QTextEdit::mousePressEvent(e);
}

void DevEdit::mouseReleaseEvent(QMouseEvent *e)
{
	highlight();
	
	QTextEdit::mouseReleaseEvent(e);
}

void DevEdit::mouseDoubleClickEvent(QMouseEvent *e)
{
	highlight();
	
	QTextEdit::mouseDoubleClickEvent(e);
}

void DevEdit::keyPressEvent(QKeyEvent *e)
{
	QTextEdit::keyPressEvent(e);
	
	highlight();
}

void DevEdit::paintEvent(QPaintEvent *event)
{
	QPainter painter(viewport());
	QRect r(0,
			cursorRect().y(),
			viewport()->width(),
			QFontMetrics(document()->defaultFont()).lineSpacing()
			);
	painter.fillRect(r, QColor(0x00, 0xff, 0xff, 0x30));
	painter.end();
	
	QTextEdit::paintEvent(event);
}
