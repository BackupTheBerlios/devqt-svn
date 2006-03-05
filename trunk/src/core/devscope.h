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

/*

These class are the core of project management.
Basically a *.pro is considered as a tree of scopes with a global scope at top.
Each scope is a map of variable (name as key, content as value), each variable
being a map itslef.

Actually no QMap are used because we don't care about ordering vaiables by name,
but we do care about the speed gain brought by QHash.

So as to get a tree structure, each scope has to hold a list of children. In
order to allow easy navigation through project data, scope do not have names
because their names are hold by their parent in a map with name as key and
pointer to child as value.

To give the highest flexibility, variables are not QStringList but maps of
operators and corresponding value. That way of storage allow to keep intact any
variable and to modify each aspect, not only a punctually calculated value.

This example project is not a typical one AFAIK, but It could become one easily.
I doubt many people make an expensive use of scopes but that is because there
wasn't any IDE that smartly used them before DevQt does. Scopes are a great
advantage of *.pro format because the allow both targets and folder. Indeed, a
scope which is set in the CONFIG variable as follow, will be treated by qmake
as if it wasn't present in the project file :
	
# Project.pro :
	
	CONFIG +=	qt thread release \ 		# don't care about this line
				scope child_scope_1 \		# but look at this one!!!
				child_scope_2				# and this on as well!!!
	
	VARIABLE_X = ...						# basic variable assignment
	VARIABLE_Y = ...
	VARIABLE_Z = ...
	
	scope {									# first folder
		
		VARIABLE_X += ...					# variable in folder
		VARIABLE_Y += ...
		
		child_scope_1 {						# child folder 1
			
			VARIABLE_Z += ...				# variable in child folder
			VARIABLE_X += ...
			
			child_scope_3 {					# optionnal target
				
				VARIABLE_Y -= ...			# remove from variable
				
			}								# ! optionnal target
						
		}									# ! child folder 1
		
		child_scope_2 {						# child folder 2
			
			VARIABLE_Z ~= ...				# replace in variable
			
		}									# ! child folder 2
		
	}										# ! first folder

# EOF
	
Since a project file is represented as a tree of scopes holding variable, this
property of scopes allow an IDE to create and read project files where data is
ordered in folders.

The file above will give the following (pseudo) data structure :
	
DevProject *project {
	
	DevScope *global {
		
		QHash<QString, DevVariable> {
			
			"VARIABLE_X", DevVariable {
				
							QHash<QString, QStringList> {
								
								"+=", QStringList { ... }
								
							}
							
			}
			
			"VARIABLE_Y", DevVariable {
				
							QHash<QString, QStringList> {
								
								"+=", QStringList { ... }
								
							}
							
			}
			
			"VARIABLE_Z", DevVariable {
				
							QHash<QString, QStringList> {
								
								"+=", QStringList { ... }
								
							}
							
			}
			
		}
		
		QHash<QString, DevScope*> nest {
			
			"scope", DevScope* {
							QHash<QString, DevVariable> {
								
										"VARIABLE_Z", DevVariable {
											
													QHash<QString, QStringList> {
															
															"+=", QStringList { ... }
															
													}
								
										"VARIABLE_X", DevVariable {
											
													QHash<QString, QStringList> {
															
															"+=", QStringList { ... }
															
													}
													
							}
							
							...
							
			}
			
		}
		
	}
	
}

DevQt automatically check if a scope is set in the CONFIG variable, create the
corresponding tree item and update the target combobox correctly.

*/

typedef QStringList DevValues;

class DevVariable : public QHash<QString, DevValues>
{
	public:
		DevVariable();
};


typedef QHash<QString, DevVariable>	DevVarsMap;

class DevScope;
typedef QHash<QString, DevScope*> 	DevScopeMap;


class DevScope : public QObject, public DevVarsMap
{
	friend class DevProject;
	friend class DevWorkSpace;
	
	public:
		DevScope(DevProject *project, DevScope *parent = 0);
		virtual ~DevScope();
		
		DevProject* project() const;
		
		DevScope* parent() const;
		DevScope* nested(const QString& name);
		
		QStringList scopes();
		void scopes(QStringList& l, const QString& prefix = QString());
		
		QString content(int indent);
		
		void calculate(const QString& var, QStringList& l);
		void content(const QString& var, QStringList& l);
		
		void content(const QString& var, const QStringList& scopes, QStringList& l);
		
	private:
		DevScope *par;
		DevProject *pro;
		
		DevScopeMap nest;
};

#endif
