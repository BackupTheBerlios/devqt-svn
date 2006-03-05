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
#include "coreedit.h"
#include "devdialogs.h"
#include "devfileinfo.h"

class DevFileList : public QMap<QString, QString> //filename, variable
{
	public:
		
		void insert(const QString& name, const QString& cat, DevProject *pro)
		{
			int i;
			
			if ( (i=name.indexOf("$$")) != -1 )
			{
				if ( !pro )
					return;
				
				QString prefix(name.mid(0, i)), suffix(name.mid(i+2));
				
				if ( suffix.startsWith('(') || suffix.startsWith('{') )
					suffix.remove(0, 1);
					
				if ( suffix.endsWith(')') || suffix.endsWith('}') )
					suffix.chop(1);
					
				QStringList list = pro->content(suffix, true);
				
				foreach (QString var, list)
					insert(prefix+var, cat, pro);
				
			} else {
				if ( contains(name) )
					return;
				
				QMap<QString, QString>::insert( DevFileInfo(name, 
													QStringList(QFileInfo(
																	pro->name()
																).path()
															)
													).all(),
												cat
												);
			}
		}

};

DevWorkSpace::DevWorkSpace(QString name, QWidget *parent)
 : QTreeWidget(parent), n(name)
{
	setAcceptDrops(true);
	setDragEnabled(true);
	setDropIndicatorShown(true);
	setSelectionMode(QAbstractItemView::ExtendedSelection);
	//setSortingEnabled(true);
	
	setHeaderItem(new QTreeWidgetItem(	QStringList(tr("WorkSpace : ")),
										DevQt::classes) );
	
	connect(this, SIGNAL( itemActivated(QTreeWidgetItem*, int) ),
			this, SLOT  ( focus(QTreeWidgetItem*, int) ) );
	
	DevProject *p;
	QTreeWidgetItem *i;
	
	i = new QTreeWidgetItem(QStringList(tr("External")),
							DevQt::files);
	i->setIcon(0, QIcon(":/project.png"));
	i->setFlags( 	Qt::ItemIsSelectable | Qt::ItemIsEnabled | 
					Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
	
	p = new DevProject(tr("External"));
	p->m->clear();
	p->m->addAction(p->aSub);
	p->m->addAction(p->aNew);
	connect(p	, SIGNAL( addFile(DevFolder*) ),
			this, SLOT  ( addFile(DevFolder*) ) );
	connect(p	, SIGNAL( subFolder(DevFolder*) ),
			this, SLOT  ( subFolder(DevFolder*) ) );
	
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
	i->setFlags( 	Qt::ItemIsSelectable | Qt::ItemIsEnabled | 
					Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
	
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
	keys<<"SOURCES"<<"HEADERS"<<"FORMS"<<"RESOURCES"<<"TRANSLATIONS";
	
	createNested(top, keys, cfg, i);
	
	expandItem(i);
	scrollToItem(i, QAbstractItemView::PositionAtTop);
	
	return true;
}

void DevWorkSpace::createNested(DevScope *s, const QStringList& vars,
								const QStringList& cfg, QTreeWidgetItem *i)
{
	for (DevScopeMap::iterator it = s->nest.begin(); it != s->nest.end(); it++)
	{
		DevFolder *fold;
		QString name = it.key();
		
		if ( name.isEmpty() )
			continue;
		
		if ( cfg.contains(name) )
		{
			fold = new DevFolder(name);
			connect(fold, SIGNAL( rename(AbstractFile*) ),
					this, SLOT  ( rename(AbstractFile*) ) );
			connect(fold, SIGNAL( deletion(AbstractFile*) ),
					this, SLOT  ( deletion(AbstractFile*) ) );
		} else {
			fold = new DevDirectory(name);
		}
		
		fold->o = s;
		
		connect(fold, SIGNAL( addFile(DevFolder*) ),
				this, SLOT  ( addFile(DevFolder*) ) );
		connect(fold, SIGNAL( subFolder(DevFolder*) ),
				this, SLOT  ( subFolder(DevFolder*) ) );
		
				
		QTreeWidgetItem *si = new QTreeWidgetItem(	i,
													QStringList(name),
													DevQt::folder);
		si->setIcon(0, QIcon(":/folder.png"));
		si->setFlags( 	Qt::ItemIsSelectable | Qt::ItemIsEnabled | 
						Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled );
		
		DevTreeMap::insert(si, fold);
		
		createNested(*it, vars, cfg, si);
	}
	
	//QStringList dat;
	DevFileList dat;
	
	foreach (QString n, vars)
	{
		QStringList raw;
		DevVariable v = (*s)[n];
		raw<<v["="]<<v["+="]<<v["*="];
		
		foreach (QString fn, raw)
		{
			/*
			s->project()->insert(dat, fn, true, true);
			*/
			dat.insert(fn, n, s->project());
		}
		
	}
	
	DevFile *sf;
	QTreeWidgetItem *si;
	
	for ( DevFileList::iterator iter = dat.begin(); iter != dat.end(); iter++ )
	{
		sf = new DevFile(iter.key(), 0);
		sf->v = *iter;
		connect(sf	, SIGNAL( rename(AbstractFile*) ),
				this, SLOT  ( rename(AbstractFile*) ) );
		connect(sf	, SIGNAL( deletion(AbstractFile*) ),
				this, SLOT  ( deletion(AbstractFile*) ) );
		si = new QTreeWidgetItem(	QStringList(QFileInfo(iter.key()).fileName()),
									DevQt::files);
		si->setIcon(0, QIcon(":/file.png"));
		si->setFlags( 	Qt::ItemIsSelectable | Qt::ItemIsDragEnabled |
						Qt::ItemIsEnabled );
		i->addChild(si);
		
		sf->o = s;
		
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

bool DevWorkSpace::closeProject(const QString& name)
{
	if ( name.isEmpty() )
		return false;
	
	DevProjectMap::iterator p = DevProjectMap::find(name);
	
	if ( p == DevProjectMap::end() )
		return false;
	
	deletion(*p);
	
	return true;
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
	
	e->accept();
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
	QTreeWidgetItem *i;
	
	if ( !(i = DevTreeMap::key(o)) )
		return;
	
	disconnect(o, 0, 0, 0);
	
	DevTreeMap::remove(i);
	delete i;
	
	DevScope *s = o->o;
	QString name = o->name();
	
	switch ( o->flag() )
	{
		case AbstractFile::file :
			{
				QStringList l, m;
				QString tmp, var, op;
				DevVarsMap::iterator i_var;
				DevVariable::iterator i_val;
				
				m<<"="<<"+="<<"*=";
				l<<"SOURCES"<<"HEADERS"<<"FORMS"<<"RESOURCES"<<"TRANSLATIONS";
				
				if ( !s )
					return;
				
				foreach ( var, l )
				{
					i_var = s->find(var);
					
					if ( i_var == s->end() )
						continue;
					
					foreach ( op, m )
					{
						i_val = i_var->find(op);
						
						if ( i_val == i_var->end() )
							continue;
						
						tmp = DevFileInfo(name).relative(s->project()->n);
						
						i_val->removeAll(tmp);
					}
				}
				
				break;
			}
		
		case AbstractFile::scope :
		case AbstractFile::folder :
			
			if ( !s )
				return;
			
			n += s->nest.remove(name);
			break;
		
		case AbstractFile::project :
			{
				DevProjectMap::remove(name);
				
				QComboBox *cb = DEV_GUI->project;
				
				int index = cb->findText(name);
				
				cb->removeItem( index );
				
				if ( !DevProjectMap::size() )
				{
					DEV_GUI->aCompile->setEnabled(true);
					DEV_GUI->aCompileCur->setEnabled(true);
					DEV_GUI->aRun->setEnabled(true);
					DEV_GUI->aParams->setEnabled(true);
					DEV_GUI->aCompRun->setEnabled(true);
					DEV_GUI->aRebuild->setEnabled(true);
					DEV_GUI->aSyntax->setEnabled(true);
					DEV_GUI->aClean->setEnabled(true);
					
					DEV_GUI->aSaveProject->setEnabled(true);
					DEV_GUI->aSaveProjectAs->setEnabled(true);
					DEV_GUI->aCloseProject->setEnabled(true);
				}
				
				break;
			}
			
		default:
			break;
	}
	
	delete o;
	
}

void DevWorkSpace::rename(AbstractFile *o)
{
	QTreeWidgetItem *i;
	
	if ( !(i = DevTreeMap::key(o)) )
		return;
	
	bool ok;
	QString name = QInputDialog::getText(this, "Renaming :", "New name : ",
										QLineEdit::Normal, i->text(0), &ok
										);
	
	if ( !ok || name.isEmpty() )
		return;
	
	DevProject *p;
	QString whole, old = i->text(0), old_whole = o->n;
	
	i->setText(0, name);
	o->n = whole = DevFileInfo(name, QStringList(DevFileInfo(o->n).path())).all();
	
	DevScope *s = o->o;
	
	switch ( o->flag() )
	{
		case AbstractFile::file :
			{
				if ( !s )
					return;
				
				QStringList l, m;
				QString tmp, var, op;
				DevVarsMap::iterator i_var;
				DevVariable::iterator i_val;
				
				m<<"="<<"+="<<"*=";
				l<<"SOURCES"<<"HEADERS"<<"FORMS"<<"RESOURCES"<<"TRANSLATIONS";
				
				if ( !s )
					return;
				
				foreach ( var, l )
				{
					i_var = s->find(var);
					
					if ( i_var == s->end() )
						continue;
					
					foreach ( op, m )
					{
						i_val = i_var->find(op);
						
						if ( i_val == i_var->end() )
							continue;
						
						for ( int i = 0; i < i_val->size(); i++ )
						{
							if ( i_val->at(i) == old )
								(*i_val)[i] = name;
						}
					}
				}
				
				break;
			}
		
		case AbstractFile::folder :
			if ( !s )
				return;
			
			p = s->project();
			
			if ( !p )
				return;
			
			if ( !p->content("CONFIG").contains(name) )
				(*(p->global))["CONFIG"]["+="]<<name;
		
		case AbstractFile::scope :
			{
				if ( !s )
					return;
				
				DevScopeMap::iterator it = s->nest.find(old);
				
				if ( it == s->nest.end() )
					return ;
				
				s->nest.insert(name, *it);
				s->nest.erase(it);
				
				break;
			}
		
		case AbstractFile::project :
			{
				DevProjectMap::iterator it = DevProjectMap::find(old_whole);
				
				if ( it == DevProjectMap::end() )
					return;
				
				DevProjectMap::insert(whole, *it); 	//insert renamed project
				DevProjectMap::erase(it);			//delete old project
				
				QComboBox *cb = DEV_GUI->project;
				
				int index = cb->findText(old_whole);
				
				cb->setItemText(index, whole);
				
				break;
			}
		
		default:
			break;
	}
	
}

void DevWorkSpace::addFile(DevFolder *f)
{
	QTreeWidgetItem *i;
	
	if ( !(i = DevTreeMap::key(f)) )
		return;
	
	DevScope *s;
	
	if ( f->flag() == AbstractFile::project )
		s = qobject_cast<DevProject*>(f)->global;
	else
		s = *f->o->nest.find(i->text(0));
	
	if ( !s )
		return;
	
	QString n;
	QString name = DevDialogs::newFile(n);
	
	if ( name.isEmpty() || n.isNull() )
		return;
	
	DevVarsMap::iterator it = s->find(n);
	
	(*s)[n]["+="]<<name;
	
	QTreeWidgetItem * si;
	
	si = new QTreeWidgetItem(	QStringList( DevFileInfo(name).name() ),
								DevQt::files );
	si->setIcon(0, QIcon(":/file.png"));
	i->addChild(si);
	
	DevFile *sf = new DevFile(name, 0);
	connect(sf	, SIGNAL( rename(AbstractFile*) ),
			this, SLOT  ( rename(AbstractFile*) ) );
	connect(sf	, SIGNAL( deletion(AbstractFile*) ),
			this, SLOT  ( deletion(AbstractFile*) ) );
	
	sf->o = s;
	
	DevTreeMap::insert(si, sf);
	
}

void DevWorkSpace::subFolder(DevFolder *f)
{
	QTreeWidgetItem *i;
	
	if ( !(i = DevTreeMap::key(f)) )
		return;
	
	DevScope *s;
	DevProject *p;
	
	if ( f->flag() == AbstractFile::project )
	{
		p = qobject_cast<DevProject*>(f);
		s = p->global;
	} else {
		s = *(f->o->nest.find(i->text(0)));
		p = s->project();
	} 
	
	if ( !s )
		return;
	
	DevQt::node n;
	QString name = DevDialogs::newFold(n);
	
	if ( name.isEmpty() || (n == DevQt::none) )
		return;
	
	DevScopeMap::iterator it = s->nest.find(name);
	
	if ( it != s->nest.end() )
		return (void)QMessageBox::warning(0, 0, "Folder already exists");
	
	QTreeWidgetItem * si;
	
	si = new QTreeWidgetItem(	QStringList( DevFileInfo(name).name() ),
								DevQt::folder );
	si->setIcon(0, QIcon(":/folder.png"));
	i->addChild(si);
	
	DevFolder *sf;
	
	if ( n == DevQt::folder )
	{
		if ( !p->content("CONFIG").contains(name) )
			(*(p->global))["CONFIG"]["+="]<<name;
		
		sf = new DevFolder(name);
		connect(sf	, SIGNAL( rename(AbstractFile*) ),
				this, SLOT  ( rename(AbstractFile*) ) );
		connect(sf	, SIGNAL( deletion(AbstractFile*) ),
				this, SLOT  ( deletion(AbstractFile*) ) );
	} else {
		sf = new DevDirectory(name);
	}
	
	connect(sf	, SIGNAL( addFile(DevFolder*) ),
			this, SLOT  ( addFile(DevFolder*) ) );
	connect(sf	, SIGNAL( subFolder(DevFolder*) ),
			this, SLOT  ( subFolder(DevFolder*) ) );
	
	sf->o = s;
	
	DevTreeMap::insert(si, sf);
	
	DevScope *creation = new DevScope(p, s);
	s->nest.insert(name, creation);
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

bool DevWorkSpace::dropMimeData(QTreeWidgetItem *parent, int index,
								const QMimeData *data, Qt::DropAction action)
{
	QTreeWidgetItem *top = parent ? parent->child(index) : topLevelItem(index);
	
	if ( !top )
		return false;
	
	DevTreeMap::iterator p = DevTreeMap::find(top);
	
	if ( p == DevTreeMap::end() )
		return false;
	
	DevScope *owner;
	AbstractFile *f = *p;
	
	if ( f->flag() == AbstractFile::project )
		owner = qobject_cast<DevProject*>(f)->global;
	else
		owner = (f)->o->nested(top->text(0));
	
	QList<QTreeWidgetItem*> select = selectedItems();
	
	foreach (QTreeWidgetItem *i, select)
	{
		DevTreeMap::iterator it;
		QTreeWidgetItem *old = i->parent();
		
		it = DevTreeMap::find(i);
		
		if ( (it == DevTreeMap::end()) || !old )
			continue;
		
		AbstractFile *ab = *it;
		
		int flag = ab->flag();
		DevScope *oldown = ab->o;
		QString name = ab->name();
		
		if ( flag == AbstractFile::file )
		{
			if ( !owner )
				continue;
			
			DevFile *file = qobject_cast<DevFile*>(ab);
			
			if ( !file )
				continue;
			
			QStringList m;
			QString tmp, op; 
			DevVarsMap::iterator i_var;
			DevVariable::iterator i_val;
			
			m<<"="<<"+="<<"*=";
			tmp = DevFileInfo(name).relative(owner->project()->name());
			
			(*owner)[file->v]["+="]<<tmp;
			
			i_var = oldown->find(file->v);
			
			if ( i_var == oldown->end() )
				continue;
			
			foreach ( op, m )
			{
				i_val = i_var->find(op);
				
				if ( i_val == i_var->end() )
					continue;
				
				i_val->removeAll(tmp);
			}
			
		} else if ( flag == AbstractFile::folder ||
					flag == AbstractFile::scope ) {
			if ( !owner )
				continue;
			
			owner->nest.insert(name, oldown->nest.take(name) );
			
		} else {
			continue;
		}
		
		old->takeChild( old->indexOfChild(i) );
		top->addChild(i);
		
	}
	
	return true; //QTreeWidget::dropMimeData(parent, index, data, action);
}


