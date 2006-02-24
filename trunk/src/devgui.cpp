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

#include "devapp.h"
#include "devgui.h"

#include "devdock.h"
#include "devedit.h"
#include "coreedit.h"
#include "devcorner.h"
#include "devstatus.h"
#include "devdialogs.h"
#include "devworkspace.h"

DevGUI* DevGUI::_gui = 0;

DevGUI* DevGUI::Instance()
{
	if ( _gui == 0 )
		_gui = new DevGUI;
	return _gui;
}

DevGUI::DevGUI()
 : QMainWindow(0), noname_count(0)
{
	Editor = new QTabWidget(this);
	Editor->setWindowState(Qt::WindowMaximized);
	Editor->setContextMenuPolicy(Qt::CustomContextMenu);
	
    setupMenu();
    
	setupFileActions();
    setupEditActions();
    //setupTextActions();
    setupCompiler();
    setupExplorer();
    
	
	connect(Editor	, SIGNAL( currentChanged(int) ),
			this	, SLOT  ( editorChanged() ) );
	connect(Editor	, SIGNAL( customContextMenuRequested(const QPoint&) ),
			this	, SLOT  ( editorMenu(const QPoint&) ) );
	
    c = new DevCorner(Editor);
	Editor->setCornerWidget(c, Qt::TopLeftCorner);
    
	connect(c	, SIGNAL( create() ),
			this, SLOT  ( fileNew() ) );
	connect(c	, SIGNAL( close() ),
			this, SLOT  ( close() ) );
	
	setCentralWidget(Editor);
	
	gDlg = new DevGotoDialog(Editor);
	fDlg = new DevFindDialog(Editor);
	rDlg = new DevReplaceDialog(Editor);
	pDlg = new DevPropertiesDialog(Editor);

	connect(QApplication::clipboard()	, SIGNAL(dataChanged()),
			this						, SLOT  (clipboardDataChanged()) );
	
	s = new DevStatus;
	setStatusBar(s);
	
	if (DEV_APP->argc() == 1)
	{
		//if (!load("deveditor.cpp"))
		//	fileNew();
	} else {
		for (int i = 1; i < DEV_APP->argc(); ++i)
			load(DEV_APP->argv()[i]);
	}
	
	
	setWindowState(Qt::WindowMaximized);
	setWindowTitle("Dev-Qt++ 0.1.0");
	show();
}

DevGUI::~DevGUI()
{
	closeAll();
}

