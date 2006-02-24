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

#include "devdialogs.h"

DevGotoDialog::DevGotoDialog(QTextEdit *e)
 : QDialog(e), edit(e)
{
	setWindowTitle("Goto line");
	
	connect(this, SIGNAL( accepted() ),
			this, SLOT  ( process()  ) );
	
	QLabel *lbl;
	QHBoxLayout *hb = new QHBoxLayout;
	QVBoxLayout *vc = new QVBoxLayout, *vb = new QVBoxLayout;
	
	lbl = new QLabel("New line");
	vc->addWidget(lbl);
	
	line = new QSpinBox;
	line->setMinimum(1);
	vc->addWidget(line);
	
	//vc->setSizeConstraint(QLayout::SetMaximumSize);
	
	b_accept = new QPushButton("&Goto");
	connect(b_accept, SIGNAL( clicked() ),
			this	, SLOT  ( accept() ) );
	vb->addWidget(b_accept);
	
	b_cancel = new QPushButton("&Cancel");
	connect(b_cancel, SIGNAL( clicked() ),
			this	, SLOT  ( reject() ) );
	vb->addWidget(b_cancel);
	
	hb->addLayout(vc);
	hb->addLayout(vb);
	
	setLayout(hb);
}

DevGotoDialog::~DevGotoDialog()
{
}

void DevGotoDialog::execute()
{
	line->setMaximum( DevQt::lines(edit->document()) );
	QRect r = edit->geometry();
	
	int x = (r.width()-width())>>1;
	int y = (r.height()-height())>>1;
	
	move(x, y);		//dialog is centered inside text edit
	
	exec();
}

void DevGotoDialog::process()
{
	QTextBlock blk;
	int i=1, n = line->value();
	
	for (blk = edit->document()->begin(); blk.isValid(); blk = blk.next())
	{
		if (i++ == n)
		{
			QTextCursor c(blk);
			edit->setTextCursor(c);
			color(c);
			return;
		}
	}
}

DevFindDialog::DevFindDialog(QTextEdit *e)
 : QDialog(e), edit(e)
{
	setWindowTitle("Find text");
	
	connect(this, SIGNAL( accepted() ),
			this, SLOT  ( setVars() ) );
	
	QLabel *lbl;
	QGroupBox *g;
	QVBoxLayout *b;
	QGridLayout *l = new QGridLayout;
	
	text = new QLineEdit;
	lbl = new QLabel("Text to Find");
	l->addWidget(lbl, 0, 0, 1, 1);
	l->addWidget(text, 0, 1, 1, 3);
	
	g = new QGroupBox("Options");
	b = new QVBoxLayout;
	
	sensitive = new QCheckBox("Case &sensitive");
	b->addWidget(sensitive);
	
	bound = new QCheckBox("&Whole words only");
	b->addWidget(bound);
	
	g->setLayout(b);
	l->addWidget(g, 1, 0, 3, 2);
	
	g = new QGroupBox("Direction");
	b = new QVBoxLayout;
	
	forward = new QRadioButton("Forward");
	forward->toggle();
	b->addWidget(forward);
	
	backward = new QRadioButton("Backward");
	b->addWidget(backward);
	
	g->setLayout(b);
	l->addWidget(g, 1, 2, 3, 2);
	
	g = new QGroupBox("Scope");
	b = new QVBoxLayout;
	
	global = new QRadioButton("Global");
	global->toggle();
	b->addWidget(global);
	
	selection = new QRadioButton("Selected only");
	b->addWidget(selection);
	
	g->setLayout(b);
	l->addWidget(g, 4, 0, 3, 2);
	
	g = new QGroupBox("Origin");
	b = new QVBoxLayout;
	
	cursor = new QRadioButton("Cursor");
	cursor->toggle();
	b->addWidget(cursor);
	
	whole = new QRadioButton("Entire scope");
	b->addWidget(whole);
	
	g->setLayout(b);
	l->addWidget(g, 4, 2, 3, 2);
	
	b_accept = new QPushButton("&Find");
	connect(b_accept, SIGNAL( clicked() ),
			this	, SLOT  ( accept() ) );
	l->addWidget(b_accept, 7, 0, 1, 1);
	
	b_cancel = new QPushButton("&Cancel");
	connect(b_cancel, SIGNAL( clicked() ),
			this	, SLOT  ( reject() ) );
	l->addWidget(b_cancel, 7, 3, 1, 1);
	
	setLayout(l);
}

