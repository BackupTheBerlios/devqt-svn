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

class DevDock;
class DevEdit;
class CoreEdit;
class DevCorner;
class DevStatus;
class DevGotoDialog;
class DevFindDialog;
class DevReplaceDialog;
class DevPropertiesDialog;

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
		
		bool load(const QString &f);
		bool loadProject(const QString &f);
		
		virtual void closeEvent(QCloseEvent *e);
		
	protected slots:
		void fileNew();
		void fileOpen();
		void fileSave(const QString& n = QString(), const QString& ext = QString());
		void fileSaveAs();
		void fileSaveAll();
		
		void undo();
		void redo();
		
		void cut();
		void copy();
		void paste();
		void selectAll();
		void delSelect();
		
		void print();
		
		void find();
		void findNext();
		void replace();
		void goTo();
		
		void breakpoint();
		
		void close();
		void closeAll();
		
		void properties();
		
		//void textFamily(const QString &f);
		//void textSize(const QString &p);
		
		void editorChanged();
		void clipboardDataChanged();
		//void fontChanged(const QFont &f);
		
		void editorModified(bool mod);
		void editorMenu(const QPoint& pos);
		
	private:
		void setupFileActions();
		void setupEditActions();
		//void setupTextActions();
		void setupCompiler();
		void setupExplorer();
		void setupMenu();
		
		void maybeSave();
		
		void checkExtension(QString& f, const QString& ext);
		
		DevEdit* createEditor(const QString& title = QString(), bool show = true);
		
		QPointer<DevEdit> e;
		
		QMenu *menu;
		DevStatus *s;
		DevCorner *c;
		
		DevGotoDialog *gDlg;
		DevFindDialog *fDlg;
		DevReplaceDialog *rDlg;
		DevPropertiesDialog *pDlg;
		
		DevDock *Explorer, 
			*Compiler;
		
		QTabWidget *Editor,
			*tabExplorer,
			*tabCompiler;
		
		//QComboBox *Font,
		//	*Size;
		
		DevWorkSpace *treeFiles;
		QTreeWidget	*treeClasses;
		
		QAction *aNew,
			*aOpen,
			*aSave,
			*aSaveAs,
			*aSaveAll,
			*aFind,
			*aFindNext,
			*aReplace,
			*aGoto,
			*aBreakPoint,
			*aProp,
			*aDelete,
			*aSelectAll,
			*aPrint,
			*aClose,
			*aExit,
			*aUndo,
			*aRedo,
        	*aCut,
        	*aCopy,
			*aPaste,
			*aCompile,
			*aRebuild,
			*aRun,
			*aCmpRun,
			*aDebug;
			
		long noname_count;
			
		static DevGUI *_gui;
};

#endif
