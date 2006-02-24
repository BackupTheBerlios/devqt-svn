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

#ifndef _DEV_DIALOGS_H_
#define _DEV_DIALOGS_H_

#include "dev.h"

class DevGotoDialog : public QDialog
{
	Q_OBJECT
	
	public:
		DevGotoDialog(QWidget *p);
		virtual ~DevGotoDialog();
		
	public slots:
		void execute(QTextEdit *e);
		
	protected slots:
		void process();
		
	signals:
		void color(QTextCursor& c);
		
	private:
		QPointer<QTextEdit> edit;
		
		QSpinBox *line;
		QPushButton *b_accept, *b_cancel;
};

class DevFindDialog : public QDialog
{
	Q_OBJECT
	
	public:
		DevFindDialog(QWidget *p = 0);
		virtual ~DevFindDialog();
		
	public slots:
		void execute(QTextEdit *e);
		void process();
		
	signals:
		void color(QTextCursor& c);
		
	private slots:
		void setVars();
		
	private:
		QPointer<QTextEdit> edit;
		
		bool bw, ww;
		QString s, f;
		int absPos, pos;
		Qt::CaseSensitivity cs;
		
		QLineEdit *text;
		QCheckBox *sensitive, *bound;
		QPushButton *b_accept, *b_cancel;
		QRadioButton *global, *selection, *forward, *backward, *cursor, *whole;
};

class DevReplaceDialog : public QDialog
{
	Q_OBJECT
	
	public:
		DevReplaceDialog(QWidget *p = 0);
		virtual ~DevReplaceDialog();
		
	public slots:
		void execute(QTextEdit *e);
		
	protected slots:
		void process();
		void all();
		
	signals:
		void color(QTextCursor& c);
		
	private slots:
		void setVars();
		
	private:
		QPointer<QTextEdit> edit;
		
		QString s, f, r;
		Qt::CaseSensitivity cs;
		bool bw, ww, pr, abort;
		int absPos, pos, displace, count;
		
		QLineEdit *find, *replace;
		QPushButton *b_accept, *b_all, *b_cancel;
		QCheckBox *sensitive, *bound, *prompt;
		QRadioButton *global, *selection, *forward, *backward, *cursor, *whole;
};

class DevPropertiesDialog : public QDialog
{
	Q_OBJECT
	
	public:
		DevPropertiesDialog(QWidget *p = 0);
		virtual ~DevPropertiesDialog();
		
	public slots:
		void execute(const QString& n);
		
	private:
		QString name;
		
};


#endif
