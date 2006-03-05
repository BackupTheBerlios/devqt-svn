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

#include "devgui.h"

QString DevDialogs::newFile(QString& n)
{
	QDialog *dlg = new QDialog(DEV_GUI);
	
	dlg->setWindowTitle(tr("New file :"));
	
	QFrame *hl;
	QLabel *lbl;
	QRadioButton *rad_s, *rad_h, *rad_r, *rad_t, *rad_f;
	QPushButton *b_accept, *b_cancel;
	QVBoxLayout *vb = new QVBoxLayout;
	QHBoxLayout *hc = new QHBoxLayout, *hb = new QHBoxLayout;
	
	lbl = new QLabel(tr("Name : "));
	hc->addWidget(lbl);
	
	QLineEdit *line = new QLineEdit;
	hc->addWidget(line);
	
	QGridLayout *gr = new QGridLayout;
	
	lbl = new QLabel(tr("Type : "));
	gr->addWidget(lbl, 0, 0);
	
	rad_s = new QRadioButton(tr("Source"));
	rad_s->setChecked(true);
	gr->addWidget(rad_s, 0, 1);
	
	rad_h = new QRadioButton(tr("Header"));
	gr->addWidget(rad_h, 0, 2);
	
	rad_f = new QRadioButton(tr("Form"));
	gr->addWidget(rad_f, 1, 0);
	
	rad_r = new QRadioButton(tr("Resource"));
	gr->addWidget(rad_r, 1, 1);
	
	rad_t = new QRadioButton(tr("Translation"));
	gr->addWidget(rad_t, 1, 2);
	
	b_accept = new QPushButton(QIcon(":/ok.png"), "&Ok");
	connect(b_accept, SIGNAL( clicked() ),
			dlg	, SLOT  ( accept() ) );
	hb->addWidget(b_accept);
	
	b_cancel = new QPushButton(QIcon(":/cancel.png"), "&Cancel");
	connect(b_cancel, SIGNAL( clicked() ),
			dlg	, SLOT  ( reject() ) );
	hb->addWidget(b_cancel);
	
	vb->addLayout(hc);
	hl = new QFrame;
	hl->setFrameShape(QFrame::HLine);
	vb->addWidget(hl);
	vb->addLayout(gr);
	hl = new QFrame;
	hl->setFrameShape(QFrame::HLine);
	vb->addWidget(hl);
	vb->addLayout(hb);
	
	dlg->setLayout(vb);
	
	int code = dlg->exec();
	QString name = QString::null;
	
	if ( code == QDialog::Accepted )
	{
		name = line->text();
		
		if ( rad_s->isChecked() )
			n = "SOURCES";
		else if ( rad_h->isChecked() )
			n = "HEADERS";
		else if ( rad_f->isChecked() )
			n = "FORMS";
		else if ( rad_r->isChecked() )
			n = "RESOURCES";
		else if ( rad_t->isChecked() )
			n = "TRANSLATIONS";
	} else {
		n = QString::null;
		return QString::null;
	}
	
	delete dlg;
	return name;
}

QString DevDialogs::newFold(DevQt::node& n)
{
	QDialog *dlg = new QDialog(DEV_GUI);
	
	dlg->setWindowTitle(tr("New folder :"));
	
	QFrame *hl;
	QLabel *lbl;
	QRadioButton *rad_f, *rad_s;
	QPushButton *b_accept, *b_cancel;
	QVBoxLayout *vb = new QVBoxLayout;
	QHBoxLayout *hc = new QHBoxLayout, *hb = new QHBoxLayout;
	
	lbl = new QLabel(tr("Name : "));
	hc->addWidget(lbl);
	
	QLineEdit *line = new QLineEdit;
	hc->addWidget(line);
	
	QGridLayout *gr = new QGridLayout;
	
	lbl = new QLabel(tr("Type : "));
	gr->addWidget(lbl, 0, 0);
	
	rad_f = new QRadioButton(tr("Folder"));
	rad_f->setChecked(true);
	gr->addWidget(rad_f, 0, 1);
	
	rad_s = new QRadioButton(tr("Target"));
	gr->addWidget(rad_s, 0, 2);
	
	b_accept = new QPushButton(QIcon(":/ok.png"), "&Ok");
	connect(b_accept, SIGNAL( clicked() ),
			dlg	, SLOT  ( accept() ) );
	hb->addWidget(b_accept);
	
	b_cancel = new QPushButton(QIcon(":/cancel.png"), "&Cancel");
	connect(b_cancel, SIGNAL( clicked() ),
			dlg	, SLOT  ( reject() ) );
	hb->addWidget(b_cancel);
	
	vb->addLayout(hc);
	hl = new QFrame;
	hl->setFrameShape(QFrame::HLine);
	vb->addWidget(hl);
	vb->addLayout(gr);
	hl = new QFrame;
	hl->setFrameShape(QFrame::HLine);
	vb->addWidget(hl);
	vb->addLayout(hb);
	
	dlg->setLayout(vb);
	
	
	int code = dlg->exec();
	QString name = QString::null;
	
	if ( code == QDialog::Accepted )
	{
		name = line->text();
		
		if ( rad_f->isChecked() )
			n = DevQt::folder;
		else if ( rad_s->isChecked() )
			n = DevQt::scope;
	} else {
		n = DevQt::none;
		return QString::null;
	}
	
	delete dlg;
	return name;
}

