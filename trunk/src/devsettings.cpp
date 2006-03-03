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

#include "devsettings.h"

#include "devgui.h"
#include "devedit.h"

/*

Settings map:

|       purpose           |    key                |      type               |
|---------------------------------------------------------------------------|
|    recent files:        |    recent/files       |      QStringList        |
|    recent projects:     |    recent/projects	  |      QStringList        |
|-------------------------|-----------------------|-------------------------|
|    higlight formats     |    edit/formats       |  QList<QTextCharFormat> |
|    editor font          |    edit/font          |  QFont                  |
|    gutter font          |    edit/gutter/font   |  QFont                  |
|    tab width            |    edit/tabs          |  int                    |
|    insert mode          |    edit/insert        |  bool                   |
|    auto indent          |    edit/indent        |  bool                   |
|    scrollbar as needed  |    edit/scroll        |  bool                   |
|---------------------------------------------------------------------------|

*/

DevSettings* DevSettings::inst = 0;

DevSettings* DevSettings::Instance()
{
	if ( !inst )
		inst = new DevSettings;
		
	return inst;
}

void DevSettings::killSettings()
{
	if ( inst )
		delete inst;
}

DevSettings::DevSettings(QWidget *p)
 : QSettings(p)
{
	dlg = new QDialog(p, Qt::Dialog | Qt::WindowStaysOnTopHint);
	
	QVBoxLayout *box = new QVBoxLayout;
	QTabWidget *tab = new QTabWidget(dlg);
	
	QFrame *frame;
	QGridLayout *grid;
	
// <General>
	frame = new QFrame;
	frame->setFrameShape(QFrame::Box);
	frame->setFrameShadow(QFrame::Sunken);
	
	grid = new QGridLayout;
	
	frame->setLayout(grid);
	tab->addTab(frame, tr("General") );
// </General>
	
// <Compiler>
	frame = new QFrame;
	frame->setFrameShape(QFrame::Box);
	frame->setFrameShadow(QFrame::Sunken);
	
	grid = new QGridLayout;
	
	frame->setLayout(grid);
	tab->addTab(frame, tr("Compilation") );
// </Compiler>
	
// <Editors>
	frame = new QFrame;
	frame->setFrameShape(QFrame::Box);
	frame->setFrameShadow(QFrame::Sunken);
	
	grid = new QGridLayout;
	
	frame->setLayout(grid);
	tab->addTab(frame, tr("Editors") );
// </Editors>
	
// <Highlight>
	frame = new QFrame;
	frame->setFrameShape(QFrame::Box);
	frame->setFrameShadow(QFrame::Sunken);
	
	grid = new QGridLayout;
	
	frame->setLayout(grid);
	tab->addTab(frame, tr("Highlighting") );
// </Highlight>
	
	box->addWidget(tab);
	
	QHBoxLayout *but = new QHBoxLayout;
	
	b_Default = new QPushButton(QIcon(":/default.png"), tr("&Default"));
	but->addWidget(b_Default);
	
	b_Valid = new QPushButton(QIcon(":/ok.png"), tr("&Ok"));
	but->addWidget(b_Valid);
	
	b_Cancel = new QPushButton(QIcon(":/cancel.png"), tr("&Cancel"));
	but->addWidget(b_Cancel);
	
	box->addLayout(but);
	dlg->setLayout(box);
	
	m = new QMenu(tr("&Reopen..."), p);
	m->setIcon(QIcon(":/reopen.png"));
	
	aClear = new QAction(QIcon(":/clear.png"), tr("&Clear history"), this);
	connect(aClear	, SIGNAL( triggered() ),
			this	, SLOT  ( clearRecents() ) );
	recent();
	
	if ( allKeys().isEmpty() )
	{
		;//dlg->exec();
	} else {
		;
	}
	
}

DevSettings::~DevSettings()
{
        beginGroup("gui");
        setValue("state", QVariant(DEV_GUI->windowState()));
        setValue("title", QVariant(DEV_GUI->windowTitle()));
        setValue("height", QVariant(DEV_GUI->height()));
        setValue("width", QVariant(DEV_GUI->width()));
        setValue("pos", QVariant(DEV_GUI->pos()));
        endGroup();

	beginGroup("editor");
	setValue("fontfamily", QVariant(DEV_GUI->getFontFamily()));
	setValue("fontsize", QVariant(DEV_GUI->getFontSize()));
	endGroup();

	beginGroup("highlighter");
	setValue("number", QVariant(DEV_GUI->getNumberBrush()));
	setValue("quote", QVariant(DEV_GUI->getQuoteBrush()));
	setValue("preprocessor", QVariant(DEV_GUI->getPreprocessorBrush()));
	setValue("keyword", QVariant(DEV_GUI->getKeywordBrush()));
	setValue("comment", QVariant(DEV_GUI->getCommentBrush()));
	endGroup();
	qDebug("Settings saved.");
}

void DevSettings::applyFormat(DevEdit *e)
{
	;
}

QMenu* DevSettings::recent()
{
	m->clear();
	m->disconnect();
	recents.clear();
	
	m->addAction(aClear);
	
	int n = 0;
	QString name, s, prefix;
	
	QStringList l, keys = QStringList()<<"projects"<<"files";
	
	beginGroup("recent");
	
	foreach ( QString name, keys )
	{
		l = value(name).toStringList();
		
		m->addSeparator();
		
		foreach ( QString s, l )
		{
			prefix = QString::number(n) + " : ";
			
			if ( n++ < 10 )
				prefix.prepend('&');
			
			QAction *a = new QAction(prefix + s, this);
			m->addAction(a);
			
			recents.insert(a, s);
		}
	}
	
	endGroup();
	
	connect(m	, SIGNAL( triggered(QAction*) ),
			this, SLOT  ( recent(QAction*) ) );
	
	return m;
}

void DevSettings::execute()
{
	if ( allKeys().isEmpty() )
	{
		;
	} else {
		;
	}
	
	dlg->exec();
}

void DevSettings::clearRecents()
{
	remove("recent/files");
	remove("recent/projects");
}

void DevSettings::addRecent(const QString& n, bool project)
{
	if ( n.isEmpty() )
		return;
	
	int max;
	QString s;
	QStringList l;
	
	if ( project )
	{
		max = maxProjects;
		s = "recent/projects";
	} else {
		max = maxFiles;
		s = "recent/files";
	}
	
	l = value(s).toStringList();
	
	for( int i = 0; i < l.size(); i++)
		if ( l.at(i) == n )
			l.removeAt(i);
	
	l.prepend(n);
	
	while ( l.size() > max )
		l.pop_back();
	
	setValue(s, l);
}

void DevSettings::recent(QAction *a)
{
	QHash<QAction*, QString>::iterator i = recents.find(a);
	
	if ( i == recents.end() )
		return;
	
	DEV_GUI->load(*i);
}

/*

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
