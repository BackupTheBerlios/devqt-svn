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

class CoreEdit;
class DevStatus;
class DevLineNumber;
class DevHighlighter;

class DevEdit : public QWidget
{
	Q_OBJECT
	
	friend class DevGUI;
	
	public:
		enum OpenType
		{
			string,
			file
		};
		
		DevEdit(QWidget *p = 0);
		DevEdit(const QString& s,
				OpenType t = string,
				QWidget *p = 0 );
		
		QString name() const;
		
		QString text() const;
		
		QTextCursor textCursor() const;
		QTextDocument* document() const;
		
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
		
		void setFont(const QFont& f);
		void setText(const QString& s, OpenType t = string);
		
		void setModified(bool b);
		
		void undo();
		void redo();
		
		void cut();
		void copy();
		void paste();
		void selectAll();
		void delSelect();
		
		void find();
		void findNext();
		void replace();
		void goTo();
		
		void print();
		
		void breakpoint();
		
		/*!
		
		Low-level slots
		
		*/
		void toggleError(int line);
		void toggleError(QTextCursor& cur);
		void toggleBreakPoint(int line);
		void toggleBreakPoint(QTextCursor& cur);
		
		void scrollTo(const QString &txt, const QString &first);
		
	signals:
		//status bar signals
		void message(const QString& msg, int p);
		
		//internal purpose signals
		void needUndo();
		void needRedo();
		
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
		
		QString n;
		
		QTextEdit *e;
		DevLineNumber *l;
		
		DevHighlighter *hl;
};

#endif

