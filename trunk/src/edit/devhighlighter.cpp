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

#include "devhighlighter.h"

#include "blockdata.h"

/*

DevHighlighter : a modified version of QSyntaxHiglighter

*/

DevHighlighter::DevHighlighter(QObject *parent)
 : QObject(parent)
{
}

DevHighlighter::DevHighlighter(QTextDocument *parent)
 : QObject(parent)
{
	setDocument(parent);
}

DevHighlighter::DevHighlighter(QTextEdit *parent)
 : QObject(parent)
{
	setDocument(parent->document());
}

DevHighlighter::~DevHighlighter()
{
	setDocument(0);
}

int DevHighlighter::line(const QTextBlock& b) const 
{
	if ( !b.isValid() )
		return -1;
		
	int n = 1;
	QTextBlock i;
	
	for(i=doc->begin(); i.isValid(); i = i.next(), ++n)
		if ( i == b )
			return n;
	return -1;
}

void DevHighlighter::setDocument(QTextDocument *d)
{
	if ( doc )
	{
		disconnect(	doc	, SIGNAL(contentsChange(int, int, int)),
					this, SLOT(reformatBlocks(int, int, int)));

		QTextCursor cursor(doc);
		cursor.beginEditBlock();
		
		for (QTextBlock blk = doc->begin(); blk.isValid(); blk = blk.next())
            blk.layout()->clearAdditionalFormats();
        
		cursor.endEditBlock();
	}
	doc = d;
	if ( doc )
	{
		connect(doc	, SIGNAL(contentsChange(int, int, int)),
				this, SLOT(reformatBlocks(int, int, int)));
	}
}

QTextDocument *DevHighlighter::document() const
{
	return doc;
}

void DevHighlighter::applyFormatChanges(QTextBlock& block)
{
	QList<QTextLayout::FormatRange> ranges;

	QTextCharFormat emptyFormat;

	QTextLayout::FormatRange r;
	r.start = r.length = -1;

	int i = 0;
	while (i < formatChanges.count())
	{
		while (i < formatChanges.count() && formatChanges.at(i) == emptyFormat)
			++i;

		if (i >= formatChanges.count())
			break;
		
		r.start = i;
		r.format = formatChanges.at(i);

		while (i < formatChanges.count() && formatChanges.at(i) == r.format)
			++i;

		if (i >= formatChanges.count())
			break;

		r.length = i - r.start;
		ranges << r;
		r.start = r.length = -1;
	}

	if (r.start != -1)
	{
		r.length = formatChanges.count() - r.start;
		ranges << r;
	}
	
	block.layout()->setAdditionalFormats(ranges);
}

void DevHighlighter::reformatBlocks(int from, int charsRemoved, int charsAdded)
{
	QTextBlock block = doc->findBlock(from);
	if (!block.isValid())
		return;

	QTextBlock endBlock;
	if (charsAdded > charsRemoved || charsAdded == charsRemoved)
		endBlock = doc->findBlock(from + charsAdded);
	else
		endBlock = block;

	bool forceHighlightOfNextBlock = false;

	while (block.isValid() && (!(endBlock < block) || forceHighlightOfNextBlock) )
	{
		const int stateBeforeHighlight = block.userState();

		reformatBlock(block);

		forceHighlightOfNextBlock = (block.userState() != stateBeforeHighlight);

		block = block.next();
	}

	formatChanges.clear();
}

void DevHighlighter::reformatBlock(QTextBlock& block)
{
	previous = block.previous();

	formatChanges.fill(QTextCharFormat(), block.length() - 1);
	highlightBlock(block);
	applyFormatChanges(block);
	
	previous = QTextBlock();
	
	doc->markContentsDirty(block.position(), block.length());
}

void DevHighlighter::setFormat(int start, int count, const QTextCharFormat &format)
{
	if (start < 0 || start >= formatChanges.count())
		return;

	const int end = qMin(start + count, formatChanges.count());
	for (int i = start; i < end; ++i)
		formatChanges[i] = format;
}

void DevHighlighter::setFormat(int start, int count, const QColor &color)
{
	QTextCharFormat format;
	format.setForeground(color);
	setFormat(start, count, format);
}

void DevHighlighter::setFormat(int start, int count, const QFont &font)
{
	QTextCharFormat format;
	format.setFont(font);
	setFormat(start, count, format);
}