void DevGUI::setupMenu()
{
	menu = new QMenu(Editor);
	
	QAction *a;
	
	a = aUndo		= new QAction(QIcon(":/undo.png"), "&Undo", this);
	a->setEnabled( false );
	menu->addAction(a);
	
	a = aRedo		= new QAction(QIcon(":/redo.png"), "&Redo", this);
	a->setEnabled( false );
	menu->addAction(a);
	
	menu->addSeparator();
    
	a = aCut		= new QAction(QIcon(":/cut.png"), "Cu&t", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( cut() ) );
	menu->addAction(a);
	
	a = aCopy		= new QAction(QIcon(":/copy.png"), "Cop&y", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( copy() ) );
	menu->addAction(a);
	
	a = aPaste		= new QAction(QIcon(":/paste.png"), "&Paste", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( paste() ) );
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
			this, SLOT  ( goTo() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aBreakPoint = new QAction("Toggle Brea&kpoint", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( breakpoint() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aPrint		= new QAction(QIcon(":/print.png"), "Print", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( print() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aProp		= new QAction("Pr&operties", this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( properties() ) );
	menu->addAction(a);
};

void DevGUI::setupFileActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle("File Actions");
    addToolBar(tb);

    QMenu *menu = new QMenu("&File", this);
    menuBar()->addMenu(menu);

    QAction *a;

    aNew = a = new QAction(QIcon(":/new.png"), "&New...", this);
    a->setShortcut(Qt::CTRL + Qt::Key_N);
    connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    tb->addAction(a);
    menu->addAction(a);

    aOpen = a = new QAction(QIcon(":/open.png"), "&Open...", this);
    a->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    a = aSave = new QAction(QIcon(":/save.png"), "&Save...", this);
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aSaveAll = new QAction(QIcon(":/saveall.png"), "&Save All...", this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAll()));
    tb->addAction(a);
    menu->addAction(a);

    a = aSaveAs = new QAction(QIcon(":/saveas.png"), "Save &As...", this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    a->setEnabled(false);
    menu->addAction(a);
    menu->addSeparator();

    a = aPrint;
    a->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(a, SIGNAL(triggered()), this, SLOT(print()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);

    a = aClose = new QAction(QIcon(":/close.png"), "&Close", this);
    a->setShortcut(Qt::CTRL + Qt::Key_W);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);

    aExit = a = new QAction(QIcon(":/exit.png"), "E&xit", this);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( closeAll() ) );
    connect(a	, SIGNAL( triggered() ),
			qApp, SLOT  ( quit() ) );
    menu->addAction(a);
    
    tb->setIconSize( QSize(22, 22) );
}

void DevGUI::setupEditActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle("Edit Actions");
    addToolBar(tb);

    QMenu *menu = new QMenu("&Edit", this);
    menuBar()->addMenu(menu);

    QAction *a;
    a = aUndo;
    a->setShortcut(Qt::CTRL + Qt::Key_Z);
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aRedo;
    a->setShortcut(Qt::CTRL + Qt::Key_Y);
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    menu->addSeparator();
    
    a = aCut;
    a->setShortcut(Qt::CTRL + Qt::Key_X);
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aCopy;
    a->setShortcut(Qt::CTRL + Qt::Key_C);
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aPaste;
    a->setShortcut(Qt::CTRL + Qt::Key_V);
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    tb->setIconSize( QSize(22, 22) );
}

/*
void DevGUI::setupTextActions()
{
	
	QToolBar *tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	tb->setWindowTitle("Format Actions");
	addToolBarBreak(Qt::TopToolBarArea);
	addToolBar(tb);
	
	Font = new QComboBox(tb);
	tb->addWidget(Font);
	Font->setEditable(true);
	QFontDatabase db;
	Font->addItems(db.families());
	connect(Font, SIGNAL(activated(const QString &)),
			this, SLOT(textFamily(const QString &)));
	Font->setCurrentIndex(Font->findText(DevApp::font().family()));
	
	Size = new QComboBox(tb);
	Size->setObjectName("Size");
	tb->addWidget(Size);
	Size->setEditable(true);

	foreach(int size, db.standardSizes())
		Size->addItem(QString::number(size));

	connect(Size, SIGNAL(activated(const QString &)),
			this, SLOT(textSize(const QString &)));
	Size->setCurrentIndex(Size->findText(QString::number(DevApp::font().pointSize())));
	
}*/

void DevGUI::setupExplorer()
{
	
	Explorer = new DevDock("Explorer", Qt::LeftDockWidgetArea, this);
	
	tabExplorer = Explorer->Tab();
	
	treeFiles = new DevWorkSpace("default.dws");
	tabExplorer->addTab(treeFiles, "Files");
	
	treeClasses = new QTreeWidget;
	treeClasses->setHeaderItem(new QTreeWidgetItem(QStringList("Classes : "), 
													DevQt::classes));
	tabExplorer->addTab(treeClasses, "Classes");
	
}

void DevGUI::setupCompiler()
{
	QAction *a;
	QToolBar *tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    tb->setWindowTitle("Compiler Actions");
    addToolBar(tb);
    
    QMenu *menu = new QMenu("&Compile", this);
    menuBar()->addMenu(menu);
    
    aCompile = a = new QAction(QIcon(":/compil.png"), "&Compile...", this);
    a->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_C);
    //connect(a, SIGNAL(triggered()), this, SLOT(projComp()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    aRebuild = a = new QAction(QIcon(":/rebuild.png"), "&Rebuild...", this);
    a->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_A);
    //connect(a, SIGNAL(triggered()), this, SLOT(projRebuild()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    tb->setIconSize( QSize(22, 22) );
    
	Compiler = new DevDock("Compiler", Qt::BottomDockWidgetArea, this);
	tabCompiler = Compiler->Tab();
}

