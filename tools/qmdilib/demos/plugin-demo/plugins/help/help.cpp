#include <QAction>
#include <QProcess>
#include <QApplication>
#include <QUrl>
#include <QLibraryInfo>
#include <QMessageBox>
#include <QMainWindow>
#include <QStatusBar>

#include "iplugin.h"
#include "qmdiserver.h"
#include "help.h"
#include "helpbrowse.h"


HelpPlugin::HelpPlugin()
	: IPlugin()
{
	actionAbout	= new_action( QIcon(), tr("&About"), this, "", tr("XXXXX"), SLOT(showAboutApp()) );
	actionAboutQt	= new_action( QIcon(), tr("&About Qt"), this, "", tr("XXXXX"), SLOT(showAboutQt()) );
	actionShowQtHelp= new_action( QIcon(), tr("&Qt help"), this, "F1", tr("XXXXX"), SLOT(showQtHelp()) );

// 	actionAbout->setShortcut( "F1" );

	name = "Help plugin";
	author = "Diego Iastrubni <elcuco@kde.org>";
	iVersion = 0;
	sVersion = "0.0.1";
	autoEnabled = true;
	alwaysEnabled = true;

	menus["&Help"]->addAction( actionShowQtHelp );
	menus["&Help"]->addSeparator();
	menus["&Help"]->addAction( actionAbout );
	menus["&Help"]->addAction( actionAboutQt );

// 	externalBrowser = "/opt/kde3/bin/konqueror";
}

HelpPlugin::~HelpPlugin()
{
	delete actionAbout;
	delete actionAboutQt;
	delete actionShowQtHelp;
}

void HelpPlugin::showAbout()
{
}

void HelpPlugin::showAboutApp()
{
	QMessageBox::about( 0, "qmdilib demo", "A small demo for loading plugins<br>Diego Iastrubni (elcuco@kde.org) - lgpl)" );
}

void HelpPlugin::showAboutQt()
{
	QApplication::aboutQt();
}

void HelpPlugin::showQtHelp()
{
	QString helpFile = QLibraryInfo::location(QLibraryInfo::DocumentationPath) + QLatin1String("/html/index.html");
	
	QexHelpBrowser *browser = new QexHelpBrowser( QUrl(helpFile) );
	browser->hide();
	browser->name = "Qt help";
	browser->setObjectName( browser->name );
	connect( browser, SIGNAL(sourceChanged(QUrl)), this, SLOT(on_browser_sourceChanged(QUrl)));
	
	mdiServer->addClient( browser );
}

void HelpPlugin::on_browser_sourceChanged ( const QUrl & src )
{
	if (externalBrowser.isEmpty())
		return;

	if ((src.scheme() == "file") || (src.scheme().isEmpty()))
		    return;
	
	if (!QProcess::startDetached( externalBrowser, QStringList(src.toString()) ))
	{
		QWidget *w = dynamic_cast<QWidget*>(mdiServer);
		if (w == NULL)
			return;
			
		QMainWindow *ww = dynamic_cast<QMainWindow*>(w->window());
		if (ww)
			ww->statusBar()->showMessage("Error: could not start external browser", 5000);
	}
}
