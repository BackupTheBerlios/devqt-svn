#ifndef EDITORWIDGET_H_
#define EDITORWIDGET_H_

#include <QWidget>

class EditorWidget : public QWidget {
	Q_OBJECT
	
	public:
		EditorWidget(QWidget *parent = 0);
		virtual ~EditorWidget();
};

#endif /*EDITORWIDGET_H_*/
