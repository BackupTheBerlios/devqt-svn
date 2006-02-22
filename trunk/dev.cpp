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


#include "dev.h"

namespace DevQt
{
	QChar charFromCursor(QTextCursor cursor, QTextCursor::MoveOperation op)
	{
		cursor.clearSelection();
		if (!cursor.movePosition(op, QTextCursor::KeepAnchor))
			return QChar();
		if (!cursor.hasSelection())
			return QChar();
		return cursor.selectedText().at(0);
	}
	
	QTextCursor gotoLine(const QTextDocument *d, int n)
	{
		int i = -1;
		for (QTextBlock blk = d->begin();
			blk.isValid(); blk = blk.next(), ++n)
		{
			if (i == n)
			{
				return QTextCursor(blk);
			}
		}
		return QTextCursor();
	}

	int lines(const QTextDocument *d)
	{
		int cnt = 0;
		QTextBlock blk;
		
		for (blk = d->begin(); blk.isValid(); blk = blk.next())
			++cnt;
		
		return cnt;
	}
	
	int line(const QTextDocument *d, const QTextBlock& b)
	{
		if ( !b.isValid() )
			return -1;
			
		int n = 1;
		QTextBlock i;
		
		for(i=d->begin(); i.isValid(); i = i.next(), ++n)
			if ( i == b )
				return n;
		
		return -1;
	}
	
	bool isNumber(char c, char base)
	{
		if ( base > 10 )
		{
			return (((c>='0') && (c<='9')) ||
					((c>='a') && (c<=('a'+base-10))) ||
					((c>='A') && (c<=('A'+base-10))) );
		}
		
		return ( (c>='0') && (c<=('0'+base)) );
	}
	
};
