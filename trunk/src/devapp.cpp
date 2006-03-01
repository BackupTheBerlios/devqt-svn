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

#include "devapp.h"

#include "devgui.h"
#include "devsplash.h"
#include "devstatus.h"
#include "devsettings.h"

DevApp* DevApp::_app = 0;

DevApp* DevApp::Instance()
{
	return _app;
}

DevApp* DevApp::Init(int argc, char **argv)
{
	if (_app != 0)
		delete _app;
	_app = new DevApp(argc, argv);
	return _app;
}

DevApp::DevApp(int argc, char **argv)
 : QApplication(argc, argv)
{
	// Setup global app style
	#ifdef WIN32
	setStyle("plastique");
	setPalette(style()->standardPalette());
	#endif
	
	// Setup app stuffs needed for QSettings use
	setApplicationName("DevQt");
	setOrganizationName("FullMetalCoder");
	setOrganizationDomain("http://devqt.berlios.de/");
	
	
	// Setup splash screen
	scr = DevSplash::Instance();
	
	// Setup Locale Language
	scr->showMessage("Initializing langage...", Qt::AlignLeft | Qt::AlignBottom, Qt::white);
	
	QTranslator *translator = new QTranslator(this);
	
	QString lang = QLocale::system().name();
	translator->load(lang, QFileInfo(*argv).path()+"/translations/");
	installTranslator(translator);
	
	// Get back app settings
	scr->showMessage("Getting back settings...", Qt::AlignLeft | Qt::AlignBottom, Qt::white);
	
	set = DEV_SETTINGS;
	
	// Setup main window
	scr->showMessage("Initializing UI...", Qt::AlignLeft | Qt::AlignBottom, Qt::white);
	gui = DEV_GUI;
	gui->s->connection();
	
	
	scr->showMessage("DevQt initialized.", Qt::AlignLeft | Qt::AlignBottom, Qt::white);
	
	gui->show();
}

void DevApp::killApp()
{
	if ( _app )
	{
		qDebug("App terminating....");
		delete _app;
	}
}

DevApp::~DevApp()
{
	set->killSettings();
	gui->killGUI();
	qDebug("Shutting down application.");
}