bool DevGUI::load(const QString &f)
{
    if (!QFile::exists(f))
        return false;
    
    if (f.endsWith(".pro"))
    	return loadProject(f);
    
    createEditor(f);
    
    return true;
}

bool DevGUI::loadProject(const QString &f)
{
	treeFiles->addProject(f);
	return true;
}

void DevGUI::fileNew()
{
    createEditor();
}

void DevGUI::fileOpen()
{
    QStringList fl = QFileDialog::getOpenFileNames(	this,
													"Open File...",
        											QString(),
													DevQt::supportedFiles );
	
	foreach(QString fn, fl)
        load(fn);
}

void DevGUI::fileSave(const QString& n, const QString& ext)
{
	if ( !e )
		return;
	
	if ( n.isEmpty() && e->name().startsWith("noname_") )
	{
		return fileSaveAs();
	}
	
	QString name(n); //get rid of const
	
	checkExtension(name, ext);
	
	QFile f(name);
	
	if ( !f.open(QFile::WriteOnly) )
		return (void)QMessageBox::warning(this, "Error!",
										  "Unable to write in file : " + name);
	QTextStream cout(&f);
	cout<<e->text();
	e->setModified(false);
    
    e->n = name;
    Editor->setTabText( Editor->indexOf(e), name);
	
}

void DevGUI::fileSaveAll()
{
	if ( !e )
		return;
	
	DevEdit *se = e;
	
	for (int i=0; i<Editor->count(); i++)
	{
		e = qobject_cast<DevEdit*>(Editor->widget(i));
		
		if ( !e )
			return;
		
		fileSave();
	}
	e = se;
}

void DevGUI::fileSaveAs()
{
	if ( !e )
        return;
	
    QString ext, name = QFileDialog::getSaveFileName(this, "Save as...",
											QString(),
    										DevQt::supportedFiles, &ext );
    if ( name.isEmpty() )
    	return;
    
	while ( QFile::exists(name) )
	{
		int state = QMessageBox::warning(this, "Warning !", 
								"File already exists! Overwrite?",
								QMessageBox::Ok,
								QMessageBox::No | QMessageBox::Default,
								QMessageBox::Cancel | QMessageBox::Escape );
		if ( state == QMessageBox::Ok )
			break;
		
		if ( state == QMessageBox::Cancel )
			return;
		
		name = QFileDialog::getSaveFileName(this, "Save as...", QString(),
    										DevQt::supportedFiles, &ext );
	} 
	
	fileSave(name, ext);
}

void DevGUI::undo()
{
	if ( !e )
        return;
    e->undo();
}

void DevGUI::redo()
{
	if ( !e )
        return;
    e->redo();
}

void DevGUI::cut()
{
	if ( !e )
        return;
    e->cut();
}

void DevGUI::copy()
{
	if ( !e )
        return;
    e->copy();
}

void DevGUI::paste()
{
	if ( !e )
        return;
    e->paste();
}

void DevGUI::selectAll()
{
	if ( !e )
        return;
    e->selectAll();
}

void DevGUI::delSelect()
{
	if ( !e )
        return;
    e->delSelect();
}

void DevGUI::find()
{
	if ( !e )
        return;
    fDlg->execute(e->e);
}

void DevGUI::findNext()
{
	if ( !e )
        return;
    fDlg->process();
}

void DevGUI::replace()
{
	if ( !e )
        return;
    rDlg->execute(e->e);
}

void DevGUI::goTo()
{
	if ( !e )
        return;
    gDlg->execute(e->e);
}

