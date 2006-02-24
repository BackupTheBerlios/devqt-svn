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

#ifndef _DEV_SCOPE_H_
#define _DEV_SCOPE_H_

#include "dev.h"

#ifdef _ORDERED_
typedef QMap<int, QString> DevValues;
#else
typedef QStringList DevValues;
#endif

class DevVariable : public QHash<QString, DevValues>
{
	public:
		DevVariable();
};


typedef QHash<QString, DevVariable>	DevVarsMap;

class DevScope;
class DevProject;
typedef QHash<QString, DevScope*> 	DevScopeMap;


class DevScope : public DevVarsMap
{
	friend class DevProject;
	friend class DevWorkSpace;
	
	public:
		DevScope(DevProject *project, DevScope *parent = 0);
		virtual ~DevScope();
		
		DevProject* project() const;
		
		DevScope* parent() const;
		DevScope* nested(const QString& name);
		
		void scopes(QStringList& l, const QString& prefix = QString());
		
		QString content(int indent);
		
		void calculate(const QString& var, QStringList& l);
		void content(const QString& var, QStringList& l);
		
	private:
		DevScope *par;
		DevProject *pro;
		
		DevScopeMap nest;
};

#endif
