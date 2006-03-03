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

#include "ui_configdialog.h"
#include "dev.h"

class DevEdit;

class DevSettings : public QSettings
{
	Q_OBJECT
	
	friend class DevApp;
	
	public:
		
		enum Settings
		{
			maxProjects = 5,
			maxFiles = 15
		};
		
		static DevSettings* Instance();
		
		QMenu* recent();
		void applyFormat(DevEdit *e);
		void killSettings();
	public slots:
		void execute();
		void addRecent(const QString& n, bool project = false);
		
	protected slots:
		void recent(QAction *a);
		void clearRecents();
		
	protected:
		DevSettings(QWidget *p = 0);
		virtual ~DevSettings();
		
	private:
		Ui::ConfigurationDlg configDialogUI;
		QDialog *dlg;

		QHash<QAction*, QString> recents;
		
		QMenu *m;
		QAction *aClear;
// 		QPushButton *b_Valid, *b_Cancel, *b_Default;
		
		static DevSettings *inst;
};

/***********
TODO
The way this is implemented is not unclear. While this 
class should only deal with configration, it also deals with the way
it's saved, and even inherits it. It also contains a declaration of a menu
and a dialog - this means mixing GUI with functionalit. This is not a good
idea on the long term.

I do like the singleton implementation, and I do like the 
idea that this class takes are of the recently opened files. 

I propose the following interface:
When the user presses the apply button, this class emits a 
"needUpdate()" signal. All widgets attached to the interface should
connect that signal to an internal function which will read the configration
from this singleton and then apply the changes as necesary.

class DevSettings
{
public:
	QMenu getRecentlyUsedMenu();
	void addRecentFile(const QString& n);
	void addRecentProject(const QString& n);

	void saveConfig();
	void restoreConfig();

	void showConfigDialog();

signal:
	needUpdate();

private:
	QDialog configDialog;

// editor definitions
	bool showLineNubers;
	QFont editorFont;
	... etc

// application definitions
	QPoint	windowSize;
	bool	windowMaximized;
	... etc
}
*/
#endif