QTextCharFormat DevHighlighter::format(int pos) const
{
	if (pos < 0 || pos >= formatChanges.count())
		return QTextCharFormat();
	return formatChanges.at(pos);
}

int DevHighlighter::previousBlockState() const
{
	if (!previous.isValid())
		return -1;

	return previous.userState();
}

void DevHighlighter::highlightBlock(QTextBlock& b)
{
	b.setUserState( process(b.text()) );
}

/*

CppHighlighter : example of highlighter. handles c++ syntax.

*/

const char* CppHighlighter::kwds[CppHighlighter::keywords] = 
{	"asm", "auto", "break", "bool", "case", "catch", "char",
	"class", "const", "continue", "default", "delete", "do",
	"double", "else", "enum", "extern", "explicit", "false",
	"friend", "float", "for", "goto", "if", "inline", "int",
	"long", "mutable", "new", "namespace", "operator", "private",
	"public", "protected", "register", "return", "short", "signed",
	"sizeof", "static", "struct", "switch", "this", "template",
	"throw", "true", "try", "typedef", "typename", "union", "using",
	"unsigned", "virtual", "volatile", "void", "wchar_t", "while",
	"dynamic_cast", "static_cast", "const_cast", "reinterpret_cast"
};

CppHighlighter::CppHighlighter(QTextEdit *parent)
 : DevHighlighter(parent)
{
	setupData();
}

CppHighlighter::CppHighlighter(QTextDocument *parent)
 : DevHighlighter(parent)
{
	setupData();
}

CppHighlighter::~CppHighlighter()
{
	fmts.clear();		//clear words' formats informations
}

void CppHighlighter::setupData()
{
	fmts.resize(5);
	QTextCharFormat f;
	
	f.setForeground(Qt::darkMagenta);
	fmts[number] = f;
	
	f.setForeground(Qt::red);
	fmts[quote] = f;
	
	f.setForeground(Qt::darkGreen);
	fmts[preprocessor] = f;
	
	f.setForeground(Qt::black);
	f.setFontWeight(QFont::Bold);
	fmts[keyword] = f;
	
	f.setForeground(Qt::darkBlue);
	f.setFontWeight(QFont::Normal);
	f.setFontItalic(true);
	fmts[comment] = f;
	
}

void CppHighlighter::highlightBlock(QTextBlock& b)
{
	b.setUserState( process(b.text()) );
	
	BlockData *blockData = BlockData::data(b);
	
	if ( !blockData )
	{
		blockData = new BlockData;
		blockData->setToBlock(b);
	}
	
	if (blockData->parenthesisMatchStart != -1)
	{
		QTextCharFormat fmt = format(blockData->parenthesisMatchStart);
        blockData->parenthesisMatchingFormat.setProperty(
        										ParenthesisMatcherPropertyId,
        										qVariantFromValue(fmt) );
        fmt.merge(blockData->parenthesisMatchingFormat);
        setFormat(blockData->parenthesisMatchStart, 1, fmt);
    }

    if (blockData->parenthesisMatchEnd != -1)
    {
        QTextCharFormat fmt = format(blockData->parenthesisMatchEnd);
        blockData->parenthesisMatchingFormat.setProperty(
        										ParenthesisMatcherPropertyId,
        										qVariantFromValue(fmt) );
        fmt.merge(blockData->parenthesisMatchingFormat);
        setFormat(blockData->parenthesisMatchEnd, 1, fmt);
    }
	
}

/*

New highlighting core : no QRegExps enymore!!! much faster, some fixes had been
done (multi-lines string if '\' at EOL; comment after preprocessor...)

*/

