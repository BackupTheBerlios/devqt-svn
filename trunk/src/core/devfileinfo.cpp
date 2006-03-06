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

#include "devfileinfo.h"


DevFileInfo::DevFileInfo(const QString& f)
{
	s = f;
	
	s.replace('\\', '/');
}

DevFileInfo::DevFileInfo(const QString& file, const QStringList& dirs)
{
	QString f(file), dir, folder;
	QStringList l, tmp, d(dirs);
	
	d<<f.replace('\\', '/');
	
	foreach (dir, d)
	{
		tmp = dir.replace('\\', '/').split('/', QString::SkipEmptyParts);
		
		foreach (folder, tmp)
		{
			if ( folder == "." )
				continue;
			else if ( folder == ".." )
				l.removeLast();
			else
				l<<folder;
		}
	}
	
// TODO
// I know this code works for me, and the original worked for FMC
// I have no idea what this code does, I just know this fixes it.
// 
// - diego
#ifdef Q_WS_WIN
	s = l.join("/");
#else
	s = "/" + l.join("/");
#endif
}

QString DevFileInfo::path() const	//path only
{
	return s.section('/', 0, -2);
}

QString DevFileInfo::base() const	//name only
{
	return s.section('/', -1).section('.', 0, 0);
}

QString DevFileInfo::ext () const	//ext only
{
	return s.section('/', -1).section('.', 1);
}

QString DevFileInfo::file() const	//path + name
{
	return s.section('.', 0, 0);
}

QString DevFileInfo::name() const 	//name + ext
{
	return s.section('/', -1);
}

QString DevFileInfo::all () const 	//path + name + ext
{
	return s;
}

QString DevFileInfo::relative(const QString& other)
{
	if ( other.isEmpty() )
		return QString::null;
	
	QFileInfo src(s), dst( ( other.isEmpty() ? "." : other ) );
	
	src.makeAbsolute();
	dst.makeAbsolute();
	
	QString tmp, sDst, sSrc = src.absoluteFilePath();
	
	if ( dst.isFile() )
		sDst = dst.absolutePath(); // A path
	else
		sDst = dst.absoluteFilePath();
	
	if ( sSrc.left( sDst.length() ) == sDst )
	{
		sSrc = sSrc.remove( 0, sDst.length() +1 );
		return sSrc;
	}
	
#ifdef Q_WS_WIN 

	// not same drive?
	if ( sSrc.left( 3 ) != sDst.left( 3 ) )
		return s;
	
	sSrc = sSrc.remove(0, 3); // remove drive
	sDst = sDst.remove(0, 3);
#else
	sSrc = sSrc.remove(0, 1); // remove racine
	sDst = sDst.remove(0, 1);
#endif
	
	int last = sDst.length();
	
	for ( int i = sDst.length() -1; i > -1; i-- )
	{
		if ( sDst[i] == '/' )
		{
			tmp += "../";
			last = i + 1;
		}
		
		if ( sSrc.indexOf( sDst.left(last) ) != -1 )
		{
			sSrc = tmp.append( sSrc.mid(i+1) );
			return sSrc;
		}
	}
	
	for ( int i = 0; i < sDst.count("/"); i++ )
		tmp += "../";
	
	sSrc.prepend(tmp);
	
	return sSrc;
}
