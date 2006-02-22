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

AbstractFile::AbstractFile(const QString& name)
 : n(name), f(abstract)
{
	m = new QMenu;
	
	QAction *a = new QAction(name, this);
	a->setEnabled(false);
	
	m->addAction(a);
	m->addSeparator();
	
	aDel = new QAction( QIcon( ":/remove.png" ), "Re&move", this);
	connect(aDel, SIGNAL( triggered() ),
			this, SLOT  ( del() ) );
	
	aRen = new QAction( QIcon( ":/edit.png" ), "&Rename", this);
	connect(aRen, SIGNAL( triggered() ),
			this, SLOT  ( ren() ) );
	
}

AbstractFile::~AbstractFile()
{
	n.clear();
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
	
	m->addAction(aDel);
}

DevFile::~DevFile()
{
	;
}

DevFolder::DevFolder(const QString& name)
 : AbstractFile(name)
{
	f = folder;
	
	aSub = new QAction( QIcon( ":/folder_new.png" ), "&Subdir", this);
	connect(aSub, SIGNAL( triggered() ),
			this, SLOT  ( sub() ) );
	
	aNew = new QAction( QIcon( ":/add.png" ), "&New file", this);
	connect(aNew, SIGNAL( triggered() ),
			this, SLOT  ( add() ) );
	
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

DevProject::DevProject(const QString& name)
 : DevFolder(name)
{
	f = project;
	
	QFile f(name);
	
	if ( f.open(QFile::ReadOnly) )
		setup( f.readAll() );
}

DevProject::~DevProject()
{
	;
}

void DevProject::setup(const QString& data)
{
	int i=0, j;
	iterator it;
	bool multi = false;
	QString var, tmp, op;
	QStringList lines, words, dest;
	
	lines = data.split('\n');
	
	while ( i<lines.count() )
	{
		QString line = lines[i];
		if ( line.startsWith("#") || line.isEmpty() )
		{
			i++;
			continue;
		}
		
		words.clear();
		
		QChar c;
		bool isVar;
		const QChar *l = line.data();
		int n = -1, count = line.count();
		
		while ( n < count )
		{
			do
			{
				c = l[++n];
			} while ( c.isSpace() && (n < count) );
			
			isVar = false;
			int j = n;
						
			while ((!c.isSpace() && (n < count ) &&
					(c != '~') && (c != '-') && (c != '{') &&
					(c != '}') && (c != '[') && (c != ']') &&
					(c != ':') && (c != '!') && (c != '=') &&
					(c != '*') && (c != '+') ) ||
					( (c == '(') && (c == ')') && isVar )  )
			{
				if ( isVar && ( (c == ')') || (c == '}') || (c == ']') ) )
					isVar = false;
				if ( c == '$' )
					isVar = true;
				c = l[++n];
			}
			
			if ( n != j )
				words<<QString(l+j, n-j);
			
			if ( (c == '*') || (c == '+') ||
				 (c == '-') || (c == '~') ) {
				if ( l[n] == '=' )
					words<<QString(c)+=l[++n];
			} else if ( (c == '{') || (c == '}') ||
						(c == '(') || (c == ')') ||
						(c == '[') || (c == ']') ||
						(c == ':') || (c == '!') ||
						(c == '=') ) {
				words<<c;
			}
		}
		
		j = 0;
		
		if ( !multi )
		{
			if ( words.count()<3 )
			{
				i++;
				continue;
			}
			
			dest.clear();
			var = words[0].toUpper();
			
			if ( (it = find(var)) != end() )
				dest = *it;
			
			op = words[1];
			j = 2;
			
			if ( op == "=" )
				dest.clear();
		} else {
			multi = false;
		}
		
		if ( (op == "=") || (op == "+=") )
		{
			while ( j<words.count() )
			{
				tmp = words[j++];
				
				if ( (j==words.count()) && (tmp=="\\") )
				{
					multi = true;
					break;
				}
				dest<<tmp;
			}
		} else if ( op == "-=" ) {
			while ( j<words.count() )
			{
				tmp = words[j++];
				
				if ( (j==words.count()) && (tmp=="\\") )
				{
					multi = true;
					break;
				}
				dest.removeAll(tmp);
			}
		} else if ( op == "*=" ) {
			while ( j<words.count() )
			{
				tmp = words[j++];
				
				if ( (j==words.count()) && (tmp=="\\") )
				{
					multi = true;
					break;
				}
				if ( !dest.contains(tmp) )
					dest<<tmp;
			}
		} else if ( op == "~=" ) {
			while ( j<words.count() )
			{
				tmp = words[j++];
				
				if ( (j==words.count()) && (tmp=="\\") )
				{
					multi = true;
					break;
				}
			}
		} else {
			QMessageBox::warning(0, "Parsing error", QString("Unknown operator: ")+op);
		}
		
		i++;
		
		if ( multi )
			continue;
		
		QHash<QString, QStringList>::insert(var, dest);
	}
}

QStringList DevProject::content(const QString& var, bool files)
{
	iterator i = find(var);
	
	if ( i == end() )
		return QStringList();
	
	QString v;
	QStringList l;
	
	foreach(v, *i)
		insert(l, v, true, files);
	
	return l;
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
		
		iterator iter = find(suffix);
		
		if ( iter == end() )
			return;
			
		QStringList list = *iter;
		
		for (int i=0; i<list.count(); i++)
			insert(l, prefix+list[i], u, f);
		
	} else {
		if ( u && l.contains(s) )
			return;
		
		if ( f )
			l<<QFileInfo(QFileInfo(n).path(), s).filePath();
		else
			l<<s;
	}
}