void DevGUI::print()
{
	if ( !e )
        return;
    e->print();
}

void DevGUI::breakpoint()
{
	if ( !e )
        return;
    e->breakpoint();
}

void DevGUI::close()
{
	if ( !e )
		return;
	
	maybeSave();
	
    const int pos = Editor->indexOf(e);
	const bool hadFocus = (e && e->hasFocus());
	
	QString name = Editor->tabText(pos);
    Editor->removeTab(pos);
    
    e = qobject_cast<DevEdit*>(Editor->currentWidget());
	
    if (e && hadFocus)
        e->setFocus();
    
    if ( !e )
    {
		aSave->setEnabled(false);
    	aSaveAs->setEnabled(false);
    	aUndo->setEnabled(false);
    	aRedo->setEnabled(false);
		aCut->setEnabled(false);
    	aCopy->setEnabled(false);
    	aPaste->setEnabled(false);
    	aPrint->setEnabled(false);
		aClose->setEnabled(false);
		
		for ( int p = DevQt::General; p < DevQt::None; p++ )
			s->message(QString(), p);
	}
}

void DevGUI::closeAll()
{
	while ( e )
		close();
}

void DevGUI::properties()
{
	if ( e )
		pDlg->execute(e->name());
}

void DevGUI::clipboardDataChanged()
{
    aPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

/*

TODO : make custom styling look nicer!


void DevGUI::textFamily(const QString &f)
{
    if ( !e )
        return;
    e->setFont( QFont( f, Size->currentText().toInt() ) );
}

void DevGUI::textSize(const QString &p)
{
    if ( !e )
        return;
    e->setFont( QFont( Font->currentText(), p.toInt() ) );
}

void DevGUI::fontChanged(const QFont &f)
{
    Font->setCurrentIndex(Font->findText(f.family()));
    Size->setCurrentIndex(Size->findText(QString::number(f.pointSize())));
}
*/

void DevGUI::editorChanged()
{
	QTextDocument *doc;
	
    if ( e )
	{
		
		doc = e->document();
		
		disconnect(doc, SIGNAL(modificationChanged(bool)),
					aSave, SLOT(setEnabled(bool)));
		disconnect(doc, SIGNAL(modificationChanged(bool)),
					this, SLOT(setWindowModified(bool)));
		disconnect(doc, SIGNAL(undoAvailable(bool)),
					aUndo, SLOT(setEnabled(bool)));
		disconnect(doc, SIGNAL(redoAvailable(bool)),
					aRedo, SLOT(setEnabled(bool)));

		disconnect(	aUndo, SIGNAL(triggered()),
					doc, SLOT(undo()));
        disconnect(	aRedo, SIGNAL(triggered()),
					doc, SLOT(redo()));

		disconnect(	aCut, SIGNAL(triggered()),
					e	, SLOT(cut()));
		disconnect(	aCopy, SIGNAL(triggered()),
					e, SLOT(copy()));
		disconnect(	aPaste, SIGNAL(triggered()),
					e, SLOT(paste()));
		
		disconnect(e, SIGNAL(copyAvailable(bool)), aCut, SLOT(setEnabled(bool)));
		disconnect(e, SIGNAL(copyAvailable(bool)), aCopy, SLOT(setEnabled(bool)));
		
		disconnect(e, SIGNAL( message(const QString&, DevQt::StatusPurpose p) ),
					s,SLOT  ( message(const QString&, DevQt::StatusPurpose p) ) );
    }

    e = qobject_cast<DevEdit*>(Editor->currentWidget());
    if ( !e )
    {
		aSave->setEnabled(false);
    	aSaveAs->setEnabled(false);
    	aUndo->setEnabled(false);
    	aRedo->setEnabled(false);
		aCut->setEnabled(false);
    	aCopy->setEnabled(false);
    	aPaste->setEnabled(false);
    	aPrint->setEnabled(false);
		aClose->setEnabled(false);
        return;
	}
    
	doc = e->document();
	
    //fontChanged(doc->defaultFont());

    connect(doc, SIGNAL(modificationChanged(bool)),
            aSave, SLOT(setEnabled(bool)));
    
    connect(doc	, SIGNAL(modificationChanged(bool)),
            this, SLOT(editorModified(bool)));
    
    connect(doc, SIGNAL(undoAvailable(bool)),
            aUndo, SLOT(setEnabled(bool)));
    connect(doc, SIGNAL(redoAvailable(bool)),
            aRedo, SLOT(setEnabled(bool)));

    e->setWindowModified(e->isModified());
    aSave->setEnabled(doc->isModified());
    aUndo->setEnabled(doc->isUndoAvailable());
    aRedo->setEnabled(doc->isRedoAvailable());
    
    aPrint->setEnabled(true);
	aClose->setEnabled(true);
    aSaveAs->setEnabled(true);

    connect(aUndo, SIGNAL( triggered() ), e, SLOT( undo() ) );
    connect(aRedo, SIGNAL( triggered() ), e, SLOT( redo() ) );

    const bool selection = e->textCursor().hasSelection();
    aCut->setEnabled(selection);
    aCopy->setEnabled(selection);
    
    aPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());

    connect(aCut, SIGNAL(triggered()), e, SLOT(cut()));
    connect(aCopy, SIGNAL(triggered()), e, SLOT(copy()));
    connect(aPaste, SIGNAL(triggered()), e, SLOT(paste()));

    connect(e, SIGNAL(copyAvailable(bool)), aCut, SLOT(setEnabled(bool)));
    connect(e, SIGNAL(copyAvailable(bool)), aCopy, SLOT(setEnabled(bool)));
    
    connect(e, SIGNAL( message(const QString&, int) ),
			s, SLOT  ( message(const QString&, int) ) );
	
    s->message("Text : Row 0, Column 0", (int)DevQt::TextCursor);
	s->message("Mouse : Row 0, Column 0", (int)DevQt::MouseCursor);
	s->message("Insert", (int)DevQt::TypingMode);
	s->message(QString::number(DevQt::lines(e->document())), (int)DevQt::Lines);
	
	e->e->setFocus();
}

