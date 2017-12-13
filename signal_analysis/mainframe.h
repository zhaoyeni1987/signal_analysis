#ifndef MAINFRAME_H
#define MAINFRAME_H

#include <QtWidgets/QMainWindow>
#include "ui_mainframe.h"
#include <QAction>
#include <QToolBar>
#include "message.h"

class mainframe : public QMainWindow
{
	Q_OBJECT

public:
	mainframe(QWidget *parent = 0);
	~mainframe();

private:
	void CreateAction();
	void CreateToolBar();

	QAction* m_pActionOpen;
	QAction* m_pActionAutoView;
	QAction* m_pActionSave2Png;
	QToolBar* m_pToolBarFast;

	CMessage m_Message;

	Ui::mainframeClass ui;

	QVector<double>* m_pvTime;

private slots:
	void Open();
	void AutoView();
	void ClickedSignalList(QModelIndex index);
	void contextMenuRequest(QPoint pos);
	void SetX();
	void Plot();
	void CancelPlot();
	void Save2Png();
	void test();

	
};

#endif // MAINFRAME_H
