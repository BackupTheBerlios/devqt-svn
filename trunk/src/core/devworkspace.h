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

#ifndef _DEV_WORKSPACE_H_
#define _DEV_WORKSPACE_H_

#include "dev.h"

#include "devproject.h"

/*

The real project manager (dragging, deleting, renaming, adding, for both files
and folders, everything is handled by the workspace, which is, for simlicity, a
QTreeWidget itself.

Low level stuff are eplained in devscope.h

*/

typedef QHash<QTreeWidgetItem*, AbstractFile*> 	DevTreeMap;
typedef QHash<QString, DevProject*>				DevProjectMap;

class DevWorkSpace : 	public QTreeWidget,
						protected DevTreeMap,
						public DevProjectMap
{
	Q_OBJECT
	
	friend class DevProject;
	
	public:
		DevWorkSpace(QString name = QString(), QWidget *p = 0);
		virtual ~DevWorkSpace();
		
		DevProject* find(const QString& n);
		AbstractFile* find(QTreeWidgetItem *i);
		
	public slots:
		void loadWorkSpace(const QString& name);
		
		bool addProject(const QString& name);
		
		/*
		bool addFolder(	const QString& name,
						const QString& project);
		
		bool addFile(	const QString& name,
						const QString& project,
						const QString& folder);
		*/
		void rename(AbstractFile *f, const QString& name);
		
		bool newProject(const QString& name);
		bool closeProject(const QString& name);
		
	protected slots:
		void deletion(AbstractFile *o);
		void rename(AbstractFile *o);
		void addFile(DevFolder *p);
		void subFolder(DevFolder *p);
		
		void focus(QTreeWidgetItem *i, int c);
		
	protected:
		bool dropMimeData (	QTreeWidgetItem *parent, int index,
							const QMimeData *data, Qt::DropAction action);
		
		virtual void contextMenuEvent(QContextMenuEvent *e);
		
	private:
		void createNested(	DevScope *s, const QStringList& vars, 
							const QStringList& cfg, QTreeWidgetItem *i);
		
		QString n;
};

#endif
