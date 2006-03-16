#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include "ui_mainwindow.h"
#include "editor.h"

class MainWindow : public QMainWindow, private Ui::MainWindow {
	Q_OBJECT
	
	public:
		MainWindow(QWidget *parent = 0);
		virtual ~MainWindow();
		
	protected:
		void configureActions();
		
	private:
		Editor *editor;
};

#endif /*MAINWINDOW_H_*/
