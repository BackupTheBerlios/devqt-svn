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

#ifndef _DEV_PROJECT_H_
#define _DEV_PROJECT_H_

#include "dev.h"

#include "devscope.h"

class DevEdit;

/*

AbstractFile and heirs are meant to handle every signal and slots related stuff
inside QTreeWidget. Each instance of these class is linked to a QTreeWidgetItem,
using a QHash, once again.

The AbstractFile::FileFlag enum is the core of an ugly looking but working
communication system, never change AbstractFile::f field !!!

*/

class AbstractFile : public QObject
{
	Q_OBJECT
	
	friend class DevWorkSpace;
	
	public:
		enum FileFlag
		{
			abstract,
			file,
			folder,
			scope,
			project
		};
		
		AbstractFile(const QString& name);
		virtual ~AbstractFile();
		
		FileFlag flag() const;
		const QString& name() const;
		
		void menu(const QPoint& pos);
		
	signals:
		void rename(AbstractFile *o);
		void deletion(AbstractFile *o);
		
	protected slots:
		void ren();
		void del();
		
	protected:
		QString		n;
		FileFlag	f;
		QMenu 		*m;
		
		DevScope	*o;
		
		QAction *aDel,
			*aRen;
		
};
	
class DevFile : public AbstractFile
{
	Q_OBJECT
	
	friend class DevWorkSpace;
	
	public:
		DevFile(const QString& name, DevEdit *edit);
		virtual ~DevFile();
		
	protected:
		QString v;
		QPointer<DevEdit> e;
};

class DevFolder : public AbstractFile
{
	Q_OBJECT
	
	public:
		DevFolder(const QString& name);
		virtual ~DevFolder();
		
	signals:
		void addFile(DevFolder *o);
		void subFolder(DevFolder *o);
		
	protected slots:
		void add();
		void sub();
		
	protected:
		QAction *aSub,
			*aNew;
		
};

/*

DevDirectory ( bad name but, well... Who cares about such a low level class? ),
is a restricted DevFolder, dseigned for targets representation.

*/

class DevDirectory : public DevFolder
{
	Q_OBJECT
	
	friend class DevWorkSpace;
	
	public:
		DevDirectory(const QString& name);
		virtual ~DevDirectory();
		
};

/*

DevProject is the lowest level project manager (lower level classes only perform
generic stuffs). It holds the project parser and a few routines usefull for the
GUI part of the project managing.

*/

class DevProject : 	public DevFolder
{
	Q_OBJECT
	
	friend class DevGUI;
	friend class DevWorkSpace;
	
	public:
		enum ParsingState
		{
			None				= 0x00,		//nothing special
			MultiLineVariable	= 0x01,		//multiline variable assignement
			SingleLineScope		= 0x02,		//single line scope
			MultiLineScope		= 0x04,		//multiline scope
			NegatedScope		= 0x08		//negated scope (not implemented yet)
		};
		
		typedef QFlags<ParsingState> ParserState;
		
		DevProject(const QString& name);
		virtual ~DevProject();
		
		QString content();
		
		QStringList scopes();
		QStringList content(const QString& var, const QString& scope);
		QStringList content(const QString& var, bool files = false);
		
	protected:
		void setup(const QString& data);
		
		void close(DevScope *s);
		
		void recurse(const DevScope *p, const QString& v, QStringList& l, bool f);
		void insert(QStringList& l, const QString& s, bool u, bool f);
		
	private:
		DevScope *global;
};

#endif
