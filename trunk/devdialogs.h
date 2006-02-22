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
		DevGotoDialog(QTextEdit *e);
		virtual ~DevGotoDialog();
		
	public slots:
		void execute();
		
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
		DevFindDialog(QTextEdit *e);
		virtual ~DevFindDialog();
		
	public slots:
		void execute();
		void process();
		
	signals:
		void color(QTextCursor& c);
		
	private slots:
		void setVars();
		
	private:
		bool bw, ww;
		QString s, f;
		int absPos, pos;
		Qt::CaseSensitivity cs;
		
		QPointer<QTextEdit> edit;
		
		QLineEdit *text;
		QCheckBox *sensitive, *bound;
		QPushButton *b_accept, *b_cancel;
		QRadioButton *global, *selection, *forward, *backward, *cursor, *whole;
};

class DevReplaceDialog : public QDialog
{
	Q_OBJECT
	
	public:
		DevReplaceDialog(QTextEdit *e);
		virtual ~DevReplaceDialog();
		
	public slots:
		void execute();
		
	protected slots:
		void process();
		void all();
		
	signals:
		void color(QTextCursor& c);
		
	private slots:
		void setVars();
		
	private:
		QString s, f, r;
		Qt::CaseSensitivity cs;
		bool bw, ww, pr, abort;
		int absPos, pos, displace, count;
		
		QPointer<QTextEdit> edit;
		
		QLineEdit *find, *replace;
		QPushButton *b_accept, *b_all, *b_cancel;
		QCheckBox *sensitive, *bound, *prompt;
		QRadioButton *global, *selection, *forward, *backward, *cursor, *whole;
};

#ifdef _DEV_DEBUG_
class DevDebug : public QDialog
{
	Q_OBJECT
	
	public:
		DevDebug(const QString& s1, const QString& s2)
		{
			QHBoxLayout *h = new QHBoxLayout;
			QVBoxLayout *v = new QVBoxLayout;
			
			lbl1 = new QLabel("first text log");
			log1 = new QTextEdit;
			log1->setPlainText(s1);
			v->addWidget(lbl1);
			v->addWidget(log1);
			
			lbl2 = new QLabel("second text log");
			log2 = new QTextEdit;
			log2->setPlainText(s2);
			v->addWidget(lbl2);
			v->addWidget(log2);
			
			accept = new QPushButton("&Ok");
			connect(accept	, SIGNAL( clicked() ),
					this	, SLOT  ( accept()  ) );
			cancel = new QPushButton("&Cancel");
			connect(cancel	, SIGNAL( clicked() ),
					this	, SLOT  ( reject()  ) );
			h->addWidget(accept);
			h->addWidget(cancel);
			v->addLayout(h);
			
			setLayout(v);
		}
		
	protected:
		;
		
	private:
		QLabel *lbl1, *lbl2;
		QTextEdit *log1, *log2;
		QPushButton *accept, *cancel;
};
#endif

#endif
