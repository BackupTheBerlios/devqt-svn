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

#include "devsettings.h"

#include "devgui.h"
#include "devedit.h"
#include "coreedit.h"
#include "linenumberpanel.h"
#include "devstandardpanel.h"
#include "devsettingsdialog.h"

/*

Settings map:
	
|		purpose			|		key				|			type			|
|---------------------------------------------------------------------------|
|	max nb of files : 	|	recent/filecount	|			int				|
|	recent files : 		|	recent/files		|		QStringList			|
|	max nb of projects :|	recent/projectcount	|			int				|
|	recent projects : 	|	recent/projects		|		QStringList			|
|-----------------------|-----------------------|---------------------------|
|	remember files :	|	openned/files/rem	|			int				|
|	openned files : 	|	openned/files/dat	|		QStringList			|
|	openned projects : 	|	openned/projects/rem|			int				|
|	openned projects : 	|	openned/projects/dat|		QStringList			|
|-----------------------|-----------------------|---------------------------|
|	main win state :	|	gui/state			|		Qt::WindowStates	|
|	main win title : 	|	gui/title			|			QString			|
|	main win width :	|	gui/width			|			int				|
|	main win height : 	|	gui/height			|			int				|
|	main win position : |	gui/pos				|			QPoint			|
|-----------------------|-----------------------|---------------------------|
|	insert mode			|	edit/insert			|			bool			|
|	auto indent			|	edit/indent			|			bool			|
|	scrollbar as needed	|	edit/scroll			|			bool			|
|	auto close chars	|	edit/autoclose		|			bool			|
|	CTRL navigation		|	edit/ctrl			|			bool			|
|	editor font			|	edit/font/editor	|			QFont			|
|	gutter font			|	edit/font/gutter	|			QFont			|
|	tab width			|	edit/tabs/size		|			int				|
|	tab replace			|	edit/tabs/replace	|			bool			|
|	word wrap			|	edit/wrap/word		|			bool			|
|	line wrap			|	edit/wrap/line		|			bool			|
|	draws margin		|	edit/margin/draws	|			bool			|
|	margin size			|	edit/margin/margin	|			int				|
|	margin color		|	edit/margin/brush	|			QBrush			|
|-----------------------|-----------------------|---------------------------|
|	higlight formats	|	highlight/formats	|	QList<QTextCharFormat>	|
|	higlight line		|	highlight/line		|			bool			|
|	highlight block		|	highlight/block		|			bool			|
|---------------------------------------------------------------------------|

*/

DevSettings* DevSettings::inst = 0;

#ifdef Q_WS_WIN
	const QString DevSettings::PATH_VAR = "PATH";
#elif defined(Q_WS_X11)
	const QString DevSettings::PATH_VAR = "PATH";
#elif defined(Q_WS_MAC)
	const QString DevSettings::PATH_VAR = "PATH";
#endif

DevSettings* DevSettings::Instance()
{
	if ( !inst )
		inst = new DevSettings;
		
	return inst;
}

DevSettings::DevSettings(QWidget *p)
 : QSettings(p)
{
	dlg = new DevSettingsDialog(p);
	
	m = new QMenu(tr("&Reopen..."), p);
	m->setIcon(QIcon(":/reopen.png"));
	
	aClear = new QAction(QIcon(":/clear.png"), tr("&Clear history"), this);
	connect(aClear	, SIGNAL( triggered() ),
			this	, SLOT  ( clearRecents() ) );
	recent();
}

DevSettings::~DevSettings()
{
	//dlg->apply();
}

void DevSettings::killSettings()
{
	if ( inst )
	{
		qDebug("closing settings...");
		delete inst;
	}
}

QString DevSettings::make()
{
	#ifdef Q_WS_WIN
	return "mingw32-make";
	#elif defined(Q_WS_X11)
	return "make";
	#elif defined(Q_WS_MAC)
	return "make"; 		//Is that true???
	#endif
}

