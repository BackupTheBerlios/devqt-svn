#ifndef __CONFIG_DIALOG_H__
#define __CONFIG_DIALOG_H__

#include <QObject>
#include <QDialog>
#include "ui_configdialog.h"

class DevSettingsDialog: public QDialog, private Ui::ConfigurationDlg
{
	Q_OBJECT
public:
	DevSettingsDialog( QWidget *parent=NULL );
	void updateWidgetsFromSettings();

private slots:
	void on_applyButton_clicked();
	void on_buttonChooseFont_clicked();
};

#endif // __CONFIG_DIALOG_H__