DevFindDialog::~DevFindDialog()
{
	s.clear();
	f.clear();
}

void DevFindDialog::execute()
{
	QTextCursor cur = edit->textCursor();
	text->setText( cur.selectedText() );
	
	QRect r = edit->geometry();
	
	int x = (r.width()-width())>>1;
	int y = (r.height()-height())>>1;
	
	move(x, y);		//dialog is centered inside text edit
	
	exec();
}

void DevFindDialog::setVars()
{
	/*
	setup internal vars used for find and find next
	*/
	
	f = text->text();
	s = edit->document()->toPlainText();
	QTextCursor cur(edit->textCursor());
	bw = backward->isChecked();
	ww = bound->isChecked();
	cs = (sensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive );
	pos = absPos = 0;
	
	if ( cursor->isChecked() )
		absPos = cur.position();
	
	if ( selection->isChecked() )
	{
		s = cur.selectedText();
		if( bw )
			absPos = cur.selectionEnd();
		else
			absPos = cur.selectionStart();
	}
	else
	{
		if ( cursor->isChecked() )
			absPos = cur.position();
		else if ( bw )
			absPos = s.count();
		
		if ( bw )
			s = s.mid(0, absPos);
		else
			s = s.mid(absPos);
	}
	
	if ( bw )		//start at string's end
		pos = -1;
	
	process(); //find once
}

void DevFindDialog::process()
{
	int i;
	QTextCursor cur(edit->document());
	
	if ( ww )
	{
		QRegExp xp(QString("\\b")+f+"\\b");
		xp.setCaseSensitivity(cs);
		
		if ( bw )
			i = s.lastIndexOf(xp, pos);
		else
			i = s.indexOf(xp, pos);
	}
	else
	{
		if ( bw )
			i = s.lastIndexOf(f, pos, cs);
		else
			i = s.indexOf(f, pos, cs);
	}
	
	if ( i==-1 )
		return (void)QMessageBox::warning(	this,
											"Search results",
											"Unable to find : " + f );
	
	//now jump to start of word and select it
	if ( bw )
		cur.setPosition(absPos-s.count()+i, QTextCursor::MoveAnchor);
	else
		cur.setPosition(absPos+i, QTextCursor::MoveAnchor);
	
	cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, f.count());
	
	edit->setTextCursor(cur);		//update document cursor
	color(cur);						//update current line highlighting
	
	if ( bw )
		pos = i - s.count() - f.count();
	else
		pos	= i + f.count();
}

DevReplaceDialog::DevReplaceDialog(QTextEdit *e)
 : QDialog(e), edit(e)
{
	setWindowTitle("Replace text");
	
	connect(this, SIGNAL( accepted() ),
			this, SLOT  ( setVars() ) );
	
	QLabel *lbl;
	QGroupBox *g;
	QVBoxLayout *b;
	QGridLayout *l = new QGridLayout;
	
	find = new QLineEdit;
	lbl = new QLabel("Text to Find");
	l->addWidget(lbl, 0, 0, 1, 1);
	l->addWidget(find, 0, 1, 1, 3);
	
	replace = new QLineEdit;
	lbl = new QLabel("Replace with");
	l->addWidget(lbl, 1, 0, 1, 1);
	l->addWidget(replace, 1, 1, 1, 3);
	
	g = new QGroupBox("Options");
	b = new QVBoxLayout;
	
	sensitive = new QCheckBox("Case &sensitive");
	b->addWidget(sensitive);
	
	bound = new QCheckBox("&Whole words only");
	b->addWidget(bound);
	
	prompt = new QCheckBox("&Prompt on replace");
	b->addWidget(prompt);
	
	g->setLayout(b);
	l->addWidget(g, 2, 0, 4, 2);
	
	g = new QGroupBox("Direction");
	b = new QVBoxLayout;
	
	forward = new QRadioButton("Forward");
	forward->toggle();
	b->addWidget(forward);
	
	backward = new QRadioButton("Backward");
	b->addWidget(backward);
	
	g->setLayout(b);
	l->addWidget(g, 2, 2, 4, 2);
	
	g = new QGroupBox("Scope");
	b = new QVBoxLayout;
	
	global = new QRadioButton("Global");
	global->toggle();
	b->addWidget(global);
	
	selection = new QRadioButton("Selected only");
	b->addWidget(selection);
	
	g->setLayout(b);
	l->addWidget(g, 6, 0, 3, 2);
	
	g = new QGroupBox("Origin");
	b = new QVBoxLayout;
	
	cursor = new QRadioButton("Cursor");
	cursor->toggle();
	b->addWidget(cursor);
	
	whole = new QRadioButton("Entire scope");
	b->addWidget(whole);
	
	g->setLayout(b);
	l->addWidget(g, 6, 2, 3, 2);
	
	b_accept = new QPushButton("&Ok");
	connect(b_accept, SIGNAL( clicked() ),
			this	, SLOT  ( accept() ) );
	connect(b_accept, SIGNAL( clicked() ),
			this	, SLOT  ( process() ) );
	l->addWidget(b_accept, 9, 0, 1, 1);
	
	b_all = new QPushButton("&Replace all");
	connect(b_all	, SIGNAL( clicked() ),
			this	, SLOT  ( accept() ) );
	connect(b_all	, SIGNAL( clicked() ),
			this	, SLOT  ( all() ) );
	l->addWidget(b_all, 9, 1, 1, 1);
	
	b_cancel = new QPushButton("&Cancel");
	connect(b_cancel, SIGNAL( clicked() ),
			this	, SLOT  ( reject() ) );
	l->addWidget(b_cancel, 9, 3, 1, 1);
	
	setLayout(l);
}

