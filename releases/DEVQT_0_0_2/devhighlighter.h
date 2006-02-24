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

#ifndef _DEV_HIGHLIGHTER_H_
#define _DEV_HIGHLIGHTER_H_

#include "dev.h"

class BlockData;

class DevHighlighter : public QObject
{
	Q_OBJECT
	
	public:
		enum BlockState
		{
			dirty 		= -1,
			normal 		= 0,
			keyword 	= normal,
			comment, 
			preprocessor,
			quote,
			number
		};
		
		DevHighlighter(QObject *parent);
		DevHighlighter(QTextDocument *parent);
		DevHighlighter(QTextEdit *parent);
		
		virtual ~DevHighlighter();

		void setDocument(QTextDocument *doc);
		QTextDocument *document() const;
		
	protected:
		virtual void highlightBlock(QTextBlock& b);
		virtual BlockState process(const QString& text) = 0;
		
		int line(const QTextBlock& b) const;

		void setFormat(int start, int count, const QTextCharFormat &format);
		void setFormat(int start, int count, const QColor &color);
		void setFormat(int start, int count, const QFont &font);
		QTextCharFormat format(int pos) const;

		int previousBlockState() const;
			
		Q_DISABLE_COPY(DevHighlighter)
		
		QTextBlock previous;
		QPointer<QTextDocument> doc;
		QVector<QTextCharFormat> formatChanges;

	private slots:
		void reformatBlocks(int from, int charsRemoved, int charsAdded);
		void reformatBlock(QTextBlock& block);

		void applyFormatChanges(QTextBlock& block);
};

class CppHighlighter : public DevHighlighter
{
	Q_OBJECT
	
	public:
		CppHighlighter(QTextEdit *parent, bool cl = false);
		CppHighlighter(QTextDocument *parent, bool cl = false);
		
		~CppHighlighter();
		
	public slots:
		void colorLines(bool yes);
		
	protected:
		enum Options
		{
			keywords = 61,
		};
		
		enum LineState
		{
			None,
			Error,
			Current,
			BreakPoint
		};
		
		virtual void highlightBlock(QTextBlock& b);
		virtual BlockState process(const QString& text);
		
		void setupData();
		
	private:
		bool colorbgs;
    	static const char *kwds[];
    	QVector<QTextCharFormat> fmts;
		QVector<QBrush> bgds;
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
	
	inline BlockData() : s(None) {}
	
	State s;
};

#endif
