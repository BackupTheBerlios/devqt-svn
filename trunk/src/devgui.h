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
class DevWorkSpace;
class DevGotoDialog;
class DevFindDialog;
class DevAboutDialog;
class DevReplaceDialog;
class DevPropertiesDialog;

class DevGUI : public QMainWindow
{
	Q_OBJECT
	
	friend class DevApp;
	friend class DevStatus;
	friend class DevSettings;
	friend class DevWorkSpace;
	
	public:
		static DevGUI* Instance();
		void killGUI();
		QString getFontFamily() { return fontFamily; }
		qreal	getFontSize() { return fontSize; }
		Qt::GlobalColor	getNumberBrush() { return numberBrush; }
		Qt::GlobalColor	getCommentBrush() { return commentBrush; }
		Qt::GlobalColor	getKeywordBrush() { return keywordBrush; }
		Qt::GlobalColor	getQuoteBrush() { return quoteBrush; }
		Qt::GlobalColor	getPreprocessorBrush() { return preprocessorBrush; }
	protected:
		DevGUI();
		virtual ~DevGUI();
		
		bool load(const QString& f);
		bool loadProject(const QString& f);
		/*
		bool loadUI(const QString& f);
		bool loadQRC(const QString& f);
		bool loadTemplate(const QString& f);
		*/
		
		virtual void closeEvent(QCloseEvent *e);
		
	protected slots:
		void fileNew();
		void fileOpen();
		void fileSave(const QString& n = QString(), const QString& ext = QString());
		void fileSaveAs();
		void fileSaveAll();
		
		void projectNew();
		void projectSave();
		void projectSaveAs();
		
		/*
		void uiNew();
		void uiSave();
		void uiSaveAs();
		
		void qrcNew();
		void qrcSave();
		void qrcSaveAs();
		
		void templateNew();
		void templateSave();
		void templateSaveAs();
		*/
		
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
		
		void compile();
		void compileCur();
		void run();
		void params();
		
		void compRun();
		void rebuild();
		void syntax();
		
		void clean();
		
		void aboutQt();
		void QtAssistant();
		
		void editorChanged();
		void clipboardDataChanged();
		
		void editorModified(bool mod);
		void editorMenu(const QPoint& pos);
		
	private slots:
		void changeProject(const QString& f);
		
		void hideCompiler();
		void hideExplorer();
		
	private:
		void setupFileActions();
		void setupEditActions();
		void setupSearchActions();
		void setupHelpActions();
		void setupProjectActions();
		void setupSettings();
		
		void setupCompiler();
		void setupExplorer();
		
		void setupMenu();
		
		void maybeSave();
		
		void checkExtension(QString& f, const QString& ext);
		
		DevEdit* createEditor(const QString& title = QString(), bool show = true);
		
		QPointer<DevEdit> e;
		
		QMenu *menu;
		DevStatus *s;
		
		QToolButton *add, *rem;
		
		DevGotoDialog 	*gDlg;
		DevFindDialog 	*fDlg;
		DevAboutDialog 	*aDlg;
		
		DevReplaceDialog 	*rDlg;
		DevPropertiesDialog *pDlg;
		
		DevDock *Explorer, 
			*Compiler;
		
		QTabWidget *Editor,
			*tabExplorer,
			*tabCompiler;
		
		//QComboBox *Font,
		//	*Size;
		
		QComboBox *project,
			*config;
		
		DevWorkSpace *workspace;
		QTreeWidget	*treeClasses;
		
		QAction *aNewFile,
			*aNewProject,
			*aNewTemplate,
			*aNewUI,
			*aNewQRC,
			*aOpen,
			*aSave,
			*aSaveAs,
			*aSaveAll,
			*aSaveProject,
			*aSaveProjectAs,
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
			*aCloseAll,
			*aCloseProject,
			*aExit,
			*aUndo,
			*aRedo,
			*aCut,
			*aCopy,
			*aPaste,
			*aCompile,
			*aCompileCur,
			*aRun,
			*aParams,
			*aCompRun,
			*aRebuild,
			*aSyntax,
			*aClean,
			*aSettings,
			*aAboutQt,
			*aQtAssistant,
			*aAboutDevQt,
			*aDevQtAssistant;
			
		long noname_count;
			
		static DevGUI *_gui;

		// Variables which store the default font family and size for the editor windows
		QString	fontFamily;
		qreal	fontSize;
		// Variables containing the colors for the syntax highlighter
		Qt::GlobalColor	numberBrush;
		Qt::GlobalColor	quoteBrush;
		Qt::GlobalColor	preprocessorBrush;
		Qt::GlobalColor	keywordBrush;
		Qt::GlobalColor	commentBrush;
		
};

#endif
