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

#ifndef _BLOCK_DATA_H_
#define _BLOCK_DATA_H_

#include "dev.h"

const int ParenthesisMatcherPropertyId = QTextFormat::UserProperty;
const int ErrorMarkerPropertyId = QTextFormat::UserProperty + 1;

struct TextLine;

struct Parenthesis
{
	enum Type
	{
		Open,
		Closed
	};
	
	inline Parenthesis() : type(Open), pos(-1) { }
	
	inline Parenthesis(Type t, QChar c, int position)
			: type(t), chr(c), pos(position) { }
	
	Type type;
	QChar chr;
	int pos;
};

struct BlockData : public QTextBlockUserData
{
	enum InternalState
	{
		None		= 0x00,
		Error 		= 0x01,
		Current 	= 0x02,
		BreakPoint 	= 0x04
	};
	
	typedef QFlags<InternalState> State;
	
	static BlockData *data(const QTextBlock& block)
	{ return static_cast<BlockData *>(block.userData()); }
	void setToBlock(QTextBlock& block)
	{ block.setUserData(this); }
	
	inline BlockData() : s(None), parenthesisMatchStart(-1),
						parenthesisMatchEnd(-1), couldCollapse(false) {}
	inline BlockData(const BlockData& b)
	{
		s 				= b.s;
		parentheses 	= b.parentheses;
		collapsedLines 	= b.collapsedLines;
		anotations		= b.anotations;
		
		errorMarkerFormat			= b.errorMarkerFormat;
		parenthesisMatchingFormat	= b.parenthesisMatchingFormat;
		
		parenthesisMatchStart	= b.parenthesisMatchStart;
		parenthesisMatchEnd		= b.parenthesisMatchEnd;
		
		couldCollapse	= b.couldCollapse;
	}
	
	State s;
	
	QList<Parenthesis>	parentheses;
	QList<TextLine>		collapsedLines;
	QStringList			anotations;
	
	QTextCharFormat    parenthesisMatchingFormat;
	QTextCharFormat    errorMarkerFormat;
	
	int                parenthesisMatchStart;
	int                parenthesisMatchEnd;
	
	bool               couldCollapse;
};

struct TextLine
{
	inline TextLine()
					: data(0) {}
	inline TextLine(const QString& l, BlockData *d)
					: txt(l), data(d) {}
	inline TextLine(const TextLine& line)
					: txt(line.txt), data(line.data) {}
	
	bool isNested() const { return !txt.canConvert(QVariant::String); }
	
	QString text() const { return text(txt); }
	
	static QString text(const QVariant& v)
	{
		if ( v.canConvert(QVariant::String) )
			return v.toString();
		else
		{
			QString s;
			
			foreach ( QVariant var, v.toList() )
			{
				s += text(var);
			}
			
			return s;
		}
	}
	
	QVariant txt; //either QString or QList<QVariant>
	BlockData *data;
};

Q_DECLARE_METATYPE(QTextCharFormat)

#endif
