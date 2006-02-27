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
#include "devstatus.h"
#include "devdialogs.h"
#include "devsettings.h"
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
	setFocusPolicy(Qt::StrongFocus);
	
	Editor = new QTabWidget(this);
	Editor->setWindowState(Qt::WindowMaximized);
	Editor->setContextMenuPolicy(Qt::CustomContextMenu);
	
	connect(Editor	, SIGNAL( currentChanged(int) ),
			this	, SLOT  ( editorChanged() ) );
	connect(Editor	, SIGNAL( customContextMenuRequested(const QPoint&) ),
			this	, SLOT  ( editorMenu(const QPoint&) ) );
	
	add = new QToolButton(Editor);
	add->setIcon(QIcon(":/addtab.png"));
	add->setFixedSize(22, 22);
	
	rem = new QToolButton(Editor);
	rem->setIcon(QIcon(":/close.png"));
	rem->setFixedSize(22, 22);
	
	connect(add	, SIGNAL( clicked() ),
			this, SLOT  ( fileNew() ) );
	
	connect(rem	, SIGNAL( clicked() ),
			this, SLOT  ( close() ) );
	
	Editor->setCornerWidget(add, Qt::TopLeftCorner);
	Editor->setCornerWidget(rem, Qt::TopRightCorner);
	
	setCentralWidget(Editor);
	
	gDlg = new DevGotoDialog(Editor);
	fDlg = new DevFindDialog(Editor);
	aDlg = new DevAboutDialog(Editor);
	rDlg = new DevReplaceDialog(Editor);
	pDlg = new DevPropertiesDialog(Editor);

	connect(QApplication::clipboard()	, SIGNAL(dataChanged()),
			this, SLOT  (clipboardDataChanged()) );
	
	s = new DevStatus;
	setStatusBar(s);
	
	setupMenu();
    
	setupFileActions();
	setupEditActions();
	setupSearchActions();
	setupProjectActions();
	
	setupExplorer();
	setupCompiler();
	
	setupSettings();
	setupHelpActions();
	
	QToolBar *tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	tb->setWindowTitle(tr("Projects Actions"));
	addToolBar(tb);

	tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	tb->setWindowTitle(tr("Scopes Actions"));
	addToolBarBreak(Qt::TopToolBarArea);
	addToolBar(tb);

	project = new QComboBox(tb);
	project->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	tb->addWidget(project);
	
	config = new QComboBox(tb);
	config->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	tb->addWidget(config);
	
	connect(project	, SIGNAL( currentIndexChanged(const QString&) ),
			this	, SLOT  ( changeProject(const QString&) ) );
		
	if (DEV_APP->argc() == 1)
	{
		// TODO what to open by default when no arguments have been passed to the appliation...?
		//if (!load("deveditor.cpp"))
		//	fileNew();
	} else {
		for (int i = 1; i < DEV_APP->argc(); ++i)
			load(DEV_APP->argv()[i]);
	}
	
	// TODO: session managment - restore window state
	setWindowState(Qt::WindowMaximized);
	setWindowTitle("DevQt 0.2.2");
}

DevGUI::~DevGUI()
{
	closeAll();
	delete workspace;
}

