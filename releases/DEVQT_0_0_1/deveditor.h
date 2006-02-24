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

#ifndef _DEV_EDITOR_H_
#define _DEV_EDITOR_H_

#include "dev.h"

#include "devedit.h"
#include "devstatus.h"
#include "devlinenumber.h"

class DevEditor : public QWidget
{
	Q_OBJECT
	
	public:
		DevEditor(QWidget *p = 0);
		DevEditor(	const QString& s,
					DevEdit::OpenType t = DevEdit::fromString,
					QWidget *p = 0 );
		
		QString name() const;
		
		QString text() const;
		
		QTextCursor textCursor() const;
		const QTextDocument* document() const;
		
		bool isUndoAvailable() const;
		bool isRedoAvailable() const;

		void indent();
		void readSettings();

		//void clearError();
		//void clearStackFrame();
		
		bool isModified() const;
		
		int count();						//return number of lines
		int lines();						//do the same!!!
		
	public slots:
		virtual void load();
		virtual void save();
		virtual void saveAs();
		virtual void print();
		
		void setFont(const QFont& f);
		void setText(const QString& s, DevEdit::OpenType t = DevEdit::fromString);
		
		void toggleError(int line);
		void toggleBreakPoint(int line);
		//void setStackFrame(int line);
		
		void setModified(bool b);
		
		void undo();
		void redo();
		void cut();
		void copy();
		void paste();
		void selectAll();
		
		void find();
		void findNext();
		void replace();
		
		void gotoLine(int line);
		
		void scrollTo(const QString &txt, const QString &first);
		void setContext(QObject *ctx);
		
	signals:
		//QTextEdit signals provided for convinience :
		void copyAvailable(bool yes); 
		void currentCharFormatChanged(const QTextCharFormat& f); 
		void cursorPositionChanged() ;
		void redoAvailable(bool available);
		void selectionChanged();
		void textChanged();
		void undoAvailable(bool available);
		
		//QTextEdit's scrollbars' signals wrapper :
		void actionTriggered(int action, Qt::Orientation o);
		void rangeChanged(int min, int max, Qt::Orientation o);
		void sliderMoved(int value, Qt::Orientation o);
		void sliderPressed(Qt::Orientation o);
		void sliderReleased(Qt::Orientation o);
		void valueChanged(int value, Qt::Orientation o);
		
		//QStatusBar signals provided for convinience :
		void messageChanged(const QString& message);
		
		//DevLineNumber signals provided for convinience :
		void clicked(int line);
		
	protected slots:
		//used to wrap scrollbars' signals
		void h_actionTriggered(int action);
		void h_rangeChanged(int min, int max);
		void h_sliderMoved(int value);
		void h_sliderPressed();
		void h_sliderReleased();
		void h_valueChanged(int value);
		
		void v_actionTriggered(int action);
		void v_rangeChanged(int min, int max);
		void v_sliderMoved(int value);
		void v_sliderPressed();
		void v_sliderReleased();
		void v_valueChanged(int value);
		
	private:
		void setup();
		
		QTextCursor old;
		
		DevEdit *e;
		QStatusBar *s;
		DevLineNumber *l;
		
};

#endif