DevGotoDialog::DevGotoDialog(QWidget *p)
 : QDialog(p)
{
	setupUi(this);
}

DevGotoDialog::~DevGotoDialog()
{
}

void DevGotoDialog::execute(QTextEdit *e)
{
	if ( !e )
		return;
	edit = e;
	
	line->setMaximum( DevQt::lines(edit->document()) );
	
	exec();
}

void DevGotoDialog::process()
{
	if ( !edit )
		return;
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

DevFindDialog::DevFindDialog(QWidget *p)
 : QDialog(p, Qt::Dialog | Qt::WindowStaysOnTopHint), status(0)
{
	setupUi(this);

	QGridLayout *l = qobject_cast<QGridLayout*>(layout());
	if ( l != 0 ) {
		status = new QStatusBar(this);
		status->setSizeGripEnabled(true);
		l->addWidget(status, l->rowCount(), 0, 1, l->columnCount());
	}

	connect(b_accept, SIGNAL( clicked() ),
			this	, SLOT  ( process() ) );
}

DevFindDialog::~DevFindDialog()
{
	s.clear();
	f.clear();
}

void DevFindDialog::execute(QTextEdit *e)
{
	if ( !e )
		return;

	edit = e;
	
	show();
	raise();
	activateWindow();
	text->setFocus();

	QTextCursor cur = e->textCursor();
	if ( cur.hasSelection() )
		text->setText( cur.selectedText() );

	text->selectAll();
	if (status)
		status->clearMessage();
}

void DevFindDialog::showMessage(const QString &message, int timeout)
{
	if ( !status )
		return;
	status->showMessage(message, timeout);
}

void DevFindDialog::process()
{
	if ( !edit )
		return;

	f = text->text();
	s = edit->document()->toPlainText();
	QTextCursor cur(edit->textCursor());
	bw = backward->isChecked();
	ww = bound->isChecked();
	cs = (sensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive );
	pos = absPos = 0;
	
	if ( Ui::DevFindDialog::cursor->isChecked() )
		absPos = cur.position();
	
	if ( selection->isChecked() ) {
		s = cur.selectedText();
		absPos = bw ? cur.selectionEnd() : cur.selectionStart();
	} else {
		if ( Ui::DevFindDialog::cursor->isChecked() )
			absPos = cur.position();
		else if ( bw )
			absPos = s.count();

		s = bw ? s.mid(0, absPos) : s.mid(absPos);
	}
	
	if ( bw )		//start at string's end
		pos = -1;
	
	int i;
	cur = QTextCursor(edit->document());
	
	if ( ww ) {
		QRegExp xp(QString("\\b")+f+"\\b");
		xp.setCaseSensitivity(cs);
		
		i = bw ? s.lastIndexOf(xp, pos) : s.indexOf(xp, pos);
	} else {
		i = bw ? s.lastIndexOf(f, pos, cs) : s.indexOf(f, pos, cs);
	}

	if ( i == -1 ) {
		showMessage(tr("Unable to find : ") + f);
		return;
	}

	if ( bw )
		cur.setPosition(absPos-s.count()+i, QTextCursor::MoveAnchor);
	else
		cur.setPosition(absPos+i, QTextCursor::MoveAnchor);
	
	cur.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, f.count());
	
	edit->setTextCursor(cur);		//update document cursor
	color(cur);						//update current line highlighting
	
	pos = bw ? i - s.count() - f.count() : i + f.count();

}