void DevGUI::setupMenu()
{
	menu = new QMenu(Editor);
	
	QAction *a;
	
	a = aUndo		= new QAction(QIcon(":/undo.png"), tr("&Undo"), this);
	a->setEnabled( false );
	menu->addAction(a);
	
	a = aRedo		= new QAction(QIcon(":/redo.png"), tr("&Redo"), this);
	a->setEnabled( false );
	menu->addAction(a);
	menu->addSeparator();
    
	a = aCut		= new QAction(QIcon(":/cut.png"), tr("Cu&t"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( cut() ) );
	menu->addAction(a);
	
	a = aCopy		= new QAction(QIcon(":/copy.png"), tr("Cop&y"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( copy() ) );
	menu->addAction(a);
	
	a = aPaste		= new QAction(QIcon(":/paste.png"), tr("&Paste"), this);
	// We're already doing this elsewhere. Having it connect twice, once to "this" and once
	// elsewhere to "e" just makes the program paste the contents of the clipboard twice
	// when user clicks the Paste button in the toolbar.
	//	connect(a    , SIGNAL( triggered() ),
	//	 this, SLOT      ( paste() ) );
	menu->addAction(a);
	
	
	a = aSelectAll	= new QAction(tr("&Select All"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( selectAll() ) );
	menu->addAction(a);
	
	a = aDelete	= new QAction(tr("&Delete"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( delSelect() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aFind = new QAction(QIcon(":/find.png"), tr("&Find"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( find() ) );
	menu->addAction(a);
	
	a = aFindNext = new QAction(QIcon(":/next.png"), tr("Find &Next"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( findNext() ) );
	menu->addAction(a);
	
	a = aReplace = new QAction(QIcon(":/replace.png"), tr("R&eplace"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( replace() ) );
	menu->addAction(a);
	
	a = aGoto = new QAction(QIcon(":/goto.png"), tr("&Goto"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( goTo() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aBreakPoint = new QAction(tr("Toggle Brea&kpoint"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( breakpoint() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aPrint		= new QAction(QIcon(":/print.png"), tr("Print"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( print() ) );
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aProp		= new QAction(tr("Pr&operties"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT	( properties() ) );
	menu->addAction(a);
};

void DevGUI::setupFileActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("File Actions"));
    addToolBar(tb);

    QMenu *sub, *menu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(menu);

    QAction *a;
	
	sub = new QMenu(tr("&New"), menu);
    
	a = aNewProject = new QAction(QIcon(":/project.png"), tr("New &project"), this);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( projectNew() ) );
    tb->addAction(a);
    sub->addAction(a);
	
	a = aNewFile = new QAction(QIcon(":/new.png"), tr("&Source file"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_N);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( fileNew() ) );
    tb->addAction(a);
    sub->addAction(a);
    
	a = aNewUI = new QAction(QIcon(":/ui.png"), tr("&User Interface"), this);
	// TODO: slot not implemented yet
	// connect(a, SIGNAL( triggered() ), this, SLOT  ( uiNew() ) );
    sub->addAction(a);
    
	a = aNewQRC = new QAction(QIcon(":/qrc.png"), tr("&Resource"), this);
	// TODO: slot not implemented yet
	// connect(a, SIGNAL( triggered() ), this, SLOT  ( qrcNew() ) );
    sub->addAction(a);
    
    sub->addSeparator();
    
	a = aNewTemplate = new QAction(QIcon(":/new.png"), tr("&Template"), this);
	// TODO: slot not implemented yet
	// connect(a, SIGNAL( triggered() ), this, SLOT(templateNew()) );
    sub->addAction(a);
    
    menu->addMenu(sub);
    
    menu->addSeparator();

    a = aOpen = new QAction(QIcon(":/open.png"), tr("&Open project or file..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( fileOpen() ) );
    tb->addAction(a);
    menu->addAction(a);
	
    menu->addMenu(DEV_SETTINGS->recent());
    
    menu->addSeparator();

    a = aSave = new QAction(QIcon(":/save.png"), tr("&Save..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( fileSave() ) );
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aSaveAll = new QAction(QIcon(":/saveall.png"), tr("Sa&ve All..."), this);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT( fileSaveAll() ) );
    tb->addAction(a);
    menu->addAction(a);

    a = aSaveAs = new QAction(QIcon(":/saveas.png"), tr("Save &As..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_F12);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( fileSaveAs() ) );
    a->setEnabled(false);
    menu->addAction(a);
    
    
    menu->addSeparator();

    a = aClose = new QAction(QIcon(":/close.png"), tr("&Close"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_F4);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT( close() ) );
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aCloseAll = new QAction(tr("C&lose all"), this);
    connect(a, SIGNAL(triggered()), this, SLOT(close()));
    a->setEnabled(false);
    menu->addAction(a);
	
    menu->addSeparator();
    
    a = aPrint;
    a->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( print() ) );
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    menu->addSeparator();
    
    aExit = a = new QAction(QIcon(":/exit.png"), tr("E&xit"), this);
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
    tb->setWindowTitle(tr("Edit Actions"));
    addToolBar(tb);

    QMenu *menu = new QMenu(tr("&Edit"), this);
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

void DevGUI::setupSearchActions()
{
	QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("Search Actions"));
    addToolBar(tb);

    QMenu *menu = new QMenu(tr("&Search"), this);
    menuBar()->addMenu(menu);

    QAction *a;

	a = aFind;
    a->setShortcut(Qt::CTRL + Qt::Key_F);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( find() ) );
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aFindNext;
    a->setShortcut(Qt::Key_F3);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( findNext() ) );
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aReplace;
    a->setShortcut(Qt::CTRL + Qt::Key_R);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( replace() ) );
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = aGoto;
    a->setShortcut(Qt::CTRL + Qt::Key_G);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( goTo() ) );
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
}


void DevGUI::setupHelpActions()
{
	QAction *a;
	QToolBar *tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    tb->setWindowTitle(tr("Help Actions"));
    addToolBar(tb);
    
    QMenu *menu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(menu);
    
    a = aAboutQt = new QAction(QIcon(":/qt.png"), tr("A&bout Qt..."), this);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( aboutQt() ) );
    tb->addAction(a);
    menu->addAction(a);
    
    a = aQtAssistant = new QAction(QIcon(":/assistant.png"), tr("Qt Assistant..."), this);
    connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( QtAssistant() ) );
    tb->addAction(a);
    menu->addAction(a);
    
    menu->addSeparator();
    
    a = aAboutDevQt = new QAction(QIcon(":/"), tr("&About DevQt..."), this);
    connect(a	, SIGNAL( triggered() ),
			aDlg, SLOT  ( exec() ) );
    tb->addAction(a);
    menu->addAction(a);
    
    a = aDevQtAssistant = new QAction(QIcon(":/"), tr("&DevQt Help..."), this);
    /*
    connect(a	, SIGNAL( triggered() ),
			hDlg, SLOT  ( exec() ));
	*/
    tb->addAction(a);
    menu->addAction(a);
    
    tb->setIconSize( QSize(22, 22) );
}

void DevGUI::setupExplorer()
{
	Explorer = new DevDock("Explorer", Qt::LeftDockWidgetArea, this);
	
	tabExplorer = Explorer->Tab();
	
	workspace = new DevWorkSpace("default.dws");
	tabExplorer->addTab(workspace, tr("Files"));
	
	treeClasses = new QTreeWidget;
	treeClasses->setHeaderItem(new QTreeWidgetItem(QStringList(tr("Classes")+" : "), 
													DevQt::classes));
	tabExplorer->addTab(treeClasses, tr("Classes"));
	
	Explorer->setWindowIcon(QIcon(":/explorer.png"));
	Explorer->hide();
}

void DevGUI::setupCompiler()
{
	QAction *a;
	QToolBar *tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	tb->setWindowTitle(tr("Compiler Actions"));
	addToolBarBreak();
	addToolBar(tb);
	
	QMenu *menu = new QMenu(tr("E&xecute"), this);
	menuBar()->addMenu(menu);
	
	a = aCompile = new QAction(QIcon(":/compile.png"), tr("&Compile..."), this);
	a->setShortcut(Qt::CTRL + Qt::Key_F9);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( compile() ) );
	a->setEnabled(false);
	tb->addAction(a);
	menu->addAction(a);
	
	a = aRun = new QAction(QIcon(":/exec.png"), tr("&Run..."), this);
	a->setShortcut(Qt::CTRL + Qt::Key_F10);
	connect(a	, SIGNAL( triggered()  ),
			this, SLOT  ( run() ) );
	a->setEnabled(false);
	tb->addAction(a);
	menu->addAction(a);
	
	a = aParams = new QAction(tr("&Params"), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( params() ) );
	a->setEnabled(false);
	menu->addAction(a);
	
	menu->addSeparator();
	
	a = aCompRun = new QAction(QIcon(":/comprun.png"), tr("C&ompile and run..."), this);
	a->setShortcut(Qt::Key_F9);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( compRun() ) );
	a->setEnabled(false);
	tb->addAction(a);
	menu->addAction(a);
	
	a = aRebuild = new QAction(QIcon(":/rebuild.png"), tr("R&ebuild all..."), this);
	a->setShortcut(Qt::CTRL + Qt::Key_F11);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( rebuild() ));
	a->setEnabled(false);
	tb->addAction(a);
	menu->addAction(a);
	
	a = aSyntax = new QAction(tr("&Syntax check..."), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( syntax() ) );
	a->setEnabled(false);
	menu->addAction(a);
	
	a = aClean = new QAction(tr("C&lean..."), this);
	connect(a	, SIGNAL( triggered() ),
			this, SLOT  ( clean() ) );
	a->setEnabled(false);
	menu->addAction(a);
	
	tb->setIconSize( QSize(22, 22) );
   	
	Compiler = new DevDock(tr("Compiler"), Qt::BottomDockWidgetArea, this);
	tabCompiler = Compiler->Tab();
	Compiler->hide();
	Compiler->setWindowIcon(QIcon(":/compiler.png"));
}

void DevGUI::setupProjectActions()
{
	QMenu *menu = new QMenu(tr("&Project"), this);

	aSaveProject = new QAction(QIcon(":/saveproject"), tr("Save Project"), this);
	aSaveProject->setEnabled(false);
	connect(aSaveProject, SIGNAL(triggered()), this, SLOT(projectSave()));
	
	aSaveProjectAs = new QAction(QIcon(":/saveprojectas.png"), tr("Save project As..."), this);
	aSaveProjectAs->setEnabled(false);
	connect( aSaveProjectAs, SIGNAL(triggered()), this, SLOT(projectSaveAs()));

	// TODO: this should close the project and not the current file
	// TODO: connect this action to a slot
	aCloseProject = new QAction(tr("Close projec&t"), this);
	aCloseProject->setShortcut(Qt::CTRL + Qt::Key_W);
	aCloseProject->setEnabled(false);
	/*connect(a	, SIGNAL( triggered() ),this, SLOT( close() ) ); */

	menu->addAction( aNewProject );
	menu->addAction( aSaveProject );
	menu->addAction( aSaveProjectAs );
	menu->addAction( aCloseProject );
	
	menuBar()->addMenu(menu);
}

void DevGUI::setupSettings()
{
    QMenu *menu = new QMenu(tr("&Tools"), this);
    menuBar()->addMenu(menu);
    
	aSettings = new QAction(QIcon(":/settings.png"), tr("&Settings"), this);
	connect(aSettings	, SIGNAL( triggered() ),
			DEV_SETTINGS, SLOT  ( execute() ) );
	menu->addAction(aSettings);
}

bool DevGUI::load(const QString &f)
{
	if (!QFile::exists(f))
		return false;
	
	if (f.endsWith(".pro"))
		return loadProject(f);
	
	createEditor(f);
	
	DEV_SETTINGS->addRecent(f);
	
	return true;
}

bool DevGUI::loadProject(const QString &f)
{
	const bool s = workspace->addProject(f);
	
	if ( s )
	{
		aCompile->setEnabled(true);
		//aCompileCur->setEnabled(true);
		aRun->setEnabled(true);
		aParams->setEnabled(true);
		aCompRun->setEnabled(true);
		aRebuild->setEnabled(true);
		aSyntax->setEnabled(true);
		aClean->setEnabled(true);
		
		aSaveProject->setEnabled(true);
		aSaveProjectAs->setEnabled(true);
		aCloseProject->setEnabled(true);
		
		project->addItem(f);
		
		DEV_SETTINGS->addRecent(f, true);
		Explorer->show();
	}
	
	return s;
}

void DevGUI::changeProject(const QString& f)
{
	config->clear();
	
	DevProject *p = workspace->find(f);
	
	if ( !p )
		return;
	
	QStringList l = p->scopes();
	
	foreach (QString v, l)
		config->addItem(v);
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
		DevQt::supportedFiles 
	);
	
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
	
	if ( !f.open(QFile::WriteOnly | QFile::Text) )
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
    										DevQt::extFiles, &ext );
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
    										DevQt::extFiles, &ext );
	} 
	
	fileSave(name, ext);
}

void DevGUI::projectNew()
{
	;
}

void DevGUI::projectSave()
{
	DevProject *p = workspace->find( project->currentText() );
	
	if ( !p )
		return;
	
	QFile f( p->name() );
	
	if ( !f.open(QFile::WriteOnly | QFile::Text) )
		return;
	
	QTextStream out(&f);
	out<<p->content();
	
}

void DevGUI::projectSaveAs()
{
	DevProject *p = workspace->find( project->currentText() );
	
	if ( !p )
		return;
	
    QString ext, name = QFileDialog::getSaveFileName(this, "Save as...",
											QString(),
    										DevQt::extProjects, &ext);
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
    										DevQt::extProjects, &ext );
	} 
	
	checkExtension(name, ext);
	workspace->rename(p, name);
	
	project->setItemText(project->currentIndex(), name);
	
	projectSave();
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
    rDlg->hide();
    
    fDlg->execute(e->e);
}

void DevGUI::findNext()
{
	if ( !e )
        return;
    rDlg->hide();
    
    fDlg->process();
}

void DevGUI::replace()
{
	if ( !e )
        return;
    fDlg->hide();
    
    rDlg->execute(e->e);
}

void DevGUI::goTo()
{
	if ( !e )
        return;
    rDlg->hide();
    fDlg->hide();
    
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
	
    delete Editor->widget(pos);
    
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
		aCloseAll->setEnabled(false);
		aFind->setEnabled(false);
		aFindNext->setEnabled(false);
		aReplace->setEnabled(false);
		aGoto->setEnabled(false);
		
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
	if ( !e )
		return;
	
    rDlg->hide();
    fDlg->hide();
    
	pDlg->execute(e->name());
}

void DevGUI::compile()
{
	;
}

void DevGUI::compileCur()
{
	;
}

void DevGUI::run()
{
	;
}

void DevGUI::params()
{
	;
}

void DevGUI::compRun()
{
	;
}

void DevGUI::rebuild()
{
	;
}

void DevGUI::syntax()
{
	;
}

void DevGUI::clean()
{
	;
}

void DevGUI::aboutQt()
{
	QMessageBox::aboutQt(this, "about Qt");
}

void DevGUI::QtAssistant()
{
	QProcess::startDetached("assistant");
}

void DevGUI::clipboardDataChanged()
{
    aPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

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
		
		// WTF? no such slot... FMT, to be removed...?
		// disconnect(e, SIGNAL( message(const QString&, DevQt::StatusPurpose p) ), s,SLOT  ( message(const QString&, DevQt::StatusPurpose p) ) );
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
		aCloseAll->setEnabled(false);
		aFind->setEnabled(false);
		aFindNext->setEnabled(false);
		aReplace->setEnabled(false);
		aGoto->setEnabled(false);
		
        return;
	}
    
	doc = e->document();
	
    DEV_SETTINGS->applyFormat(e);

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
	aCloseAll->setEnabled(true);
    aSaveAs->setEnabled(true);
    aFind->setEnabled(true);
	aFindNext->setEnabled(true);
	aReplace->setEnabled(true);
	aGoto->setEnabled(true);

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
		while ( 1 )
		{
            e = ext.section(" *", count, count);
            count++;
			if ( e.isNull() )
			    break;
            if ( e.endsWith(')') )
                e.chop(1);
			if ( fn.contains(e) )
				return;
		}
		e = ext.section(" *", 1, 1);
		
		if ( e.endsWith(')') )
            e.chop(1);
        
		f += e;
	}
}

void DevGUI::hideCompiler()
{
	Compiler->setVisible( !Compiler->isVisible() );
}

void DevGUI::hideExplorer()
{
	
	Explorer->setVisible( !Explorer->isVisible() );
}


void DevGUI::closeEvent(QCloseEvent *e)
{
	closeAll();
	delete workspace;
	
	e->accept();
}
