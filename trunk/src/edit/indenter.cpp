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

#include "indenter.h"

#include "coreedit.h"

Indenter::Indenter(CoreEdit *e)
 : QObject(e), edit(e)
{
	;
}

Indenter::~Indenter()
{
}

QString Indenter::spaces(const QTextCursor& c, bool rep, int size)
{
	int i = -1, n;
	QString tabs = c.block().text(), suffix;
	
	if ( rep )
		suffix = QString(' ', size);
	else
		suffix = "\t";
	
	while ( ++i < tabs.size() )
	{
		if ( !tabs.at(i).isSpace() )
		{
			QString txt = tabs.mid(i);
			tabs = tabs.left(i);
			
			i = -1;
			
			while ( ++i < txt.size() )
			{
				if ( txt.at(i) == '{' )
				{
					tabs += suffix;
				} else if ( txt.at(i) == '}' ) {
					n = ( ( tabs.size() / size ) - 1 ) * size;
					
					if ( tabs.endsWith('\t') )
					{
						tabs.chop(1);
						continue;
					}
					
					while ( tabs.endsWith(' ') && --n)
						tabs.chop(1);
				}
			}
			
			return tabs;
		}
	}
	
	return tabs;
}

void Indenter::backspace(QTextCursor& c, int size, Qt::Key k)
{
	c.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor);
	
	if ( c.selectedText() == "\t" )
		c.removeSelectedText();
	else if ( c.selectedText() == " " )
	{
		int n = 0;
		while ( (c.selectedText().startsWith(" ")) && (n++ < size) )
			c.movePosition(	QTextCursor::PreviousCharacter,
							QTextCursor::KeepAnchor);
		
		if ( !c.selectedText().startsWith(" ") )
			c.movePosition(	QTextCursor::NextCharacter,
							QTextCursor::KeepAnchor);
		
		c.removeSelectedText();
	}
	
}

