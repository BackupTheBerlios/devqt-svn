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

#ifndef _MATCHER_H_
#define _MATCHER_H_

#include "dev.h"

class CoreEdit;

class ParenMatcher : public QObject
{
	Q_OBJECT
	
	public:
		enum MatchType
		{
			NoMatch,
			Match,
			Mismatch
		};


		ParenMatcher(CoreEdit *parent = 0);
		
		QColor matchColor();
		QRect currentBlock();
		QStringList anotations(QTextBlock block);
		
		bool  collapse(QTextBlock block);
		bool  expand(QTextBlock block);
		bool  isCollapsed(QTextBlock block);
		bool  couldCollapse(QTextBlock block);
		void  matchCharacter(QLine *lineToPaint = 0);
		
		static MatchType match(QTextCursor *cursor);
		
		inline static QChar charFromCursor(	QTextCursor cursor,
											QTextCursor::MoveOperation op)
		{
			cursor.clearSelection();
			
			if (!cursor.movePosition(op, QTextCursor::KeepAnchor))
				return QChar();
				
			if (!cursor.hasSelection())
				return QChar();
				
			return cursor.selectedText().at(0);
		}

	private:
		QPointer<CoreEdit> edit;
		
		QTextCharFormat  matchFormat;
		QTextCharFormat  mismatchFormat;
		
		QTextCursor currentMatch;
		
		void fixBlockData(QTextCursor cursor, MatchType matchType, bool open);
		void clearMarkerFormat(const QTextBlock &block, int markerId);
};

#endif
