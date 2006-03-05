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

#ifndef _DEV_CONSOLE_H_
#define _DEV_CONSOLE_H_

#include "dev.h"

class DevConsole : public QFrame
{
	Q_OBJECT
	
	public:
		DevConsole(QWidget *p = 0);
		virtual ~DevConsole();
	
	public slots:
		void wait();
		void exec(const QString& cmd, const QString& dir, const QStringList& params);
		
	private slots:
		void error(QProcess::ProcessError err);
		void stateChanged(QProcess::ProcessState state);
		
		void read();
		
		void display(const QString& msg);
		void command(const QString& msg);
		
		void directory(const QString& msg);
		
	private:
		int n;
		QString dir;
		
		QProcess *proc;
		
		QTextEdit *log;
		QComboBox *cmd;
		
		QPushButton *abort;
		QLineEdit *errors, *size;
};

#endif

