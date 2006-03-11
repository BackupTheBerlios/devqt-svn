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

#include "devproject.h"

#include "devedit.h"
#include "devfileinfo.h"

AbstractFile::AbstractFile(const QString& name)
 : n(name), f(abstract), o(0)
{
	m = new QMenu;
	
	aDel = new QAction( QIcon( ":/remove.png" ), tr("Re&move"), this);
	connect(aDel, SIGNAL( triggered() ),
			this, SLOT  ( del() ) );
	
	aRen = new QAction( QIcon( ":/edit.png" ), tr("&Rename"), this);
	connect(aRen, SIGNAL( triggered() ),
			this, SLOT  ( ren() ) );
	
}

AbstractFile::~AbstractFile()
{
	n.clear();
	delete m;
}

AbstractFile::FileFlag AbstractFile::flag() const
{
	return f;
}

const QString& AbstractFile::name() const
{
	return n;
}

void AbstractFile::menu(const QPoint& pos)
{
	m->exec(pos);
}

void AbstractFile::del()
{
	emit deletion(this);
}

void AbstractFile::ren()
{
	emit rename(this);
}

DevFile::DevFile(const QString& name, DevEdit *edit)
 : AbstractFile(name), e(edit)
{
	f = file;
	
	QAction *a = new QAction(QString( tr("file : ") ) + name, this);
	a->setEnabled(false);
	
	m->addAction(a);
	m->addSeparator();
	m->addAction(aDel);
	m->addAction(aRen);
}

DevFile::~DevFile()
{
	;
}

DevFolder::DevFolder(const QString& name)
 : AbstractFile(name)
{
	f = folder;
	
	aSub = new QAction( QIcon( ":/folder_new.png" ), tr("&Subdir"), this);
	connect(aSub, SIGNAL( triggered() ),
			this, SLOT  ( sub() ) );
	
	aNew = new QAction( QIcon( ":/add.png" ), tr("&New file"), this);
	connect(aNew, SIGNAL( triggered() ),
			this, SLOT  ( add() ) );
	
	
	QAction *a = new QAction(QString( tr("folder : ") ) + name, this);
	a->setEnabled(false);
	
	m->addAction(a);
	m->addSeparator();
	m->addAction(aSub);
	m->addAction(aNew);
	m->addSeparator();
	m->addAction(aRen);
	m->addAction(aDel);
}

DevFolder::~DevFolder()
{
	;
}

void DevFolder::sub()
{
	emit subFolder(this);
}

void DevFolder::add()
{
	emit addFile(this);
}

DevDirectory::DevDirectory(const QString& name)
 : DevFolder(name)
{
	f = scope;
	
	QAction *a = new QAction(QString( tr("scope : ") ) + name, this);
	a->setEnabled(false);
	
	m->clear();
	
	m->addAction(a);
	m->addSeparator();
	m->addAction(aSub);
	m->addAction(aNew);
}

DevDirectory::~DevDirectory()
{
	;
}

DevProject::DevProject(const QString& name)
 : DevFolder(name)
{
	f = project;
	
	aDel->setIcon( QIcon(":/close.png") );
	aDel->setText( tr("&Close") );
	
	global = new DevScope(this, 0);
	
	QFile f(name);
	
	if ( f.open(QFile::ReadOnly | QFile::Text) )
		setup( f.readAll() );
}

DevProject::~DevProject()
{
	delete global;
}

