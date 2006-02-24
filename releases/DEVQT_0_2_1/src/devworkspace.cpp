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

#include "devworkspace.h"

#include "devgui.h"
#include "devedit.h"

DevWorkSpace::DevWorkSpace(QString name, QWidget *parent)
 : QTreeWidget(parent), n(name)
{
	setDragEnabled(true);
	//setSortingEnabled(true);
	
	setHeaderItem(new QTreeWidgetItem(	QStringList(tr("WorkSpace : ")),
										DevQt::classes) );
	
	connect(this, SIGNAL( itemClicked(QTreeWidgetItem*, int) ),
			this, SLOT  ( focus(QTreeWidgetItem*, int) ) );
	
	DevProject *p;
	QTreeWidgetItem *i;
	
	i = new QTreeWidgetItem(QStringList(tr("External")),
							DevQt::files);
	i->setIcon(0, QIcon(":/project.png"));
	p = new DevProject(tr("External"));
	
	DevTreeMap::insert(i, p);
	DevProjectMap::insert(name, p);
	
	addTopLevelItem(i);
	
	if ( QFile::exists(n) )
		loadWorkSpace(n);
}

DevWorkSpace::~DevWorkSpace()
{
	//for (DevTreeMap::iterator i = DevTreeMap::begin(); i != DevTreeMap::end(); i++)
	//	delete (*i);
	
	//for (DevProjectMap::iterator i = DevProjectMap::begin(); i != DevProjectMap::end(); i++)
	//	delete (*i);
	
	DevTreeMap::clear();
	DevProjectMap::clear();
}

void DevWorkSpace::loadWorkSpace(const QString& name)
{
	QFile f(n);
	
	if ( !f.open(QFile::ReadOnly | QFile::Text) )
		return;
	
	QString data(f.readAll());
	
}

void DevWorkSpace::rename(AbstractFile *f, const QString& name)
{
	if ( !f )
		return;
	
	f->n = name;
	
	QTreeWidgetItem *i = DevTreeMap::key(f);
	
	if ( !i )
		return;
	
	i->setText(0, QFileInfo(name).baseName());
	
	if ( f->flag() == AbstractFile::project )
	{
		DevProject *p = qobject_cast<DevProject*>(f);
		
		DevProjectMap::iterator it = DevProjectMap::find( DevProjectMap::key(p) );
		
		if ( it == DevProjectMap::end() )
			return;
		
		DevProjectMap::erase(it);
		DevProjectMap::insert(name, p);
	}
}

bool DevWorkSpace::addProject(const QString& name)
{
	foreach (AbstractFile *f, DevTreeMap::values())
	{
		if ( f->name() == name )
			return false;
	}
	
	DevProject *p;
	QTreeWidgetItem *i;
	
	i = new QTreeWidgetItem(QStringList(QFileInfo(name).baseName()),
							DevQt::files);
	i->setIcon(0, QIcon(":/project.png"));
	
	p = new DevProject(name);
	connect(p	, SIGNAL( rename(AbstractFile*) ),
			this, SLOT  ( rename(AbstractFile*) ) );
	connect(p	, SIGNAL( deletion(AbstractFile*) ),
			this, SLOT  ( deletion(AbstractFile*) ) );
	connect(p	, SIGNAL( addFile(DevFolder*) ),
			this, SLOT  ( addFile(DevFolder*) ) );
	connect(p	, SIGNAL( subFolder(DevFolder*) ),
			this, SLOT  ( subFolder(DevFolder*) ) );
	
	DevTreeMap::insert(i, p);
	DevProjectMap::insert(name, p);
	
	addTopLevelItem(i);
	
	DevScope *top = p->global;
	QStringList keys, cfg = p->content("CONFIG");
	keys<<"SOURCES"<<"HEADERS"<<"FORMS"<<"RESOURCES";
	
	createNested(top, keys, cfg, i);
	
	expandItem(i);
	scrollToItem(i, QAbstractItemView::PositionAtTop);
	
	return true;
}