DevEdit* DevGUI::createEditor(const QString &title, bool show)
{
	DevEdit *edit;
	QString stitle;
	
	if ( title.isEmpty() )
		stitle = "noname_" + QString::number(++noname_count, 10);
	else
		stitle = title;
    
    edit = new DevEdit(stitle, DevEdit::file);
	
	if ( !show )
		return edit;
	
    int tab = Editor->addTab(edit, QFileInfo(stitle).fileName());
    Editor->setCurrentIndex(tab);
    Editor->show();
    
    return edit;
}

void DevGUI::editorMenu(const QPoint& pos)
{
	if ( !e )
		return;
	
	menu->exec(Editor->mapToGlobal(pos));
}

void DevGUI::editorModified(bool mod)
{
	if ( !e )
		return;
	
	const int i = Editor->indexOf(e);
	QString n = Editor->tabText(i);
	const bool end = n.endsWith(" [*]");
	
	if ( mod )
	{
		if ( !end )
			n += " [*]";
	} else {
		if ( end )
			n = n.left( n.length()-4 );
	}
	
	Editor->setTabText(i, n);
}

void DevGUI::maybeSave()
{
	if ( !e->isModified() )
		return;
	
	int s = QMessageBox::warning(this, "Warning",
								"Document has been modified.\nDo you want to save it?",
								QMessageBox::Yes | QMessageBox::Default,
								QMessageBox::No | QMessageBox::Escape );
	if ( s == QMessageBox::Yes )
		fileSave();
	
}

void DevGUI::checkExtension(QString& f, const QString& ext)
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

void DevGUI::closeEvent(QCloseEvent *e)
{
	closeAll();
	e->accept();
}
