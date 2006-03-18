#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
	setupUi(this);
	
	QFont font;
	font.setFamily("Courier New");
	font.setPointSize(10);
	font.setFixedPitch(true);

	editor = new Editor(this);
	editor->setFont(font);
	setCentralWidget(editor);

	configureActions();
}

MainWindow::~MainWindow() {
}

void MainWindow::configureActions() {
	// Definning icons...
    actionNew->setIcon(QIcon(QString::fromUtf8(":/icons/NEW")));
    actionOpen->setIcon(QIcon(QString::fromUtf8(":/icons/OPEN")));
    actionSave->setIcon(QIcon(QString::fromUtf8(":/icons/SAVE")));
    actionSave_As->setIcon(QIcon(QString::fromUtf8(":/icons/SAVEAS")));
    actionUndo->setIcon(QIcon(QString::fromUtf8(":/icons/UNDO")));
    actionRedo->setIcon(QIcon(QString::fromUtf8(":/icons/REDO")));
    actionCopy->setIcon(QIcon(QString::fromUtf8(":/icons/COPY")));
    actionCut->setIcon(QIcon(QString::fromUtf8(":/icons/CUT")));
    actionPaste->setIcon(QIcon(QString::fromUtf8(":/icons/PASTE")));
    actionPersisten_selection_mode->setIcon(QIcon(QString::fromUtf8(":/icons/PERSISTENTSELECT")));
    actionMultiline_edit_mode->setIcon(QIcon(QString::fromUtf8(":/icons/MULTILINEEDIT")));

	// Definning shotcurts...
	actionNew->setShortcut(tr("Ctrl+N"));
    actionOpen->setShortcut(tr("Ctrl+O"));
    actionSave->setShortcut(tr("Ctrl+S"));
    actionSave_As->setShortcut(tr("Ctrl+Shift+S"));
    actionClose->setShortcut(tr("Ctrl+W"));
    actionUndo->setShortcut(tr("Ctrl+Z"));
    actionRedo->setShortcut(tr("Ctrl+Y"));
    actionCopy->setShortcut(tr("Ctrl+C"));
    actionCut->setShortcut(tr("Ctrl+X"));
    actionPaste->setShortcut(tr("Ctrl+V"));
    actionSelect_All->setShortcut(tr("Ctrl+A"));

	// Definning toggled actions...
	actionPersisten_selection_mode->setCheckable(true);
	actionMultiline_edit_mode->setCheckable(true);

	// Connecting actions...
	connect(actionUndo, SIGNAL(triggered()), editor, SLOT(undo()) );
	connect(actionRedo, SIGNAL(triggered()), editor, SLOT(redo()) );
	connect(actionCopy, SIGNAL(triggered()), editor, SLOT(copy()) );
	connect(actionCut, SIGNAL(triggered()), editor, SLOT(cut()) );
	connect(actionPaste, SIGNAL(triggered()), editor, SLOT(paste()) );
	connect(actionSelect_All, SIGNAL(triggered()), editor, SLOT(selectAll()) );
	connect(actionPersisten_selection_mode, SIGNAL(toggled(bool)), editor, SLOT(setPersistentSelection(bool)) );
	connect(actionMultiline_edit_mode, SIGNAL(toggled(bool)), editor, SLOT(setMultilineEdit(bool)) );

	// Connecting feedbaks...
	connect(editor, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)) );
	actionCopy->setEnabled(false);

	connect(editor, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)) );
	actionCut->setEnabled(false);

	connect(editor, SIGNAL(undoAvailable(bool)), actionUndo, SLOT(setEnabled(bool)) );
	actionUndo->setEnabled(false);

	connect(editor, SIGNAL(redoAvailable(bool)), actionRedo, SLOT(setEnabled(bool)) );
	actionRedo->setEnabled(false);
}
