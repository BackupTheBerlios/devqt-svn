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

#include "devsettingsdialog.h"

#include "devapp.h"
#include "devgui.h"
#include "devedit.h"
#include "devsettings.h"

DevSettingsDialog::DevSettingsDialog(QWidget *p)
 : QDialog(p)
{
	setupUi(this);
	
	QFontDatabase db;
	
	editorFont->addItems( db.families() );
	gutterFont->addItems( db.families() );
	
	editorSize->setEditable(true);
	gutterSize->setEditable(true);

	foreach(int size, db.standardSizes())
	{
		editorSize->addItem(QString::number(size));
		gutterSize->addItem(QString::number(size));
	}
	
	connect(this, SIGNAL( accepted() ),
			this, SLOT  ( apply() ) );
}

void DevSettingsDialog::apply()
{
	// recent files/projects
	DEV_SETTINGS->beginGroup("recent");
	
	DEV_SETTINGS->setValue("filecount", iFiles->value());
	DEV_SETTINGS->setValue("projectcount", iProjects->value());
	
	DEV_SETTINGS->endGroup();
	// !recent
	
	// openned files/projects
	DEV_SETTINGS->beginGroup("openned");
	
	int rem =	bOFNone->isChecked() ? DevSettings::Forget : (
				bOFCurrent->isChecked() ? DevSettings::Active : (
				bOFAll->isChecked() ? DevSettings::All : false ) );
	
	DEV_SETTINGS->beginGroup("files");
	DEV_SETTINGS->setValue("rem", rem);
	DEV_SETTINGS->endGroup();
	
	rem =	bOPNone->isChecked() ? DevSettings::Forget : (
			bOPCurrent->isChecked() ? DevSettings::Active : (
			bOPAll->isChecked() ? DevSettings::All : false ) );
	
	DEV_SETTINGS->beginGroup("projects");
	DEV_SETTINGS->setValue("rem", rem);
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->endGroup();
	// !openned
	
	// editor widget
	DEV_SETTINGS->beginGroup("edit");
	
	DEV_SETTINGS->beginGroup("font");
	
	DEV_SETTINGS->setValue(	"editor",
						QFont(	editorFont->currentText(),
								editorSize->currentText().toInt() ) );
	DEV_SETTINGS->setValue("gutter",
						QFont(	gutterFont->currentText(),
								gutterSize->currentText().toInt() ) );
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->beginGroup("margin");
	DEV_SETTINGS->setValue("margin", iMargin->value());
	DEV_SETTINGS->setValue("draws", cbMargin->isChecked());
	//DEV_SETTINGS->setValue("brush").value<QBrush>() );
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->beginGroup("tabs");
	DEV_SETTINGS->setValue("size", iTab->value());
	DEV_SETTINGS->setValue("replace", !cbTab->isChecked());
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->beginGroup("wrap");
	DEV_SETTINGS->setValue("word", cbWordWrap->isChecked());
	DEV_SETTINGS->setValue("line", cbLineWrap->isChecked());
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->setValue("ctrl", cbCTRL->isChecked());
	DEV_SETTINGS->setValue("scroll", cbScroll->isChecked());
	DEV_SETTINGS->setValue("indent", cbIndent->isChecked());
	DEV_SETTINGS->setValue("autoclose", cbAutoClose->isChecked());
	
	DEV_SETTINGS->endGroup();
	// !editor
	
	// highlighting
	DEV_SETTINGS->beginGroup("highlight");
	
	DEV_SETTINGS->endGroup();
	// !highlighting
	
	DEV_SETTINGS->applyFormat( DEV_GUI->e );
}

void DevSettingsDialog::setDefault()
{
	DEV_SETTINGS->remove("edit");
	DEV_SETTINGS->remove("highlight");
	
	editorFont->setCurrentIndex(editorFont->findText("Courier New"));
	gutterFont->setCurrentIndex(gutterFont->findText(DevApp::font().family()));
	
	gutterSize->setCurrentIndex(editorSize->findText("10") );
	gutterSize->setCurrentIndex(gutterSize->findText(QString::number(
										DevApp::font().pointSize() ) ) );
	
}

void DevSettingsDialog::setCurrent()
{
	QFont editor, gutter;
	
	DEV_SETTINGS->beginGroup("edit");
	
	DEV_SETTINGS->beginGroup("font");
	editor = DEV_SETTINGS->value("editor").value<QFont>();
	gutter = DEV_SETTINGS->value("gutter").value<QFont>();
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->beginGroup("margin");
	iMargin->setValue( DEV_SETTINGS->value("margin").toInt() );
	cbMargin->setChecked( DEV_SETTINGS->value("draws").toBool() );
	//edit->setMarginBrush( value("brush").value<QBrush>() );
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->beginGroup("tabs");
	iTab->setValue( DEV_SETTINGS->value("size").toInt() );
	cbTab->setChecked( !DEV_SETTINGS->value("replace").toBool() );
	DEV_SETTINGS->endGroup();
	
	DEV_SETTINGS->beginGroup("wrap");
	cbWordWrap->setChecked( DEV_SETTINGS->value("word").toBool() );
	cbLineWrap->setChecked( DEV_SETTINGS->value("line").toBool() );
	DEV_SETTINGS->endGroup();
	
	cbCTRL->setChecked( DEV_SETTINGS->value("ctrl").toBool() );
	cbScroll->setChecked( DEV_SETTINGS->value("scroll").toBool() );
	cbIndent->setChecked( DEV_SETTINGS->value("indent").toBool() );
	cbAutoClose->setChecked( DEV_SETTINGS->value("autoclose").toBool() );
	
	DEV_SETTINGS->endGroup();
	
	editorFont->setCurrentIndex(editorFont->findText(editor.family()));
	gutterFont->setCurrentIndex(gutterFont->findText(gutter.family()));
	
	editorSize->setCurrentIndex(editorSize->findText(QString::number(
													editor.pointSize() ) ) );
	gutterSize->setCurrentIndex(gutterSize->findText(QString::number(
													gutter.pointSize() ) ) );
	
	
	DEV_SETTINGS->beginGroup("highlight");
	
	DEV_SETTINGS->endGroup();
	
}

void DevSettingsDialog::on_DefaultButton_clicked()
{
	setDefault();
	//update();
}

void DevSettingsDialog::on_editorFont_currentIndexChanged(const QString& text)
{
	QFont f(text, editorSize->currentText().toInt());
	editorOverview->setFont(f);
}

void DevSettingsDialog::on_gutterFont_currentIndexChanged(const QString& text)
{
	QFont f(text, gutterSize->currentText().toInt());
	gutterOverview->setFont(f);
}

void DevSettingsDialog::on_editorSize_currentIndexChanged(const QString& text)
{
	QFont f(editorFont->currentText(), text.toInt());
	editorOverview->setFont(f);
}

void DevSettingsDialog::on_gutterSize_currentIndexChanged(const QString& text)
{
	QFont f(gutterFont->currentText(), text.toInt());
	gutterOverview->setFont(f);
}

