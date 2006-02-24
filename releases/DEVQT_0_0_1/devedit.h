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

#ifndef _DEV_EDIT_H_
#define _DEV_EDIT_H_

#include "dev.h"

#include "devdialogs.h"
#include "devhighlighter.h"

class DevEdit : public QTextEdit
{
	Q_OBJECT
	
	friend class DevEditor;
	
	public:
		enum OpenType
		{
			fromString,
			fromFile
		};
		
		DevEdit(QWidget *parent = 0);
		virtual ~DevEdit();
		
		QString name() const;
		
		int count();						//return number of lines
		int line(const QTextBlock& b);		//line number of block
		
		void indent();
		
		QObject* contextObject() const;
		
	public slots:
		virtual void load();
		virtual void save();
		virtual void saveAs();
		virtual void print();
		
		void find();
		void findNext();
		void replace();
		
		void setText(const QString& s, OpenType t = fromString);
		
		void scrollTo(const QString &txt, const QString &first);
		void setContextObject(QObject *o);
		
		void toggleError(int line);
		void toggleError(QTextCursor& cur);
		void toggleBreakPoint(int line);
		void toggleBreakPoint(QTextCursor& cur);
		
		void clearError();
		
	signals:
		void propertiesRequested();
		void statusChanged(const QString& s);

	protected:
		virtual void paintEvent(QPaintEvent* e);
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseReleaseEvent(QMouseEvent *e);
		virtual void mouseDoubleClickEvent(QMouseEvent *e);
		virtual void contextMenuEvent(QContextMenuEvent *e);
		
	protected slots:
		void customPaste();
		void delSelect();
		void toggleBPt();
		void properties();
		
		void highlight();
		
	private slots:
		
		void checkExtension(QString& f, QString& ext);
		
	private:
		QMenu *menu;
		
		QAction *aCut,
			*aCopy,
			*aPaste,
			*aSelectAll,
			*aDelete,
			*aUndo,
			*aRedo,
			*aFind,
			*aFindNext,
			*aReplace,
			*aGoto,
			*aBreakPoint,
			*aLoad,
			*aSave,
			*aSaveAs,
			*aPrint,
			*aProp;

		DevHighlighter	*hl;
		QPointer<QObject> context;
		
		QString _name;
		
		QTextCursor prev;
		
		DevGotoDialog *gotoDlg;
		DevFindDialog *findDlg;
		DevReplaceDialog *replaceDlg;
};

#endif
