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

#ifndef _DEV_SETTINGS_H_
#define _DEV_SETTINGS_H_

#include "dev.h"

class DevEdit;
class DevSettingsDialog;

class DevSettings : public QSettings
{
	Q_OBJECT
	
	friend class DevApp;
	
	public:
		
		enum Settings
		{
			//remember openned files and projects
			Forget,				//none
			Active,				//only active one
			All,				//all
			
			//deprecated values
			maxProjects = 5,	//max number of recent projects
			maxFiles = 15		//max number of recent files
			
		};
		
		static DevSettings* Instance();
		void killSettings();
		
		QMenu* recent();
		void applyFormat(DevEdit *e);
		
		int tabStop();
		
		QString make();
		QStringList environment(const QStringList& dirs = QStringList());
		QStringList includes();
		
	public slots:
		void execute();
		void addRecent(const QString& n, bool project = false);
		
	protected slots:
		void clearRecents();
		void recent(QAction *a);
		
	protected:
		DevSettings(QWidget *p = 0);
		virtual ~DevSettings();
		
	private:
		QHash<QAction*, QString> recents;
		
		DevSettingsDialog *dlg;
		
		QMenu *m;
		QAction *aClear;
		
		static DevSettings *inst;
		static const QString PATH_VAR;
};


#endif