QStringList DevSettings::environment(const QStringList& dirs)
{
	QStringList l = QProcess::systemEnvironment();
	
	if ( dirs.isEmpty() )
		return l;
	
	QString PATH;
	
	QMessageBox::warning(0, 0, l.join("\n"));
	
	for ( QStringList::iterator i = l.begin(); i != l.end(); i++ )
	{
		if ( !(*i).startsWith(PATH_VAR) )
			continue;
		
		foreach ( QString dir, dirs )
			(*i) += QString(";") + dir;
		
		QMessageBox::warning(0, 0, (*i).split(';').join("\n"));
	}
	
	
	return l;
}

QStringList DevSettings::includes()
{
	QStringList l;
	
	return l;
}

void DevSettings::applyFormat(DevEdit *e)
{
	if ( !e )
		return;
	
// 	TODO: do we really need this debug output...?	
// 	qDebug("applying format to [%s]... ",	qPrintable(e->name()) );
	
	CoreEdit *edit = e->e;
	DevStandardPanel *s = e->s;
	
	beginGroup("edit");
	
	beginGroup("font");
	edit->setFont( value("editor").value<QFont>() );
	s->l->setFont( value("gutter").value<QFont>() );
	endGroup();
	
	beginGroup("margin");
	edit->setMargin( value("margin", 80).toInt() );
	edit->setDrawMargin( value("draws", true).toBool() );
	//edit->setMarginBrush( value("brush").value<QBrush>() );
	endGroup();
	
	beginGroup("tabs");
	edit->setTabstop( value("size", 4).toInt() );
	edit->setReplaceTab( value("replace", true).toBool() );
	endGroup();
	
	beginGroup("wrap");
	edit->setWordWrapMode( value("word", false).toBool()
							? QTextOption::WordWrap : QTextOption::NoWrap );
	edit->setLineWrapMode( value("line", false).toBool()
							? QTextEdit::WidgetWidth : QTextEdit::NoWrap );
	endGroup();
	
	edit->setAutoIndent( value("indent", true).toBool() );
	edit->setAutoClose( value("autoclose", false).toBool() );
	edit->setCtrlNavigation( value("ctrl", true).toBool() );
	
	endGroup();
	
	beginGroup("highlight");
	
	endGroup();
}

int DevSettings::tabStop()
{
	return 4;
}

QMenu* DevSettings::recent()
{
	m->clear();
	m->disconnect();
	recents.clear();
	
	m->addAction(aClear);
	
	int n = 0;
	QString name, s, prefix;
	
	QStringList l, keys = QStringList()<<"projects"<<"files";
	
	beginGroup("recent");
	
	foreach ( QString name, keys )
	{
		l = value(name).toStringList();
		
		m->addSeparator();
		
		foreach ( QString s, l )
		{
			prefix = QString::number(n) + " : ";
			
			if ( n++ < 10 )
				prefix.prepend('&');
			
			QAction *a = new QAction(prefix + s, this);
			m->addAction(a);
			
			recents.insert(a, s);
		}
	}
	
	endGroup();
	
	connect(m	, SIGNAL( triggered(QAction*) ),
			this, SLOT  ( recent(QAction*) ) );
	
	return m;
}

void DevSettings::execute()
{
	dlg->exec();
}

void DevSettings::clearRecents()
{
	remove("recent");
}

void DevSettings::addRecent(const QString& n, bool project)
{
	if ( n.isEmpty() )
		return;
	
	int max;
	QString s;
	QStringList l;
	
	beginGroup("recent");
	
	if ( project )
	{
		max = value("projectcount", 5).toInt();
		s = "projects";
	} else {
		max = value("filecount", 15).toInt();
		s = "files";
	}
	
	l = value(s).toStringList();
	
	for( int i = 0; i < l.size(); i++)
		if ( l.at(i) == n )
			l.removeAt(i);
	
	l.prepend(n);
	
	while ( l.size() > max )
		l.pop_back();
	
	setValue(s, l);
	
	endGroup();
}

void DevSettings::recent(QAction *a)
{
	QHash<QAction*, QString>::iterator i = recents.find(a);
	
	if ( i == recents.end() )
		return;
	
	DEV_GUI->load(*i);
}
