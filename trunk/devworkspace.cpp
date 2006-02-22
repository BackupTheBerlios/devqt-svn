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
#include "devproject.h"

DevWorkSpace::DevWorkSpace(QString name, QWidget *parent)
 : QTreeWidget(parent), n(name)
{
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	
	setHeaderItem(new QTreeWidgetItem(	QStringList("WorkSpace : "),
										DevQt::classes) );
	
	root = new QTreeWidgetItem(	QStringList(QFileInfo(n).baseName()),
								DevQt::files);
	root->setIcon(0, QIcon(":/workspace.png"));
	addTopLevelItem(root);
	
	connect(this, SIGNAL( itemClicked(QTreeWidgetItem*, int) ),
			this, SLOT  ( focus(QTreeWidgetItem*, int) ) );
	
	DevProject *p;
	QTreeWidgetItem *i;
	
	i = new QTreeWidgetItem(QStringList("External"),
							DevQt::files);
	i->setIcon(0, QIcon(":/project.png"));
	p = new DevProject("External");
	
	insert(i, p);
	projects.append(p);
	addTopLevelItem(i);
	
	if ( QFile::exists(n) )
		loadWorkSpace(n);
}

DevWorkSpace::~DevWorkSpace()
{
	;
}

void DevWorkSpace::loadWorkSpace(const QString& name)
{
	QFile f(n);
	
	if ( !f.open(QFile::ReadOnly) )
		return;
	
	QString data(f.readAll());
	
}

void DevWorkSpace::addProject(const QString& name)
{
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
	
	insert(i, p);
	projects.append(p);
	
	root->addChild(i);
	
	DevFile *sf;
	QTreeWidgetItem *si;
	QStringList key, dat;
	key<<"SOURCES"<<"HEADERS"<<"FORMS"<<"RESOURCES";
	
	for(int n=0; n<3; n++)
	{
		dat = p->content(key[n], true);
		
		foreach(QString fn, dat)
		{
			sf = new DevFile(fn, 0);
			connect(sf	, SIGNAL( rename(AbstractFile*) ),
					this, SLOT  ( rename(AbstractFile*) ) );
			connect(sf	, SIGNAL( deletion(AbstractFile*) ),
					this, SLOT  ( deletion(AbstractFile*) ) );
			si = new QTreeWidgetItem(	QStringList(QFileInfo(fn).fileName()),
										n);
			si->setIcon(0, QIcon(":/file.png"));
			i->addChild(si);
			
			insert(si, sf);
		}
	}
}

/*
void DevWorkSpace::addFolder(const QString& name, const QString& project)
{
	;
}

void DevWorkSpace::addFile(const QString& name, const QString& project, const QString& folder)
{
	;
}
*/

void DevWorkSpace::newProject(const QString& name)
{
	;
}

void DevWorkSpace::contextMenuEvent(QContextMenuEvent *e)
{
	QPoint pos = e->pos();
	QTreeWidgetItem *i = itemAt(pos);
	if ( !i )
		return;

	iter = DevMap::find(i);
	if ( iter == end() )
		return;
		
	AbstractFile *f = *iter;
	
	f->menu(mapToGlobal(pos)+QPoint(0, 16));
}

void DevWorkSpace::focus(QTreeWidgetItem *i, int c)
{
	if ( (c != 0) || (!i) )
		return;
	
	iterator iter = DevMap::find(i);
	
	if ( iter == end() )
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
	QTreeWidgetItem *i, *p;
	
	if ( !(i = key(o)) )
		return;
	
	disconnect(o, 0, 0, 0);
	
	remove(i);
	
	p = i->parent();
	p->takeChild( p->indexOfChild(i) );
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

