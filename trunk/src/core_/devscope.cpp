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

#include "devscope.h"

#include "devproject.h"

DevVariable::DevVariable()
{
	//QMessageBox::warning(0, "var built", QString::number(++count)+"th");
}

DevScope::DevScope(DevProject *project, DevScope *parent)
 : QObject(parent), par(parent), pro(project)
{
	//QMessageBox::warning(0, "scope built", QString::number(++count)+"th");
}

DevScope::~DevScope()
{
	/*
	Dunno why but those lines caused a crash when closing a project where scopes
	has been moved with drag and drop!
	*/
	//foreach(DevScope *next, nest.values())
	//	delete next;
	
	nest.clear();
	clear();
}

DevProject* DevScope::project() const
{
	return pro;
}

DevScope* DevScope::parent() const
{
	return par;
}

DevScope* DevScope::nested(const QString& name)
{
	DevScopeMap::iterator i = nest.find(name);
	
	if ( i != nest.end() )
		return *i;
	
	DevScope *s = new DevScope(pro, this);
	nest.insert(name, s);
	
	return s;
}

QStringList DevScope::scopes()
{
	QStringList l;
	
	for ( DevScopeMap::iterator i = nest.begin(); i != nest.end(); i++ )
	{
		if ( !l.contains(i.key()) )
			l<<i.key();
		
		QStringList list = (*i)->scopes();
		
		foreach (QString s, list)
			if ( !l.contains(s) )
				l<<s;
	}
	
	return l;
}

void DevScope::scopes(QStringList& l, const QString& prefix)
{
	QString preceding(prefix);
	
	if ( !preceding.isEmpty() )
		preceding += " & ";
	
	for ( DevScopeMap::iterator i = nest.begin(); i != nest.end(); i++ )
	{
		QString pre(preceding);
		
		if ( !pro->content("CONFIG").contains(i.key()) )
			pre += i.key();
		
		if ( !pre.isEmpty() )
			l<<pre;
		
		(*i)->scopes(l, pre);
	}
}

QString DevScope::content(int indent)
{
	QString s;
	QString tab(indent, '\t');
	
	for (DevVarsMap::iterator i = begin(); i != end(); i++)
	{
		DevVariable v = *i;
		
		for (DevVariable::iterator j = v.begin(); j != v.end(); j++)
		{
			QStringList l(*j);
			
			if ( l.isEmpty() )
				continue;
			
			QStringList::iterator k = l.begin();
			
			s += tab + i.key() + " " + j.key() + " " + *(k++) + " \\\n";
			
			for ( ; k != l.end(); k++)
				s += tab + "\t" + (*k) + " \\\n";
			
			s.chop(3);
			
			s += "\n\n"; 
		}
	}
	
	for (DevScopeMap::iterator i = nest.begin(); i != nest.end(); i++)
	{
		s += QString("\n") + tab +  i.key() + " {\n\n";
		s += (*i)->content(indent+1);
		s += tab + "}\n\n";
	}
	
	return s;
}

void DevScope::calculate(const QString& name, QStringList& l)
{
	if ( par )
		par->content(name, l);
	
	content(name, l);
}

void DevScope::content(const QString& name, QStringList& l)
{
	DevVarsMap::iterator i = find(name);
	
	if ( i != end() )
	{	
		DevVariable v = *i;
		
		QStringList list(v.value("="));
		
		if ( !list.isEmpty() )
			l = list;
		
		l<<v.value("+=");
		
		foreach ( const QString s, v.value("*=") )
			if ( !l.contains(s) )
				l<<s;
		
		foreach ( const QString s, v.value("-=") )
			l.removeAll(s);
		
		foreach ( const QString s, v.value("~=") )
		{
			QRegExp f("s/([^/]+)/(.)+");
			
			if ( !s.contains(f) )
				continue;
			
			l.replaceInStrings(QRegExp(f.cap(1)), f.cap(2));
		}
	}
}

void DevScope::content(const QString& var, const QStringList& scopes, QStringList& l)
{
	DevVariable v = value(var);
	
	l<<v.value("=");
	l<<v.value("+=");
	l<<v.value("*=");
	
	foreach (QString s, scopes)
	{
		if ( !nest.contains(s) )
			continue;
		
		(*(nest.find(s)))->content(var, scopes, l);
	}
}

