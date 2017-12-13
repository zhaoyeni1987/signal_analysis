#include "mainframe.h"
#include "signalfloat.h"
#include "signalint.h"
#include "signaluint.h"
#include "message.h"
#include <QFileDialog>

#define CHINESE QString::fromLocal8Bit

#define ICON_RUNNING		":/image/image/control_play.png"
#define ICON_STOP			":/image/image/control_stop.png"
#define ICON_SETTING		":/image/image/setting.png"
#define ICON_OPEN			":/mainframe/image/open_file.png"
#define ICON_AUTO_VIEW		":/mainframe/image/auto_view.png"
#define ICON_SAVE_TO_PNG	":/mainframe/image/save_to_png.png"

mainframe::mainframe(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	CreateAction();
	CreateToolBar();

	ui.plot->SetStyle(EN_DARK);

	ui.listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(ui.listWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

	m_pvTime = NULL;
}

mainframe::~mainframe()
{

}

void mainframe::CreateAction()
{
	m_pActionOpen = new QAction(QIcon(ICON_OPEN), CHINESE("打开"), this);
	connect(m_pActionOpen, SIGNAL(triggered()), this, SLOT(Open()));

	m_pActionAutoView = new QAction(QIcon(ICON_AUTO_VIEW), CHINESE("视图适应"), this);
	connect(m_pActionAutoView, SIGNAL(triggered()), this, SLOT(AutoView()));

	m_pActionSave2Png = new QAction(QIcon(ICON_SAVE_TO_PNG), CHINESE("保存为PNG"), this);
	connect(m_pActionSave2Png, SIGNAL(triggered()), this, SLOT(Save2Png()));
}

void mainframe::CreateToolBar()
{
	m_pToolBarFast = addToolBar(CHINESE("快捷栏"));

	m_pToolBarFast->addAction(m_pActionOpen);
	m_pToolBarFast->addAction(m_pActionAutoView);
	m_pToolBarFast->addAction(m_pActionSave2Png);
}

void mainframe::Open()
{
	QString filename = QFileDialog::getOpenFileName(this, CHINESE("打开二进制文件"), "", "", 0);
	if (!filename.isNull())
	{
		m_Message.OpenBinFile(filename);

		m_Message.AddSignal(new CSignalUInt("head", 2));
		m_Message.AddSignal(new CSignalUInt("len", 1));
		m_Message.AddSignal(new CSignalUInt("seq", 1));
		m_Message.AddSignal(new CSignalUInt("dev", 1));
		m_Message.AddSignal(new CSignalUInt("m_Message", 1));
		m_Message.AddSignal(new CSignalFloat("flytime", 4, 0.0001));
		m_Message.AddSignal(new CSignalUInt("sw_ctrl_sta", 4));
		m_Message.AddSignal(new CSignalUInt("drive1_sta", 4));
		m_Message.AddSignal(new CSignalUInt("drive2_sta", 4));
		m_Message.AddSignal(new CSignalUInt("input1_sta", 2));
		m_Message.AddSignal(new CSignalUInt("servo1_com_sta", 1));
		m_Message.AddSignal(new CSignalFloat("actuator_ctrl_1", 2, 6.104e-4));
		m_Message.AddSignal(new CSignalFloat("actuator_ctrl_2", 2, 6.104e-4));
		m_Message.AddSignal(new CSignalFloat("actuator_ctrl_3", 2, 6.104e-4));
		m_Message.AddSignal(new CSignalFloat("actuator_ctrl_4", 2, 6.104e-4));
		m_Message.AddSignal(new CSignalFloat("actuator_res_1", 2, 0.001069));
		m_Message.AddSignal(new CSignalFloat("actuator_res_2", 2, 0.001069));
		m_Message.AddSignal(new CSignalFloat("actuator_res_3", 2, 0.001069));
		m_Message.AddSignal(new CSignalFloat("actuator_res_4", 2, 0.001069));
		m_Message.AddSignal(new CSignalUInt("res", 1));
		m_Message.AddSignal(new CSignalUInt("res", 1));
		m_Message.AddSignal(new CSignalUInt("servo1_ctrl_time", 4));
		m_Message.AddSignal(new CSignalFloat("servo1_5V", 1, 0.0390625));
		m_Message.AddSignal(new CSignalFloat("servo1_28V", 1, 0.1953125));
		m_Message.AddSignal(new CSignalFloat("servo1_160V", 2, 0.00457763));
		m_Message.AddSignal(new CSignalFloat("motor_speed_1", 2, 0.30519));
		m_Message.AddSignal(new CSignalFloat("motor_speed_2", 2, 0.30519));
		m_Message.AddSignal(new CSignalFloat("motor_speed_3", 2, 0.30519));
		m_Message.AddSignal(new CSignalFloat("motor_speed_4", 2, 0.30519));
		m_Message.AddSignal(new CSignalFloat("motor_current_1", 1, 0.788));
		m_Message.AddSignal(new CSignalFloat("motor_current_2", 1, 0.788));
		m_Message.AddSignal(new CSignalFloat("motor_current_3", 1, 0.788));
		m_Message.AddSignal(new CSignalFloat("motor_current_4", 1, 0.788));
		m_Message.AddSignal(new CSignalUInt("crc", 2));

		QVector<QString> listSignal = m_Message.GetSignalNameList();
		for(int i = 0; i<listSignal.size(); i++)
			ui.listWidget->addItem(listSignal[i]);

		m_Message.Analysis();
	}
}

void mainframe::AutoView()
{
	ui.plot->RescaleAll();
}

void mainframe::test()
{

}

void mainframe::ClickedSignalList(QModelIndex index)
{
	int i = 0;
}

void mainframe::SetX()
{
	unsigned int index = ui.listWidget->currentIndex().row();

	m_pvTime = m_Message.GetValue(index);
}

void mainframe::Plot()
{
	unsigned int index = ui.listWidget->currentIndex().row();

	QVector<double>* m_pvValue = m_Message.GetValue(index);

	if(m_pvTime != NULL)
		ui.plot->AddData(*m_pvTime, *m_pvValue, m_Message.GetSignalNameList()[index]);
}

void mainframe::CancelPlot()
{
	QString strGraphName = ui.listWidget->currentIndex().data().toString();

	ui.plot->RemoveGraph(strGraphName);
}

void mainframe::Save2Png()
{
	QString filename = QFileDialog::getSaveFileName(this, CHINESE("打开二进制文件"), "", "Png files (*.png)", 0);
	if (!filename.isNull())
	{
		ui.plot->Save2Png(filename);
	}
}

void mainframe::contextMenuRequest(QPoint pos)
{
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	menu->addAction("Set X", this, SLOT(SetX()));
	menu->addAction("Plot curve", this, SLOT(Plot()));
	menu->addAction("Cancel", this, SLOT(CancelPlot()));

	menu->exec(mapToGlobal(pos));
}

#if 0
void mainframe::test()
{
	CMessage msg;
	msg.OpenBinFile("bin.dat");

	msg.AddSignal(new CSignalUInt("head", 2));
	msg.AddSignal(new CSignalUInt("len", 1));
	msg.AddSignal(new CSignalUInt("seq", 1));
	msg.AddSignal(new CSignalUInt("dev", 1));
	msg.AddSignal(new CSignalUInt("msg", 1));
	msg.AddSignal(new CSignalFloat("flytime", 4, 0.0001));
	msg.AddSignal(new CSignalUInt("sw_ctrl_sta", 4));
	msg.AddSignal(new CSignalUInt("drive1_sta", 4));
	msg.AddSignal(new CSignalUInt("drive2_sta", 4));
	msg.AddSignal(new CSignalUInt("input1_sta", 2));
	msg.AddSignal(new CSignalUInt("servo1_com_sta", 1));
	msg.AddSignal(new CSignalFloat("actuator_ctrl_1", 2, 6.104e-4));
	msg.AddSignal(new CSignalFloat("actuator_ctrl_2", 2, 6.104e-4));
	msg.AddSignal(new CSignalFloat("actuator_ctrl_3", 2, 6.104e-4));
	msg.AddSignal(new CSignalFloat("actuator_ctrl_4", 2, 6.104e-4));
	msg.AddSignal(new CSignalFloat("actuator_res_1", 2, 0.001069));
	msg.AddSignal(new CSignalFloat("actuator_res_2", 2, 0.001069));
	msg.AddSignal(new CSignalFloat("actuator_res_3", 2, 0.001069));
	msg.AddSignal(new CSignalFloat("actuator_res_4", 2, 0.001069));
	msg.AddSignal(new CSignalUInt("res", 1));
	msg.AddSignal(new CSignalUInt("res", 1));
	msg.AddSignal(new CSignalUInt("servo1_ctrl_time", 4));
	msg.AddSignal(new CSignalFloat("servo1_5V", 1, 0.0390625));
	msg.AddSignal(new CSignalFloat("servo1_28V", 1, 0.1953125));
	msg.AddSignal(new CSignalFloat("servo1_160V", 2, 0.00457763));
	msg.AddSignal(new CSignalFloat("motor_speed_1", 2, 0.30519));
	msg.AddSignal(new CSignalFloat("motor_speed_2", 2, 0.30519));
	msg.AddSignal(new CSignalFloat("motor_speed_3", 2, 0.30519));
	msg.AddSignal(new CSignalFloat("motor_speed_4", 2, 0.30519));
	msg.AddSignal(new CSignalFloat("motor_current_1", 1, 0.788));
	msg.AddSignal(new CSignalFloat("motor_current_2", 1, 0.788));
	msg.AddSignal(new CSignalFloat("motor_current_3", 1, 0.788));
	msg.AddSignal(new CSignalFloat("motor_current_4", 1, 0.788));
	msg.AddSignal(new CSignalUInt("crc", 2));

	unsigned int length = msg.GetLength();

	msg.SaveFile("temp");

}

#endif