DevReplaceDialog::~DevReplaceDialog()
{
	s.clear();
	f.clear();
	r.clear();
}

void DevReplaceDialog::execute()
{
	QTextCursor cur = edit->textCursor();
	find->setText( cur.selectedText() );
	
	QRect r = edit->geometry();
	
	int x = (r.width()-width())>>1;
	int y = (r.height()-height())>>1;
	
	move(x, y);		//dialog is centered inside text edit
	
	exec();
}

void DevReplaceDialog::setVars()
{
	/*
	setup internal vars used for find and find next
	*/
	
	count = 0;
	abort = false;
	f = find->text();
	r = replace->text();
	ww = bound->isChecked();
	pr = prompt->isChecked();
	bw = backward->isChecked();
	displace = f.count()-r.count();
	s = edit->document()->toPlainText();
	QTextCursor cur(edit->textCursor());
	cs = (sensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive );
	pos = absPos = 0;
	
	if ( selection->isChecked() )
	{
		s = cur.selectedText();
		if( bw )
			absPos = cur.selectionEnd();
		else
			absPos = cur.selectionStart();
	}
	else
	{
		if ( cursor->isChecked() )
			absPos = cur.position();
		else if ( bw )
			absPos = s.count();
		
		if ( bw )
			s = s.mid(0, absPos);
		else
			s = s.mid(absPos);
	}
	
	if ( bw )		//start at string's end
		pos = -1;
}

void DevReplaceDialog::process()
{
	int i;
	QTextCursor cur(edit->document());
	
	if ( ww )
	{
		QRegExp xp(QString("\\b")+f+"\\b");
		xp.setCaseSensitivity(cs);
		
		if ( bw )
			i = s.lastIndexOf(xp, pos);
		else
			i = s.indexOf(xp, pos);
	}
	else
	{
		if ( bw )
			i = s.lastIndexOf(f, pos, cs);
		else
			i = s.indexOf(f, pos, cs);
	}
	
	if ( i==-1 )
	{
		abort = true;
		return (void)QMessageBox::warning(	this,
											"Search results",
											"Unable to find : " + f );
	}
	
	int delta = count*displace;
	//now jump to start of word and select it
	if ( bw )
		cur.setPosition(absPos-s.count()+i-delta, QTextCursor::MoveAnchor);
	else
		cur.setPosition(absPos+i, QTextCursor::MoveAnchor);
	
	cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, f.count());
	
	edit->setTextCursor(cur);		//update document cursor
	color(cur);						//update current line highlighting
	
	if ( pr )
	{
		int s = QMessageBox::warning(this, "Replacing...",
							"Are you sure that you want to replace that?",
							QMessageBox::Yes,
							QMessageBox::YesAll,
							QMessageBox::Cancel );
		switch (s)
		{
			case QMessageBox::YesAll:
				pr = false;
				break;
				
			case QMessageBox::Cancel:
				abort = true;
				return;
		}
	}
	
	cur.insertText(r);
	s.replace(i, f.count(), r);
	
	if ( bw )
		pos = i - s.count() - r.count();
	else
		pos	= i + r.count();
	count++;
}

void DevReplaceDialog::all()
{
	while ( !abort )
		process();
}