void DevWorkSpace::createNested(DevScope *s, const QStringList& vars,
								const QStringList& cfg, QTreeWidgetItem *i)
{
	for(DevScopeMap::iterator it = s->nest.begin(); it != s->nest.end(); it++)
	{
		DevFolder *fold;
		QString name = it.key();
		
		if ( name.isEmpty() )
			continue;
		
		if ( cfg.contains(name) )
		{
			fold = new DevFolder(name);
			connect(fold, SIGNAL( addFile(DevFolder*) ),
					this, SLOT  ( addFile(DevFolder*) ) );
			connect(fold, SIGNAL( subFolder(DevFolder*) ),
					this, SLOT  ( subFolder(DevFolder*) ) );
		} else {
			fold = new DevDirectory(name);
		}
		
		connect(fold, SIGNAL( rename(AbstractFile*) ),
				this, SLOT  ( rename(AbstractFile*) ) );
		connect(fold, SIGNAL( deletion(AbstractFile*) ),
				this, SLOT  ( deletion(AbstractFile*) ) );
				
		QTreeWidgetItem *si = new QTreeWidgetItem(	i,
													QStringList(name),
													DevQt::folder);
		si->setIcon(0, QIcon(":/folder.png"));
		
		DevTreeMap::insert(si, fold);
		
		createNested(*it, vars, cfg, si);
	}
	QStringList dat;
	
	foreach(QString n, vars)
	{
		QStringList raw;
		DevVariable v = (*s)[n];
		raw<<v["="]<<v["+="]<<v["*="];
		
		foreach(QString fn, raw)
			s->project()->insert(dat, fn, true, true);
	}
	
	dat.sort();
	
	DevFile *sf;
	QTreeWidgetItem *si;
	
	foreach(QString fn, dat)
	{
		sf = new DevFile(fn, 0);
		connect(sf	, SIGNAL( rename(AbstractFile*) ),
				this, SLOT  ( rename(AbstractFile*) ) );
		connect(sf	, SIGNAL( deletion(AbstractFile*) ),
				this, SLOT  ( deletion(AbstractFile*) ) );
		si = new QTreeWidgetItem(	QStringList(QFileInfo(fn).fileName()),
									DevQt::files);
		si->setIcon(0, QIcon(":/file.png"));
		i->addChild(si);
		
		DevTreeMap::insert(si, sf);
	}
}


/*
bool DevWorkSpace::addFolder(const QString& name, const QString& project)
{
	;
}

bool DevWorkSpace::addFile(const QString& name, const QString& project, const QString& folder)
{
	;
}
*/

bool DevWorkSpace::newProject(const QString& name)
{
	return false;
}

void DevWorkSpace::contextMenuEvent(QContextMenuEvent *e)
{
	QPoint pos = e->pos();
	QTreeWidgetItem *i = itemAt(pos);
	if ( !i )
		return;

	DevTreeMap::iterator iter = DevTreeMap::find(i);
	if ( iter == DevTreeMap::end() )
		return;
		
	AbstractFile *f = *iter;
	
	f->menu(mapToGlobal(pos)+QPoint(0, 16));
}

void DevWorkSpace::focus(QTreeWidgetItem *i, int c)
{
	if ( (c != 0) || (!i) )
		return;
	
	DevTreeMap::iterator iter = DevTreeMap::find(i);
	
	if ( iter == DevTreeMap::end() )
		return;
	
	int index;
	QTabWidget *edit = DEV_GUI->Editor;
	DevFile *f = qobject_cast<DevFile*>(*iter);
	
	if ( !f )
		return;
	
	DevEdit *e = f->e;
	
	if ( !e )
		return (void)(f->e = DEV_GUI->createEditor(f->n, true));
	
	if ( (index=edit->indexOf(e)) == -1 )
		index = edit->addTab(e, QFileInfo(f->n).fileName());
	
	edit->setCurrentIndex(index);
	edit->show();
	e->setFocus();
}

void DevWorkSpace::deletion(AbstractFile *o)
{
	//QMessageBox::warning(0, "", "removing???");
	
	QTreeWidgetItem *i;
	
	if ( !(i = DevTreeMap::key(o)) )
		return;
	
	disconnect(o, 0, 0, 0);
	
	//QMessageBox::warning(0, "", "removing...");
	
	switch ( o->flag() )
	{
		case AbstractFile::file :
			break;
		
		case AbstractFile::folder :
			break;
			
		default:
			break;
	}
	
	DevTreeMap::remove(i);
	
	delete i;
}

void DevWorkSpace::rename(AbstractFile *o)
{
	QMessageBox::warning(0, "!", "renaming...");
}

void DevWorkSpace::addFile(DevFolder *p)
{
	QMessageBox::warning(0, "!", "adding file...");
}

void DevWorkSpace::subFolder(DevFolder *p)
{
	QMessageBox::warning(0, "!", "creating subfolder...");
}

DevProject* DevWorkSpace::find(const QString& n)
{
	DevProjectMap::iterator i = DevProjectMap::find(n);
	if ( i == DevProjectMap::end() )
		return 0;
	return *i;
}

AbstractFile* DevWorkSpace::find(QTreeWidgetItem *i)
{
	DevTreeMap::iterator n = DevTreeMap::find(i);
	if ( n == DevTreeMap::end() )
		return 0;
	return *n;
}

