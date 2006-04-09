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

#include "devconsole.h"

#include "devsettings.h"
#include "devfileinfo.h"

DevConsole::DevConsole(QWidget *p)
 : QFrame(p/*, Qt::WindowStaysOnTopHint*/), n(0)
{
	QLabel *lbl;
	QGridLayout *grid;
	
	grid	= new QGridLayout;
	
	lbl = new QLabel(tr("Enter command : "));
	cmd = new QComboBox;
	cmd->setEditable(true);
	
	grid->addWidget(lbl, 0, 0);
	grid->addWidget(cmd, 0, 1, 1, 5);
	
	log = new QTextEdit;
	log->setReadOnly(true);
	log->setAcceptRichText(false);
	log->setLineWrapMode(QTextEdit::NoWrap);
	log->setWordWrapMode(QTextOption::NoWrap);
	
	grid->addWidget(log, 1, 0, 5, 6);
	
	setLayout(grid);
	
	connect(cmd	, SIGNAL( activated(const QString&) ),
			this, SLOT  ( command(const QString&) ) );
	
	QString command;
	proc = new QProcess(this);
	
	dir = QDir::currentPath().replace('\\', '/');
	proc->setWorkingDirectory(dir);
	
	proc->setEnvironment(DEV_SETTINGS->environment());
	proc->setReadChannelMode(QProcess::MergedChannels);
	
	connect(proc, SIGNAL( readyRead() ),
			this, SLOT  ( read() ) );
	
	connect(proc, SIGNAL( error(QProcess::ProcessError) ),
			this, SLOT  ( error(QProcess::ProcessError) ) );
	
	connect(proc, SIGNAL( stateChanged(QProcess::ProcessState) ),
			this, SLOT  ( stateChanged(QProcess::ProcessState) ) );
	
	display( QString("# working directory : ") + dir );
}

DevConsole::~DevConsole()
{
}

void DevConsole::wait()
{
	while ( proc->state() != QProcess::NotRunning )
		qApp->processEvents();
}

void DevConsole::exec(const QString& cmd, const QString& dir, const QStringList& params)
{
	n = 0;
	QString cur = QDir::currentPath(), com(cmd);
	
	if ( !dir.isEmpty() )
	{
		QDir::setCurrent(dir); //this is the trick!!!
		proc->setWorkingDirectory(dir);
		proc->setEnvironment( DEV_SETTINGS->environment() );
		
		com = cmd;
	}
	
	display(proc->workingDirectory() + ">" + com + " " + params.join(" ") + "\n");
	
	if ( com.contains(32) )
	{
		#ifdef Q_WS_WIN
		com = QString("\"") + com + "\"";
		#else
		com = QString("\'") + com + "\'";
		#endif
	}
	
	QStringList p;
	
	foreach (QString s, params)
	{
		if ( s.contains(32) )
		{
			#ifdef Q_WS_WIN
			p<< QString("\"") + com + "\"";
			#else
			p<< QString("\'") + com + "\'";
			#endif
		}
		else
		{
			p<<s;
		}
	}
	
	proc->start(com, p);
	
	QDir::setCurrent(cur); // get back our working dir...
}

void DevConsole::error(QProcess::ProcessError err)
{
	n++;
	
	switch ( err )
	{
		case QProcess::FailedToStart:
				QMessageBox::warning(0, "Error: FailedToStart",
					tr("Unable to start process..."));
				break;
			
		case QProcess::Crashed:
				QMessageBox::warning(0, "Error: Crashed",
					tr("Process crashed..."));
				break;
			
		case QProcess::Timedout:
				QMessageBox::warning(0, "Error: Timedout",
					tr("Time out ended..."));
				break;
		
		case QProcess::WriteError:
				QMessageBox::warning(0, "Error: WriteError",
					tr("Unable to send data to process..."));
				break;
		
		case QProcess::ReadError:
				QMessageBox::warning(0, "Error: ReadError",
					tr("Unable to recieve data from process..."));
				break;
		
		case QProcess::UnknownError:
				QMessageBox::warning(0, "Error: UnknownError",
					tr("An unknown error occured..."));
				break;
	}
	
}

void DevConsole::stateChanged(QProcess::ProcessState state)
{
	switch (state)
	{
		case QProcess::Starting:
			//QMessageBox::warning(0, 0, "Starting...");
			break;
		
		case QProcess::NotRunning:
			//QMessageBox::warning(0, 0, "Stopping...");
			break;
		
		case QProcess::Running:
			//QMessageBox::warning(0, 0, "Running...");
			break;
		
		default:
			break;
	}
}

void DevConsole::read()
{
	display( proc->readAll() );
}

void DevConsole::directory(const QString& msg)
{
	QString s(msg.trimmed());
	
	QStringList l( s.replace('\\', '/').split('/', QString::SkipEmptyParts) );
	QStringList d( dir.split('/', QString::SkipEmptyParts) );
	
	foreach ( s, l )
	{
		if( s == "." )
		{
			continue;
		} else if ( s == ".." ) {
			if ( !d.size() )
				return display("# invalid path! working directory unchanged.");
			
			d.removeLast();
		} else {
			d.append(s);
		}
	}
	
	dir = d.join("/");
	
	display( QString("# switching to dir : ") + dir + "\n" );
}

void DevConsole::command(const QString& msg)
{
	cmd->setEditText("");
	
	if ( msg.isEmpty() )
		return;
	
	if ( (msg == "cls") || (msg == "clear") )
		return log->clear();
	
	if ( msg.startsWith("cd") )
		return directory(msg.mid(2));
	
	int i = 0;
	QString cmd;
	QStringList tmp, args;
	
	tmp = msg.split(' ', QString::SkipEmptyParts);
	
	cmd = tmp.at(i++);
	
	while ( i< tmp.size() )
	{
		if ( tmp.at(i).startsWith('-') )
		{
			QString arg("\"");
			arg += tmp.at(i++);
			arg += ' ';
			arg += tmp.at(i);
			arg += "\"";
			
			args<<arg;
		} else {
			args<<tmp.at(i);
		}
		i++;
	}
	
	exec(cmd, dir, args);
}

void DevConsole::display(const QString& msg)
{
	log->append(msg);
	//log->setPlainText( log->toPlainText() + msg );
	
	//log->setTextCursor( QTextCursor( log->document()->end() ) );
}