DevHighlighter::BlockState CppHighlighter::process(const QString& text)
{
	QChar c;
	QString s;
	const QChar *str = text.data();
	int i, n = i = 0, len = text.length();
	
	switch ( previousBlockState() )
	{
		case comment :
			i = text.indexOf("*/");
			
			if ( i == -1 )
			{
				setFormat(0, len, fmts[comment]);
				
				return comment;
			}
			
			setFormat(0, i+2, fmts[comment]);
			i--;
			break;
			
		case quote :
			do
			{
				i = text.indexOf("\"", i);
					
				if ( i == -1 )
				{
					setFormat(n, len-n, fmts[quote]);
					
					if ( text.endsWith('\\') )
						return quote;
					
					return normal;
				}
			} while ( text.at(i-1)=='\\' );
			
			setFormat(0, i+1, fmts[quote]);
			//i--;
			break;
		
		case preprocessor :
			
			setFormat(0, len, fmts[preprocessor]);
			
			while ( i<len )
			{
				if ( (i = text.indexOf('/', i)) == -1 )
					break;
				n = i;
				switch ( text.at(++i).toLatin1() )
				{
					case '/':
						setFormat(n, len-n, fmts[comment]);
						
						return normal;
						
					case '*':
						i = text.indexOf("*/", ++i);
						
						if ( i == -1 )
						{
							setFormat(n, len-n, fmts[comment]);
							
							return comment;
						}
						
						setFormat(n, i-n+2, fmts[comment]);
						break;
				}
			}
			
			if ( str[len-1] == '\\' )
				return preprocessor;
			
			return normal;
			
		default:
			i = -1;
			break;
	}
	
	while ( i < len )
	{
		do
		{
			c = str[++i];
		} while ( c.isSpace() );
		
		n = i;
		
		if ( c == '/' )
		{
			switch ( str[++i].toLatin1() )
			{
				case '/':
					setFormat(n, len-n, fmts[comment]);
					
					return normal;
					
				case '*':
					i = text.indexOf("*/", n);
					
					if ( i == -1 )
					{
						setFormat(n, len-n, fmts[comment]);
						
						return comment;
					}
					
					setFormat(n, i-n+2, fmts[comment]);
					break;
			}
		}
		else if ( c=='#' )
		{
			setFormat(n, len-n, fmts[preprocessor]);
			
			while ( i<len )
			{
				if ( (i = text.indexOf('/', i)) == -1 )
					break;
				n = i;
				switch ( text.at(++i).toLatin1() )
				{
					case '/':
						setFormat(n, len-n, fmts[comment]);
						
						return normal;
						
					case '*':
						i = text.indexOf("*/", ++i);
						
						if ( i == -1 )
						{
							setFormat(n, len-n, fmts[comment]);
							
							return comment;
						}
						
						setFormat(n, i-n+2, fmts[comment]);
						break;
				}
			} 
			
			if ( str[len-1] == '\\' )
				return preprocessor;
			else
				return normal;
		}
		else if ( c=='\'' )
		{
			do
			{
				i = text.indexOf("\'", ++i);
					
				if ( i == -1 )
				{
					setFormat(n, len-n, fmts[quote]);
					
					return normal;
				}
			} while ( text.at(i-1)=='\\' );
			
			setFormat(n, i-n+1, fmts[quote]);
		}
		else if ( c=='\"' )
		{
			do
			{
				i = text.indexOf("\"", ++i);
					
				if ( i == -1 )
				{
					setFormat(n, len-n, fmts[quote]);
					
					if ( str[len-1]=='\\' )
						return quote;
					else
						return normal;
				}
			} while ( text.at(i-1)=='\\' );
			
			setFormat(n, i-n+1, fmts[quote]);
		}
		else if ( c.isLetter() || (c == '_') )
		{
			do
			{
				c = str[++i];
			} while ( c.isLetterOrNumber() || (c == '_') );
			
			s = QString(str+n, i-n);
			
			for (int j=0; j<keywords; j++)
			{
				if ( s != kwds[j] )
					continue;
				
				setFormat(n, s.count(), fmts[keyword]);
				break;
			}
			i--;
		}
		else if ( c.isNumber() )
		{
			char base = 10;
			
			if ( str[i] == '0' )
			{
				base = 8;
				if ( str[i+1] == 'x' )
				{
					base = 16;
					i++;
				}
			}
			
			do
			{
				c = str[++i];
			} while ( 	DevQt::isNumber(c.toLatin1(), base) || ( (base==10) && 
					((c=='.') || (c=='e') || (c=='E') || (c=='f') || (c=='F')) )
					);
			
			s = QString(str+n, i-n);
			i--;
			
			if (base == 10 && (
				(s.count('e', Qt::CaseInsensitive)>1) || 
				(s.count('f', Qt::CaseInsensitive)>1)  ) )
				continue;
			
			setFormat(n, s.count(), fmts[number]);
		}
	}
	
	return normal;
}
