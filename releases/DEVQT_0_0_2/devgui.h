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

#ifndef _DEV_GUI_H_
#define _DEV_GUI_H_

#include "dev.h"

#include "devmdi.h"
#include "devdock.h"
#include "deveditor.h"
#include "devworkspace.h"

class DevGUI : public QMainWindow
{
	Q_OBJECT
	
	friend class DevApp;
	friend class DevWorkSpace;
	
	public:
		static DevGUI* Instance();
		
	protected:
		DevGUI();
		virtual ~DevGUI();
		
		void setupFileActions();
		void setupEditActions();
		void setupTextActions();
		void setupCompiler();
		void setupExplorer();
		
		bool load(const QString &f);
		bool loadProject(const QString &f);
		
	protected slots:
		void fileNew();
		void fileOpen();
		void fileSave();
		void fileSaveAs();
		void fileSaveAll();
		void filePrint();
		void fileClose();
		
		void textFamily(const QString &f);
		void textSize(const QString &p);
		
		void editorChanged();
		void clipboardDataChanged();
		void fontChanged(const QFont &f);
		
	private:
		DevEditor* createEditor(const QString& title = QString(), bool show = true);
		
		DevDock *Explorer, 
			*Compiler;
			
		DevMDI *Editor;
		
		QTabWidget *tabExplorer,
			*tabCompiler;
		
		QComboBox *Font,
			*Size;
		
		DevWorkSpace *treeFiles;
		QTreeWidget	*treeClasses;
		
		QAction
			*actionSave,
			*actionSaveAll,
			*actionUndo,
			*actionRedo,
        	*actionCut,
        	*actionCopy,
			*actionPaste,
			*actionCompile,
			*actionRebuild,
			*actionRun,
			*actionClose,
			*actionCmpRun,
			*actionDebug;
			
		long noname_count;
			
		static DevGUI *_gui;
};

#endif