void DevProject::setup(const QString& data)
{
	ParserState state = None;
	DevScope *scope = global;
	
	QChar c;
	int i=0, j;
	QString var, tmp, op;
	QStringList lines, words, dest;
	
	QStack<int> nest_len;
	
	lines = data.split('\n');
	
	while ( i<lines.count() )
	{
		QString line = lines[i++];
		
		if ( line.startsWith("#") || line.isEmpty() )
			continue;
		
		words.clear();
		
		bool isVar = false;
		const QChar *l = line.data();
		int n = -1, count = line.count(), paren = 0;
		
		while ( n < count )
		{
			do
			{
				c = l[++n];
			} while ( c.isSpace() && (n < count) );
			
			if ( n >= count )
				break;
			
			isVar = false;
			int j = n;
						
			do
			{
				if ( c.isSpace() )
					break;
				else if ( c == '$' )
					isVar = true;
				else if ( (c == '(') || (c == '{') || (c == '[') )
				{
					if ( isVar )
						paren++;
					else
						break;
				} else if ( (c == ')') || (c == '}') || (c == ']') ) {
					if ( isVar )
					{
						if ( paren )
							paren--;
						else
							QMessageBox::warning(0, "Parsing error : ", "Wrong parenthesis use!");
						
						if ( !paren )
							isVar = false;
					} else {
						break;
					}
				} else if ( (c == '~') || (c == '-') || (c == '+') ||
							(c == ':') || (c == '=') || (c == '*') ||
							(c == '#')  ) {
					break;
				}
				
				c = l[++n];
			} while ( n < count );
			
			if ( n != j )
			{
				//QMessageBox::warning(0, c, QString(l+j, n-j));
				words<<QString(l+j, n-j);
			}
			
			if ( c == '#' )
			{
				break;
			} else 	if ((c == '*') || (c == '+') ||
				 		(c == '-') || (c == '~') ) {
				if ( l[n+1] == '=' )
					words<<QString(c)+l[++n];
			} else if ( (c == '{') || (c == '}') ||
						(c == '(') || (c == ')') ||
						(c == '[') || (c == ']') ||
						(c == ':') || (c == '!') ||
						(c == '=') ) {
				words<<c;
			}
		}
		
		j = 0;
		
		if ( words.isEmpty() )
			continue;
		
		//QMessageBox::warning(0, "words", words.join("\n"));
		
		if ( !scope )
			scope = global;
		
		if ( state & MultiLineVariable )
			state &= ~MultiLineVariable;
		else
		{
			if ( words[j] == "}" )
			{
				if ( nest_len.isEmpty() )
					QMessageBox::warning(0, "Parser error : ",
										"No scope to close on line : " + 
										QString::number(i));
				else
				{
					for ( int n = nest_len.pop(); n && scope; n--)
					{
						DevScope *temp = scope->parent();
						scope = temp;
					}
					
					state &= ~( SingleLineScope | MultiLineScope );
				}
				
			} 
			
			if ( words[j] == "!" )
				j++;
			
			if (((j+1) < words.count()) &&
				((words[j+1] == ":") || (words[j+1] == "{")) )
			{
				tmp = words[++j];
				
				DevScope *temp = scope->nested(words[j-1]);
				scope = temp;
				
				if ( tmp == ":")
				{
					int n = 1;
					
					if ( (words[j+2] == ":") || (words[j+2] == "{") )
					{
						do
						{
							j++;
							
							DevScope *temp = scope->nested(words[j]);
							scope = temp;
							j++;
							n++;
							
							if ( (j+2) >= words.count() )
								break;
							
						} while ( ((words[j+2] == ":") || (words[j+2] == "{")) );
						
						nest_len.push(n);
					}
					
					if ( words[j] == "{" )
						state |= MultiLineScope;
					else
						state |= SingleLineScope;
					
					j++;
					
				} else if ( tmp == "{" ) {
					nest_len.push(1);
					state |= MultiLineScope;
					continue;
				} else {
					continue;
				}
			}
			
			if ( (j+2) >= words.count() )
				continue;
			
			var = words[j].toUpper();
			
			op = words[++j];
			j++;
			
			if ( op == "(" )
			{
				//QMessageBox::warning(0, "function encountered", var);
				continue;
			}
			
			if ( !scope )
				scope = global;
				
			dest = (*scope)[var].value(op);
		}
		
		while ( j < words.count() )
		{
			tmp = words[j++];
			
			if ( (j == words.count()) && (tmp == "\\") )
			{
				state |= MultiLineVariable;
				break;
			}
			
			if ( tmp == "}" )
			{
				if ( nest_len.isEmpty() )
					QMessageBox::warning(0, "Parser error : ",
										"No scope to close on line : " + 
										QString::number(i));
				else
				{
					for ( int n = nest_len.pop(); n && scope; n--)
					{
						DevScope *temp = scope->parent();
						scope = temp;
					}
				}
				state &= ~( SingleLineScope | MultiLineScope );
				break;
			}
			
			//QMessageBox::warning(0, "", tmp);
			dest<<tmp;
		}
		
		if ( state & MultiLineVariable )
			continue;
		
		(*scope)[var][op] = dest;
		
		if ( state & SingleLineScope )
		{
			DevScope *temp = scope->parent();
			scope = temp;
			
			state &= ~SingleLineScope;
		}
		
	}
	
}

QString DevProject::content()
{
	QString s;
	
	s += "################################\n";
	s += "#                              #\n";
	s += "#  Project generated by DevQt  #\n";
	s += "#                              #\n";
	s += "################################\n";
	s += "\n";
	
	s += global->content(0);
	
	return s;
}

QStringList DevProject::content(const QString& var, const QString& scope)
{
	QStringList l, scopes;
	
	if ( scope == "all" )
		scopes = global->scopes();
	else
		scopes = scope.split(" & ");
	
	global->content(var, scopes, l);
	
	return l;
}

QStringList DevProject::content(const QString& var, bool files)
{
	QStringList l;
	
	recurse(global, var, l, files);
	
	return l;
}

void DevProject::recurse(const DevScope *p, const QString& v, QStringList& l, bool f)
{
	DevVariable var = (*p)[v];
	QStringList list;
	
	list<<var.value("=");
	list<<var.value("+=");
	list<<var.value("*=");
	
	foreach (QString s, list)
		insert(l, s, true, f);
		
	foreach (const DevScope *s, p->nest.values())
		recurse(s, v, l, f);
}

void DevProject::insert(QStringList& l, const QString& s, bool u, bool f)
{
	int i;
	if ( (i=s.indexOf("$$")) != -1 )
	{
		QString prefix(s.mid(0, i)), suffix(s.mid(i+2));
		
		if ( suffix.startsWith('(') || suffix.startsWith('{') )
			suffix.remove(0, 1);
			
		if ( suffix.endsWith(')') || suffix.endsWith('}') )
			suffix.chop(1);
			
		QStringList list = content(suffix, f);
		
		foreach (QString var, list)
			insert(l, prefix+var, u, f);
		
	} else {
		if ( u && l.contains(s) )
			return;
		
		if ( f )
			l<<DevFileInfo(s, QStringList( QFileInfo(n).path() )).all();
		else
			l<<s;
	}
}

QStringList DevProject::scopes()
{
	QStringList l, c = content("CONFIG", false);
	
	//l<<"all";
	
	global->scopes(l);
	
	foreach (QString s, c)
		l.removeAll(s);
	
	if (!c.contains("release") && !c.contains("debug") &&
		!l.contains("release") && !l.contains("debug") )
		l<<"release"<<"debug";
	
	/*
	foreach (QString s, l)
		if ( s.contains("win32") || s.contains("unix") || s.contains("mac") )
			l.removeAll(s);
	*/
	
	return l;
}