DevReplaceDialog::DevReplaceDialog(QWidget *p)
 : QDialog(p, Qt::Dialog | Qt::WindowStaysOnTopHint)
{
	setWindowTitle(tr("Replace text"));
	
	QLabel *lbl;
	QGroupBox *g;
	QVBoxLayout *b;
	QGridLayout *l = new QGridLayout;
	
	find = new QLineEdit;
	lbl = new QLabel(tr("Text to Find"));
	l->addWidget(lbl, 0, 0, 1, 1);
	l->addWidget(find, 0, 1, 1, 3);
	
	replace = new QLineEdit;
	lbl = new QLabel(tr("Replace with"));
	l->addWidget(lbl, 1, 0, 1, 1);
	l->addWidget(replace, 1, 1, 1, 3);
	
	g = new QGroupBox(tr("Options"));
	b = new QVBoxLayout;
	
	sensitive = new QCheckBox(tr("Case &sensitive"));
	b->addWidget(sensitive);
	
	bound = new QCheckBox(tr("&Whole words only"));
	b->addWidget(bound);
	
	prompt = new QCheckBox(tr("&Prompt on replace"));
	b->addWidget(prompt);
	
	g->setLayout(b);
	l->addWidget(g, 2, 0, 4, 2);
	
	g = new QGroupBox(tr("Direction"));
	b = new QVBoxLayout;
	
	forward = new QRadioButton("Forward");
	forward->toggle();
	b->addWidget(forward);
	
	backward = new QRadioButton(tr("Backward"));
	b->addWidget(backward);
	
	g->setLayout(b);
	l->addWidget(g, 2, 2, 4, 2);
	
	g = new QGroupBox(tr("Scope"));
	b = new QVBoxLayout;
	
	global = new QRadioButton(tr("Global"));
	global->toggle();
	b->addWidget(global);
	
	selection = new QRadioButton(tr("Selected only"));
	b->addWidget(selection);
	
	g->setLayout(b);
	l->addWidget(g, 6, 0, 3, 2);
	
	g = new QGroupBox(tr("Origin"));
	b = new QVBoxLayout;
	
	cursor = new QRadioButton(tr("Cursor"));
	cursor->toggle();
	b->addWidget(cursor);
	
	whole = new QRadioButton(tr("Entire scope"));
	b->addWidget(whole);
	
	g->setLayout(b);
	l->addWidget(g, 6, 2, 3, 2);
	
	b_accept = new QPushButton(tr("&Ok"));
	connect(b_accept, SIGNAL( clicked() ),
			this	, SLOT  ( setVars() ) );
	connect(b_accept, SIGNAL( clicked() ),
			this	, SLOT  ( process() ) );
	l->addWidget(b_accept, 9, 0, 1, 1);
	
	b_all = new QPushButton(tr("&Replace all"));
	connect(b_all	, SIGNAL( clicked() ),
			this	, SLOT  ( all() ) );
	l->addWidget(b_all, 9, 1, 1, 1);
	
	b_cancel = new QPushButton(tr("&Cancel"));
	connect(b_cancel, SIGNAL( clicked() ),
			this	, SLOT  ( hide() ) );
	l->addWidget(b_cancel, 9, 3, 1, 1);
	
	setLayout(l);
}

DevReplaceDialog::~DevReplaceDialog()
{
	s.clear();
	f.clear();
	r.clear();
}

void DevReplaceDialog::execute(QTextEdit *e)
{
	if ( !e )
		return;
	
	edit = e;
	QTextCursor cur = e->textCursor();
	find->setText( cur.selectedText() );
	
	show();
}

void DevReplaceDialog::setVars()
{
	if ( !edit )
		return;
	
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
	if ( !edit )
		return;
	
	hide();
	
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
											tr("Search results"),
											tr("Unable to find : ") + f );
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
		int s = QMessageBox::warning(this, tr("Replacing..."),
							tr("Are you sure that you want to replace that?"),
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
	if ( !edit )
		return;
	
	while ( !abort )
		process();
}

DevAboutDialog::DevAboutDialog(QWidget *p)
 : QDialog(p)
{
	;
}

DevAboutDialog::~DevAboutDialog()
{
	;
}

DevPropertiesDialog::DevPropertiesDialog(QWidget *p)
 : QDialog(p)
{
	;
}

DevPropertiesDialog::~DevPropertiesDialog()
{
	;
}

void DevPropertiesDialog::execute(const QString& n)
{
	name = n;
	
	exec();
}

