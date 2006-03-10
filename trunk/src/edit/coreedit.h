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

#ifndef _CORE_EDIT_H_
#define _CORE_EDIT_H_

#include "dev.h"

#include "editorstate.h"

class Indenter;
class ParenMatcher;

class CoreEdit : public QTextEdit
{
	Q_OBJECT
	
	friend class EditorState;
	friend class NormalState;
	friend class PersistentState;
	friend class MultilineState;
	
	public:
		CoreEdit(QWidget *p = 0, const QString& s = QString());
		
		QFont font() const;
		
		int	 line(const QTextCursor& c);
		int  column(const QTextCursor& c);
		
		int	 tabstop() const;
		bool replaceTab() const;
		
		bool autoClose() const;
		bool CtrlNavigation() const;
		
		int  margin() const;
		bool drawMargin() const;
		bool autoIndent() const;
		bool parenMatching() const;
		bool highlightCurrentLine() const;
		bool highlightCurrentBlock() const;
		
		EditorState* state() const;
		
	public slots:
		void delSelect();
		
		void setFont(const QFont& f);
		
		void setAutoClose(bool y);
		void setCtrlNavigation(bool y);
		
		void setTabstop(int n);
		void setReplaceTab(bool y);
		
		void setState(EditorState *s);
		
		void setMargin(int n);
		void setDrawMargin(bool y);
		void setAutoIndent(bool y);
		void setParenMatching(bool y);
		void setHighlightCurrentLine(bool y);
		void setHighlightCurrentBlock(bool y);
		
		void gotoLine(int row, int col);
		
	signals:
		void message(const QString& msg, int p);
		
	protected slots:
		void textCursorPos();
		void docModified(bool mod);
		
	protected:
		
		enum BlockState
		{
			dirty 		= -1,
			normal 		= 0,
			comment, 
			preprocessor,
			quote,
			number
		};
		
		void changeState(EditorState *s);
		
		virtual void paintEvent(QPaintEvent *e);
		virtual void timerEvent(QTimerEvent *e);
		
		virtual void keyPressEvent(QKeyEvent *e);
		
		virtual void dropEvent(QDropEvent *e);
		virtual void dragMoveEvent(QDragMoveEvent *e);
		virtual void dragEnterEvent(QDragEnterEvent *e);
		virtual void dragLeaveEvent(QDragLeaveEvent *e);
		
		virtual void mouseMoveEvent(QMouseEvent *e);
		virtual void mousePressEvent(QMouseEvent *e);
		virtual void mouseReleaseEvent(QMouseEvent *e);
		virtual void mouseDoubleClickEvent(QMouseEvent *e);
		
		virtual void contextMenuEvent(QContextMenuEvent *e);
		
		virtual QMimeData* createMimeDataFromSelection() const; 
		virtual void insertFromMimeData(const QMimeData * source);
		
		EditorState 	*pState;
		ParenMatcher	*pMatcher;
		Indenter		*pIndenter;
		
		QBasicTimer	blinker;
		
		QTextCursor	persistent;
		QList<QTextCursor>	multiline;
		
		QLine	curLine;
		
		int		iTab;				// 1 '\t' = iTab ' '
		int		iMargin;			// margin position (number of spaces) 
		
		bool	bCursor;			// cursor state (blinking stuff)
		
		bool	bLine;				// highlight current line?
		bool	bBlock;				// highlight current block ?
		bool	bIndent;			// auto indent ?
		bool	bMatchParen;		// parenthesis matching ?
		bool	bMargin;			// draws margin ?
		bool	bTab;				// replace tabs by spaces ?
		bool	bAutoClose;			// auto close chars ( '{' => '}' ) ?
		bool	bNavCTRL;			// Ctrl key navigation ?
		
};

#endif
