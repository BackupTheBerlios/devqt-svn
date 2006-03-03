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

#ifndef _DEV_H_
#define _DEV_H_

#include <QtCore>
#include <QtGui>
#include <QtDebug>

namespace DevQt
{
	enum tree
	{
		files,
		classes
	};
	
	enum node
    {
		source,
		header,
		resource,
		form,
		folder,
	};
	
	enum StatusPurpose
	{
		General,
		MouseCursor,
		TextCursor,
		Modification,
		TypingMode,
		Lines,
		None
	};

	enum EditorMode
	{
		Tabbed,
		MDI
	};
	
	QChar charFromCursor(QTextCursor cursor, QTextCursor::MoveOperation op);

	QTextCursor gotoLine(const QTextDocument *d, int n);

	int lines(const QTextDocument *d);

	int line(const QTextDocument *d, const QTextBlock& b);
	
	bool isNumber(char c, char base);
	
	static const QString supportedFiles = QT_TR_NOOP("DevQt projects ( *.pro);;C++ source files( *.cpp *.cxx *.c);;C++ header files ( *.h *.hpp);;All Files (*)");

	static const QString extFiles = QT_TR_NOOP("C++ sources( *.cpp *.cxx *.c);;C++ headers ( *.h *.hpp);;All Files (*)");

	static const QString extProjects = QT_TR_NOOP("DevQt projects ( *.pro);;All Files (*)");
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

#define DevQt(argc, argv)	DevApp::Init(argc, argv)

#define DEV_APP DevApp::Instance()
#define DEV_GUI	DevGUI::Instance()
#define DEV_SCR DevSplash::Instance()
#define DEV_SETTINGS DevSettings::Instance()

#endif
