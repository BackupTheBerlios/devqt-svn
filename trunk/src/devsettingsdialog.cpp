#include <QFontDialog>

// #include "editorsettings.h"
#include "devsettingsdialog.h"

/**
 * \class DevSettingsDialog
 * \brief The dialog seen on screen when you want to configure DevQt
 *
 * When a user want to configure DevQt, this window will be a graphical
 * interface to DevSettings. All options displayed on screen will be applied
 * to DevSettings when the user presses "apply", and then DevSettings will
 * emit a signal to announce that settings have been changed, and all the 
 * clients which read that signal will read the settings from DevSettings
 * and apply them. 
 * 
 * Currently this class is a skeleton, since DevSettings lacks the ability to infrom
 * that changes have been made.
 * 
 * This form uses double inheritance (from QDialog and the UI made by the designer).
 * This form uses auto connection of slots.
 * 
 * \see DevSettings
 */


DevSettingsDialog::DevSettingsDialog(QWidget *parent):QDialog(parent)
{
	setupUi( this );
	updateWidgetsFromSettings();
}

void DevSettingsDialog::updateWidgetsFromSettings()
{
#if 0
	EditorSettings *editSettings = EditorSettings::getInstance();

	cbLineNumbers->setChecked( editSettings->showLineNumbers );
	cbWordWrap->setChecked( editSettings->lineWrap );
	cbMarkCurrentLine->setChecked( editSettings->markCurrentLine );

	fontPreview->setText( editSettings->font.toString() );
	fontPreview->setFont( editSettings->font );
	fontPreview->setCursorPosition( 0 );
#endif
}

// auto conncted by Qt
void DevSettingsDialog::on_applyButton_clicked()
{
#if 0
	EditorSettings *editSettings = EditorSettings::getInstance();

	editSettings->showLineNumbers	= cbLineNumbers->isChecked();
	editSettings->markCurrentLine	= cbMarkCurrentLine->isChecked();
	editSettings->lineWrap		= cbWordWrap->isChecked();
	editSettings->font		= fontPreview->font();

	editSettings->announceChange();
#endif
}

// auto conncted by Qt
void DevSettingsDialog::on_buttonChooseFont_clicked()
{
	bool ok;

//	TODO	
//	EditorSettings *editSettings = EditorSettings::getInstance();
//	QFont font = editSettings->font;

	QFont font = fontPreview->font();
	font = QFontDialog::getFont(&ok, font, this);
	if (!ok)
		return;

	fontPreview->setText( font.toString() );
	fontPreview->setFont( font );
	fontPreview->setCursorPosition( 0 );
}
