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
	setupFileActions();
    setupEditActions();
    setupTextActions();
    setupCompiler();
    setupExplorer();
	
	Editor = new DevMDI(this);
	Editor->setWindowState(Qt::WindowMaximized);
    
	connect(Editor, SIGNAL(currentChanged(int)),
			this, SLOT(editorChanged()));
	setCentralWidget(Editor);

	connect(QApplication::clipboard()	, SIGNAL(dataChanged()),
			this						, SLOT  (clipboardDataChanged()) );
	
	for (int i = 1; i < DEV_APP->argc(); ++i)
		load(DEV_APP->argv()[i]);
	
	setWindowState(Qt::WindowMaximized);
	setWindowTitle("Dev-Qt++ 0.1.0 [*]");
	show();
}

DevGUI::~DevGUI()
{
	;
}

void DevGUI::setupFileActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("File Actions"));
    addToolBar(tb);

    QMenu *menu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(menu);

    QAction *a;

    a = new QAction(QIcon(":/new.png"), tr("&New..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_N);
    connect(a, SIGNAL(triggered()), this, SLOT(fileNew()));
    tb->addAction(a);
    menu->addAction(a);

    a = new QAction(QIcon(":/open.png"), tr("&Open..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_O);
    connect(a, SIGNAL(triggered()), this, SLOT(fileOpen()));
    tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    actionSave = a = new QAction(QIcon(":/save.png"), tr("&Save"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_S);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSave()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    a = new QAction(QIcon(":/save.png"), tr("Save &As..."), this);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    a->setEnabled(false);
    menu->addAction(a);

    menu->addSeparator();

    actionSaveAll = a = new QAction(QIcon(":/saveall.png"), tr("&Save All"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_A);
    connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAll()));
    tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    a = new QAction(QIcon(":/print.png"), tr("&Print..."), this);
    a->setShortcut(Qt::CTRL + Qt::Key_P);
    connect(a, SIGNAL(triggered()), this, SLOT(filePrint()));
    tb->addAction(a);
    menu->addAction(a);

    actionClose = a = new QAction(QIcon(":/close.png"), tr("&Close"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_W);
	a->setEnabled(false);
    connect(a, SIGNAL(triggered()), this, SLOT(fileClose()));
    tb->addAction(a);
    menu->addAction(a);

    menu->addSeparator();

    a = new QAction(QIcon(":/exit.png"), tr("E&xit"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_Q);
    connect( a, SIGNAL(triggered()), qApp, SLOT( quit() ) );
    menu->addAction(a);
}

void DevGUI::setupEditActions()
{
    QToolBar *tb = new QToolBar(this);
    tb->setWindowTitle(tr("Edit Actions"));
    addToolBar(tb);

    QMenu *menu = new QMenu(tr("&Edit"), this);
    menuBar()->addMenu(menu);

    QAction *a;
    a = actionUndo = new QAction(QIcon(":/undo.png"), tr("&Undo"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_Z);
    tb->addAction(a);
    menu->addAction(a);
    
    a = actionRedo = new QAction(QIcon(":/redo.png"), tr("&Redo"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_Y);
    tb->addAction(a);
    menu->addAction(a);
    
    menu->addSeparator();
    
    a = actionCut = new QAction(QIcon(":/cut.png"), tr("Cu&t"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_X);
    tb->addAction(a);
    menu->addAction(a);
    
    a = actionCopy = new QAction(QIcon(":/copy.png"), tr("&Copy"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_C);
    tb->addAction(a);
    menu->addAction(a);
    
    a = actionPaste = new QAction(QIcon(":/paste.png"), tr("&Paste"), this);
    a->setShortcut(Qt::CTRL + Qt::Key_V);
    tb->addAction(a);
    menu->addAction(a);
    actionPaste->setEnabled(!DevApp::clipboard()->text().isEmpty());
}

void DevGUI::setupTextActions()
{
	QToolBar *tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	tb->setWindowTitle(tr("Format Actions"));
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
}

void DevGUI::setupExplorer()
{
	
	Explorer = new DevDock(tr("Explorer"), Qt::LeftDockWidgetArea, this);
	
	tabExplorer = Explorer->Tab();
	
	treeFiles = new DevWorkSpace("default.dws");
	tabExplorer->addTab(treeFiles, tr("Files"));
	
	treeClasses = new QTreeWidget;
	treeClasses->setHeaderItem(new QTreeWidgetItem(QStringList("Classes : "), 
													DevQt::classes));
	tabExplorer->addTab(treeClasses, tr("Classes"));
}

void DevGUI::setupCompiler()
{
	QAction *a;
	QToolBar *tb = new QToolBar(this);
	tb->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    tb->setWindowTitle(tr("Compiler Actions"));
    addToolBar(tb);
    
    QMenu *menu = new QMenu(tr("&Compile"), this);
    menuBar()->addMenu(menu);
    
    actionCompile = a = new QAction(QIcon(":/compile.png"), tr("&Compile..."), this);
    a->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_C);
    //connect(a, SIGNAL(triggered()), this, SLOT(projComp()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
    actionCompile = a = new QAction(QIcon(":/rebuild.png"), tr("&Rebuild..."), this);
    a->setShortcut(Qt::CTRL + Qt::ALT + Qt::Key_A);
    //connect(a, SIGNAL(triggered()), this, SLOT(projRebuild()));
    a->setEnabled(false);
    tb->addAction(a);
    menu->addAction(a);
    
	Compiler = new DevDock(tr("Compiler"), Qt::BottomDockWidgetArea, this);
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
													tr("Open File..."),
        											QString(),
													DevQt::supportedFiles );
	
	foreach(QString fn, fl)
        load(fn);
}

void DevGUI::fileSave()
{
	DevEditor * e = qobject_cast<DevEditor *>(Editor->currentWidget());
	if ( !e )
        return;
    e->save();
}

void DevGUI::fileSaveAll()
{
	//TODO
}

void DevGUI::fileSaveAs()
{
	DevEditor * e = qobject_cast<DevEditor *>(Editor->currentWidget());
	if ( !e )
        return;
    e->saveAs();
}

void DevGUI::filePrint()
{
	DevEditor * e = qobject_cast<DevEditor *>(Editor->currentWidget());
	if ( !e )
        return;
    e->print();
}

void DevGUI::fileClose()
{
	DevEditor * e = qobject_cast<DevEditor *>(Editor->currentWidget());

	if ( !e )
		return;
	
    const int pos = Editor->currentIndex();
	const bool hadFocus = e->hasFocus();
	
    Editor->removeTab(pos);
	delete e;
    
    e = qobject_cast<DevEditor*>(Editor->currentWidget());
    
	if ( e ) {
		if ( hadFocus )
			e->setFocus();
	} else {
		actionClose->setEnabled(false);
	}
}

void DevGUI::textFamily(const QString &f)
{
	DevEditor * e = qobject_cast<DevEditor *>(Editor->currentWidget());

    if ( !e )
        return;
    e->setFont( QFont( f, Size->currentText().toInt() ) );
}

void DevGUI::textSize(const QString &p)
{
	DevEditor * e = qobject_cast<DevEditor *>(Editor->currentWidget());
    if ( !e )
        return;
    e->setFont( QFont( Font->currentText(), p.toInt() ) );
}

void DevGUI::clipboardDataChanged()
{
    actionPaste->setEnabled(!QApplication::clipboard()->text().isEmpty());
}

void DevGUI::fontChanged(const QFont &f)
{
    Font->setCurrentIndex(Font->findText(f.family()));
    Size->setCurrentIndex(Size->findText(QString::number(f.pointSize())));
}

void DevGUI::editorChanged()
{
	DevEditor * e = qobject_cast<DevEditor*>(Editor->currentWidget());

    if ( e ) {
		
		const QTextDocument *doc = e->document();
		
        disconnect(doc, SIGNAL(modificationChanged(bool)),
                   actionSave, SLOT(setEnabled(bool)));
        disconnect(doc, SIGNAL(modificationChanged(bool)),
                   this, SLOT(setWindowModified(bool)));
        disconnect(doc, SIGNAL(undoAvailable(bool)),
                   actionUndo, SLOT(setEnabled(bool)));
        disconnect(doc, SIGNAL(redoAvailable(bool)),
                   actionRedo, SLOT(setEnabled(bool)));

        disconnect(actionUndo, SIGNAL(triggered()), doc, SLOT(undo()));
        disconnect(actionRedo, SIGNAL(triggered()), doc, SLOT(redo()));

        disconnect(actionCut, SIGNAL(triggered()), e, SLOT(cut()));
        disconnect(actionCopy, SIGNAL(triggered()), e, SLOT(copy()));
        disconnect(actionPaste, SIGNAL(triggered()), e, SLOT(paste()));

        disconnect(e, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
        disconnect(e, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));

    }

    e = qobject_cast<DevEditor*>(Editor->currentWidget());
    if (!e)
        return;
    
	const QTextDocument *doc = e->document();
	
    fontChanged(doc->defaultFont());

    connect(doc, SIGNAL(modificationChanged(bool)),
            actionSave, SLOT(setEnabled(bool)));
    connect(doc, SIGNAL(modificationChanged(bool)),
            this, SLOT(setWindowModified(bool)));
    connect(doc, SIGNAL(undoAvailable(bool)),
            actionUndo, SLOT(setEnabled(bool)));
    connect(doc, SIGNAL(redoAvailable(bool)),
            actionRedo, SLOT(setEnabled(bool)));

    setWindowModified(e->isModified());
    actionSave->setEnabled(doc->isModified());
    actionUndo->setEnabled(doc->isUndoAvailable());
    actionRedo->setEnabled(doc->isRedoAvailable());

    connect(actionUndo, SIGNAL(triggered()), doc, SLOT(undo()));
    connect(actionRedo, SIGNAL(triggered()), doc, SLOT(redo()));

    const bool selection = e->textCursor().hasSelection();
    actionCut->setEnabled(selection);
    actionCopy->setEnabled(selection);

    connect(actionCut, SIGNAL(triggered()), e, SLOT(cut()));
    connect(actionCopy, SIGNAL(triggered()), e, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), e, SLOT(paste()));

    connect(e, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
    connect(e, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));
}

DevEditor* DevGUI::createEditor(const QString &title, bool show)
{
    QString stitle(title.isEmpty()
			? QString(tr("noname_%1")).arg(++noname_count)
			: title);
				
	DevEditor *edit = new DevEditor(stitle, DevEdit::fromFile);
	actionClose->setEnabled(true);
    
	if ( !show )
		return edit;
	
    int tab = Editor->addTab(edit, QFileInfo(stitle).fileName());
    Editor->setCurrentIndex(tab);
    Editor->show();
    
    edit->setFocus();
    return edit;
}
