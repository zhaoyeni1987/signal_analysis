#include "zplot.h"

#define GRAPH_CURVE		0
#define INDICTOR_LINE	1

ZPlot::ZPlot(QWidget* parent) : QCustomPlot(parent)
{
	setOpenGl(true);

	setInteractions(QCP::iRangeDrag 
					| QCP::iRangeZoom 
					| QCP::iSelectAxes 
					| QCP::iSelectLegend 
					| QCP::iSelectPlottables);
	//plotView->graph(0)->setSelectable(QCP::stSingleData);
	//plotView->graph(1)->setSelectable(QCP::stNone);
	m_pTitle = NULL;
	m_pRubberBand = new QRubberBand(QRubberBand::Rectangle, this);

	m_pClickedText = NULL;

//set label
	xAxis->setLabel("time");
	yAxis->setLabel("value");

//set lengend
	legend->setVisible(true);

	QFont legendFont = font();
	legendFont.setPointSize(10);
	legend->setFont(legendFont);
	legend->setSelectedFont(legendFont);
	legend->setSelectableParts(QCPLegend::spItems); // legend box shall not be selectable, only legend items

	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequest(QPoint)));

//
	connect(this, SIGNAL(mousePress(QMouseEvent*)),
		this, SLOT(MousePressEvent(QMouseEvent*)));
	connect(this, SIGNAL(mouseMove(QMouseEvent*)),
		this, SLOT(MouseMoveEvent(QMouseEvent*)));
	connect(this, SIGNAL(mouseRelease(QMouseEvent*)),
		this, SLOT(MouseReleaseEvent(QMouseEvent*)));
	//connect(this, SIGNAL(selectionChangedByUser()), 
	//	this, SLOT(SelectionChanged()));

	setFocusPolicy(Qt::StrongFocus);
}
ZPlot::~ZPlot()
{

}

void ZPlot::SetTitle(QString strTitle)
{
	if (m_pTitle == NULL)
	{
		m_pTitle = new QCPTextElement(this, strTitle, QFont("sans", 17, QFont::Bold));
		plotLayout()->addElement(0, 0, m_pTitle);
	}
	else
	{
		delete m_pTitle;
		m_pTitle = new QCPTextElement(this, strTitle, QFont("sans", 17, QFont::Bold));
		plotLayout()->addElement(0, 0, m_pTitle);
	}
}

void ZPlot::SetupVerticalLine(double x, double y_length, bool visibale)
{
	if (visibale)
	{
		QVector<double> vX, vY;
		vX << x; vX << x;
		vY << (y_length*10); vY << (-y_length * 10);
		QCPGraph* pGraph = m_vGraph[m_vGraph.size() - 1].pGraph;
		pGraph->setData(vX, vY);
		pGraph->setVisible(true);
	}
	else
	{
		QVector<double> vX, vY;
		vX << 100; vX << 100;
		vY << 100; vY << 100;
		QCPGraph* pGraph = m_vGraph[m_vGraph.size() - 1].pGraph;
		pGraph->setData(vX, vY);
		pGraph->setVisible(false);
	}
}
void ZPlot::MouseMoveEvent(QMouseEvent* event)
{
	QPointF clickedPoint = event->localPos();
	if (!viewport().contains(event->pos()))
		return;

	if (m_vGraph.size() <= 0)
		return;

	double position_x = xAxis->pixelToCoord(clickedPoint.x());
	double y_length = yAxis->range().upper - xAxis->range().lower;

	SetupVerticalLine(position_x, y_length, true);

	if (m_pRubberBand->isVisible())
		m_pRubberBand->setGeometry(QRect(m_RubberOrigin, event->pos()).normalized());

	replot();
}

void ZPlot::MousePressEvent(QMouseEvent* event)
{
	if (!((event->button() == Qt::LeftButton)
		|| (event->button() == Qt::RightButton)))
		return;

	QPointF clickedPoint = event->localPos();
	if (!viewport().contains(event->pos()))
		return;

	//m_pClickedText->position->setCoords(clickedPoint);

	if (event->button() == Qt::LeftButton)
	{
		int curvePlotSize = m_vGraph.size() - 1;
		double key = xAxis->pixelToCoord(clickedPoint.rx());

		QString temp;

		for (int i = 0; i < curvePlotSize; i++)
		{
			unsigned int tarIndex;
			bool checked = Dichotomy(key, m_vGraph[i].vTime, &tarIndex);
			if(checked == true)
			{
				m_vGraph[i].pClickedTracer->setGraphKey(m_vGraph[i].vTime[tarIndex]);
				m_vGraph[i].pClickedTracer->setVisible(true);

				if (temp.size() <= 0)
				{
					temp.append(QString("Time: %1\n").arg(m_vGraph[i].vTime[tarIndex]));
				}

				temp.append(QString("%1:\t%2\n").arg(m_vGraph[i].pGraph->name())
					.arg(m_vGraph[i].vValue[tarIndex]));

				m_vGraph[i].pLastTracer->setVisible(false);

				m_vGraph[i].bClickedTracerShow = true;
				m_vGraph[i].ClickedTracerIndex = tarIndex;

				replot();
			}
			else
			{
				m_vGraph[i].pClickedTracer->setVisible(false);
				//m_vGraph[i].pClickPointInfo->setVisible(false);
				m_pClickedText->setVisible(false);

				m_vGraph[i].bClickedTracerShow = false;
				m_vGraph[i].ClickedTracerIndex = 0;
			}
		}

		//m_vGraph[0].pClickPointInfo->setVisible(true);
		//m_vGraph[0].pClickPointInfo->setText(temp);

		//m_pClickedText->setVisible(true);
		//m_pClickedText->setText(temp);
		SetClickedInfo(temp, clickedPoint);
	}
	else
	{
		m_RubberOrigin = event->pos();
		m_pRubberBand->setGeometry(QRect(m_RubberOrigin, QSize()));
		m_pRubberBand->show();
	}
}

void ZPlot::MouseReleaseEvent(QMouseEvent* event)
{
	QPoint pos = event->pos();
	if (legend->selectTest(pos, false) >= 0)
	{
		m_pRubberBand->hide();
		replot();
	}
	else if (m_pRubberBand->isVisible())
	{
		const QRect zoomRect = m_pRubberBand->geometry();
		int xp1, yp1, xp2, yp2;
		zoomRect.getCoords(&xp1, &yp1, &xp2, &yp2);
		double x1 = xAxis->pixelToCoord(xp1);
		double x2 = xAxis->pixelToCoord(xp2);
		double y1 = yAxis->pixelToCoord(yp1);
		double y2 = yAxis->pixelToCoord(yp2);

		xAxis->setRange(x1, x2);
		yAxis->setRange(y1, y2);

		m_pRubberBand->hide();
		replot();
	}
}

QCPGraph* ZPlot::AddNormalGraph()
{
	QCPGraph* pGraph = addGraph();
	pGraph->setSelectable(QCP::stDataRange);

	QCPItemTracer* pClickedTracer = new QCPItemTracer(this);
	pClickedTracer->setGraph(pGraph);
	pClickedTracer->setInterpolating(true);
	pClickedTracer->setStyle(QCPItemTracer::tsCircle);
	pClickedTracer->setPen(QPen(Qt::red));
	pClickedTracer->setBrush(Qt::red);
	pClickedTracer->setSize(7);
	pClickedTracer->setVisible(false);
	pClickedTracer->setSelectable(false);

	QCPItemTracer* pLastTracer = new QCPItemTracer(this);
	pLastTracer->setGraph(pGraph);
	pLastTracer->setInterpolating(true);
	pLastTracer->setStyle(QCPItemTracer::tsCircle);
	pLastTracer->setPen(QPen(Qt::green));
	pLastTracer->setBrush(Qt::green);
	pLastTracer->setSize(7);
	pLastTracer->setSelectable(false);

	QCPItemText* pClickPointInfo = new QCPItemText(this);
	pClickPointInfo->position->setType(QCPItemPosition::ptAxisRectRatio);
	pClickPointInfo->setPositionAlignment(Qt::AlignRight | Qt::AlignBottom);
	pClickPointInfo->position->setCoords(0.75, 0.75);
	pClickPointInfo->setText("");
	pClickPointInfo->setTextAlignment(Qt::AlignLeft);
	pClickPointInfo->setPadding(QMargins(8, 0, 0, 0));
	pClickPointInfo->setVisible(false);
	pClickPointInfo->setSelectable(false);

	QPen graphPen;
	//graphPen.setColor(QColor(rand() % 245 + 20, rand() % 245 + 20, rand() % 245 + 20));
	graphPen.setColor(QColor(rand() % 180 + 60, rand() % 180 + 60, rand() % 180 + 60));
	graphPen.setWidth(1);
	pGraph->setPen(graphPen);

	STRU_GRAPH stGraph;
	stGraph.pGraph = pGraph;
	stGraph.pClickedTracer = pClickedTracer;
	stGraph.pLastTracer = pLastTracer;
	stGraph.pClickPointInfo = pClickPointInfo;

	m_vGraph.append(stGraph);

	return pGraph;
}

QCPGraph* ZPlot::AddIndicatorGraph()
{
	QCPGraph* pGraph = addGraph();
	pGraph->setSelectable(QCP::stNone);
	QPen penStyle;
	penStyle.setColor(Qt::red);
	penStyle.setStyle(Qt::DashDotLine);
	penStyle.setWidth(1);
	pGraph->setPen(penStyle);
	pGraph->setVisible(false);
	pGraph->setName("indicator");

	STRU_GRAPH stGraph;

	stGraph.pGraph = pGraph;

	m_vGraph.append(stGraph);

	return pGraph;
}

int ZPlot::LocalGraphIndex(const QCPGraph* pGraph)
{
	int size = m_vGraph.size();
	for (int i = 0; i < size; i++)
	{
		if (pGraph == m_vGraph[i].pGraph)
			return i;
	}

	return -1;
}

void ZPlot::RemoveGraph(int index)
{
	if (index <= (m_vGraph.size() - 1))
	{
		if (m_vGraph[index].pClickedTracer != NULL)
		{ 
			removeItem(m_vGraph[index].pClickedTracer);
			m_vGraph[index].pClickedTracer = NULL;
		}

		if (m_vGraph[index].pClickPointInfo != NULL)
		{
			removeItem(m_vGraph[index].pClickPointInfo);
			m_vGraph[index].pClickPointInfo = NULL;
		}

		removeGraph(index);
		
		m_vGraph.takeAt(index);
	}

	replot();
}

void ZPlot::RemoveGraph(QString strName)
{
	for (int i = 0; i < m_vGraph.size(); i++)
	{
		if (strName == m_vGraph[i].pGraph->name())
		{
			RemoveGraph(i);
			return;
		}
	}

	replot();
}

void ZPlot::RemoveAllGraph()
{
	int size = m_vGraph.size();
	for (int i = 0; i < size; i++)
	{
		if (m_vGraph[i].pLastTracer != NULL)
		{
			delete m_vGraph[i].pLastTracer;
			m_vGraph[i].pLastTracer = NULL;
		}

		if (m_vGraph[i].pClickedTracer != NULL)
		{
			delete m_vGraph[i].pClickedTracer;
			m_vGraph[i].pClickedTracer = NULL;
		}
		
		if (m_vGraph[i].pClickPointInfo != NULL)
		{
			delete m_vGraph[i].pClickPointInfo;
			m_vGraph[i].pClickPointInfo = NULL;
		}
		
		m_vGraph[i].vTime.clear();
		m_vGraph[i].vValue.clear();
	}
	m_vGraph.clear();

	clearGraphs();
}

void ZPlot::SetData(QVector<double> vTime, QVector<double> vValue, QString strGraphName)
{
	if (graphCount() > 0)
		clearGraphs();

	AddNormalGraph();

	AddIndicatorGraph();

	m_vGraph[0].vTime = vTime;
	m_vGraph[0].vValue = vValue;

	m_vGraph[0].pGraph->setData(m_vGraph[0].vTime, m_vGraph[0].vValue);

	m_vGraph[0].pLastTracer->setGraphKey(m_vGraph[0].vTime.last());
}

int ZPlot::AddData(QVector<double> vTime, QVector<double> vValue, QString strGraphName)
{
	int graphcount = graphCount();
	if (graphcount > 0)
		RemoveGraph(graphcount-1);
		
	QCPGraph* pGraph = AddNormalGraph();
	pGraph->setName(strGraphName);
	int graphIndex = LocalGraphIndex(pGraph);
	if (graphIndex == -1)
		return -1;

	m_vGraph[graphIndex].vTime = vTime;
	m_vGraph[graphIndex].vValue = vValue;
	m_vGraph[graphIndex].pGraph->setData(m_vGraph[graphIndex].vTime, m_vGraph[graphIndex].vValue);
	m_vGraph[graphIndex].pLastTracer->setGraphKey(m_vGraph[graphIndex].vTime.last());

	AddIndicatorGraph();

	replot();

	return 0;
}

void ZPlot::RescaleAll()
{
	double time_min = 0;
	double time_max = 0;
	double value_max = 0;
	double value_min = 0;

	for (int i = 0; i < m_vGraph.size() - 1; i++)
	{
		int size = m_vGraph[i].vTime.size();
		for (int j = 0; j < size; j++)
		{
			if (m_vGraph[i].vTime[j] >= time_max)
			{
				time_max = m_vGraph[i].vTime[j];
			}
			else if (m_vGraph[i].vTime[j] <= time_min)
			{
				time_min = m_vGraph[i].vTime[j];
			}

			if (m_vGraph[i].vValue[j] >= value_max)
			{
				value_max = m_vGraph[i].vValue[j];
			}
			else if (m_vGraph[i].vValue[j] <= value_min)
			{
				value_min = m_vGraph[i].vValue[j];
			}
		}
	}

	xAxis->setRange(time_min, time_max);
	yAxis->setRange(value_min, value_max);

	replot();
}

void ZPlot::RescaleTime()
{
	xAxis->rescale();
}

void ZPlot::RescaleValue()
{
	yAxis->rescale();
}

void ZPlot::SetRangeTime(double min, double max)
{
	xAxis->setRange(min, max);
}

void ZPlot::SetRangeValue(double min, double max)
{
	yAxis->setRange(min, max);
}

void ZPlot::SetViewFocus(bool timeFocus, bool valueFocus)
{
	//if(timeFocus)

}

double ZPlot::GetTimeMax(int graphIndex)
{

	return 0;
}

double ZPlot::GetTimeMin(int graphIndex)
{

	return 0;
}

double ZPlot::GetValueMax(int graphIndex)
{

	return 0;
}

double ZPlot::GetValueMin(int graphIndex)
{

	return 0;   
}

bool ZPlot::Dichotomy(double key, QVector<double>& data, unsigned int* pTarIndex)
{
	unsigned int size = data.size();

	if (size <= 0)
		return false;

	unsigned int cur_size = size;
	unsigned int start_index = 0;

	unsigned int start = 0;
	unsigned int end = cur_size - 1;
	unsigned int middle = cur_size / 2;

	if (key < data.first())
		return false;

	if (key > data.last())
		return false;

	while (cur_size > 2)
	{
		if (key <= data[middle])
		{
			end = middle;
			cur_size = cur_size / 2;
			middle = start + cur_size / 2;
		}
		else
		{
			start = middle;
			cur_size = cur_size / 2;
			middle = start + cur_size / 2;
		}
	}

	double dis1 = fabs(key - data.first());
	double dis2 = fabs(key - data[middle]);
	double dis3 = fabs(key - data.last());

	if ((dis1 <= dis2)
		&& (dis1 <= dis3))
	{
		*pTarIndex = start;
		return true;
	}
	else if ((dis2 <= dis1)
		&& (dis2 <= dis3))
	{
		*pTarIndex = middle;
		return true;
	}
	else
	{
		*pTarIndex = end;
		return true;
	}
		
}

void ZPlot::contextMenuRequest(QPoint pos)
{
	QMenu *menu = new QMenu(this);
	menu->setAttribute(Qt::WA_DeleteOnClose);

	if (legend->selectTest(pos, false) >= 0) // context menu on legend requested
	{
		menu->addAction("Move to top left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignLeft));
		menu->addAction("Move to top center", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignHCenter));
		menu->addAction("Move to top right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignTop | Qt::AlignRight));
		menu->addAction("Move to bottom right", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignRight));
		menu->addAction("Move to bottom left", this, SLOT(moveLegend()))->setData((int)(Qt::AlignBottom | Qt::AlignLeft));
	}

	menu->popup(mapToGlobal(pos));
}

void ZPlot::moveLegend()
{
	if (QAction* contextAction = qobject_cast<QAction*>(sender())) // make sure this slot is really called by a context menu action, so it carries the data we need
	{
		bool ok;
		int dataInt = contextAction->data().toInt(&ok);
		if (ok)
		{
			axisRect()->insetLayout()->setInsetAlignment(0, (Qt::Alignment)dataInt);
			replot();
		}
	}
}

void ZPlot::SelectionChanged()
{
	// synchronize selection of graphs with selection of corresponding legend items:
	for (int i = 0; i<graphCount(); ++i)
	{
		QCPGraph *graph = m_vGraph[i].pGraph;
		QCPPlottableLegendItem *item = legend->itemWithPlottable(graph);
		if (item->selected() || graph->selected())
		{
			item->setSelected(true);
			graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
		}
	}
}

void ZPlot::keyPressEvent(QKeyEvent *event)
{
	QString temp;
	if (event->key() == Qt::Key_Left)
	{
		for (int i = 0; i < m_vGraph.size()-1; i++)
		{
			if (m_vGraph[i].bClickedTracerShow)
			{
				if (m_vGraph[i].ClickedTracerIndex == 0)
				{
					m_vGraph[i].pClickedTracer->setVisible(false);
					//m_vGraph[i].pClickPointInfo->setVisible(false);

					if(m_pClickedText != NULL)
						m_pClickedText->setVisible(false);

					m_vGraph[i].bClickedTracerShow = false;
					m_vGraph[i].ClickedTracerIndex = 0;

					//m_vGraph[0].pClickPointInfo->setText(temp);
					if (m_pClickedText != NULL)
						m_pClickedText->setText(temp);
				}
				else
				{
					m_vGraph[i].ClickedTracerIndex--;
					m_vGraph[i].pClickedTracer->setGraphKey(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex]);
					m_vGraph[i].pClickedTracer->setVisible(true);

					if (temp.size() <= 0)
					{
						temp.append(QString("Time: %1\n").arg(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex]));
					}

					temp.append(QString("%1:\t%2\n").arg(m_vGraph[i].pGraph->name())
						.arg(m_vGraph[i].vValue[m_vGraph[i].ClickedTracerIndex]));

					//temp.append(QString("time:%1\tvalue:%2\n").arg(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex])
					//	.arg(m_vGraph[i].vValue[m_vGraph[i].ClickedTracerIndex]));

					m_vGraph[i].pLastTracer->setVisible(false);

					m_vGraph[i].bClickedTracerShow = true;

					//m_vGraph[0].pClickPointInfo->setVisible(true);
					//m_vGraph[0].pClickPointInfo->setText(temp);
					if (m_pClickedText != NULL)
					{
						m_pClickedText->setVisible(true);
						m_pClickedText->setText(temp);
					}

					xAxis->setRange(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex],
									xAxis->range().upper - xAxis->range().lower,
									Qt::AlignVCenter);
				}
			}
		}
	}
	else if (event->key() == Qt::Key_Right)
	{
		for (int i = 0; i < m_vGraph.size() -1; i++)
		{
			if (m_vGraph[i].bClickedTracerShow)
			{
				if (m_vGraph[i].ClickedTracerIndex >= (m_vGraph[i].vTime.size() - 1))
				{
					m_vGraph[i].pClickedTracer->setVisible(false);
					//m_vGraph[i].pClickPointInfo->setVisible(false);
					if (m_pClickedText != NULL)
						m_pClickedText->setVisible(false);

					m_vGraph[i].bClickedTracerShow = false;
					m_vGraph[i].ClickedTracerIndex = m_vGraph[i].ClickedTracerIndex;
				}
				else
				{
					m_vGraph[i].ClickedTracerIndex++;
					m_vGraph[i].pClickedTracer->setGraphKey(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex]);
					m_vGraph[i].pClickedTracer->setVisible(true);

					if (temp.size() <= 0)
					{
						temp.append(QString("Time: %1\n").arg(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex]));
					}

					temp.append(QString("%1:\t%2\n").arg(m_vGraph[i].pGraph->name())
						.arg(m_vGraph[i].vValue[m_vGraph[i].ClickedTracerIndex]));

					//temp.append(QString("time:%1\tvalue:%2\n").arg(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex])
					//	.arg(m_vGraph[i].vValue[m_vGraph[i].ClickedTracerIndex]));

					m_vGraph[i].pLastTracer->setVisible(false);

					m_vGraph[i].bClickedTracerShow = true;

					//m_vGraph[0].pClickPointInfo->setVisible(true);
					//m_vGraph[0].pClickPointInfo->setText(temp);
					if (m_pClickedText != NULL)
					{
						m_pClickedText->setVisible(true);
						m_pClickedText->setText(temp);
					}

					xAxis->setRange(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex],
									xAxis->range().upper - xAxis->range().lower,
									Qt::AlignVCenter);
				}
			}
		}
	}

	replot();
}

void ZPlot::SetClickedInfo(QString strInfo, QPointF point)
{
	if (m_pClickedText == NULL)
	{
		m_pClickedText = new QCPItemText(this);
		m_pClickedText->position->setType(QCPItemPosition::ptAbsolute);
		m_pClickedText->setPositionAlignment(Qt::AlignRight | Qt::AlignBottom);
		m_pClickedText->setTextAlignment(Qt::AlignLeft);
		m_pClickedText->setPadding(QMargins(8, 0, 0, 0));
		m_pClickedText->setSelectable(false);
	}

	if (m_enStyle == EN_DARK)
	{
		m_pClickedText->setColor(QColor(255, 255, 255));
	}
	

	m_pClickedText->position->setCoords(point);
	m_pClickedText->setText(strInfo);
	m_pClickedText->setVisible(true);

	replot();
}

void ZPlot::SetStyle(EN_STYLE enStyle)
{
	m_enStyle = enStyle;

	if (enStyle == EN_DARK)
	{
		QLinearGradient plotGradient;
		plotGradient.setStart(0, 0);
		plotGradient.setFinalStop(0, 350);
		plotGradient.setColorAt(0, QColor(80, 80, 80));
		plotGradient.setColorAt(1, QColor(50, 50, 50));

		xAxis->setBasePen(QPen(Qt::white, 1));
		yAxis->setBasePen(QPen(Qt::white, 1));
		xAxis->setTickPen(QPen(Qt::white, 1));
		yAxis->setTickPen(QPen(Qt::white, 1));
		xAxis->setSubTickPen(QPen(Qt::white, 1));
		yAxis->setSubTickPen(QPen(Qt::white, 1));
		xAxis->setTickLabelColor(Qt::white);
		yAxis->setTickLabelColor(Qt::white);
		xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
		yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
		xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
		yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
		xAxis->grid()->setSubGridVisible(true);
		yAxis->grid()->setSubGridVisible(true);
		xAxis->grid()->setZeroLinePen(Qt::NoPen);
		yAxis->grid()->setZeroLinePen(Qt::NoPen);
		xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
		yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);

		xAxis->setLabelColor(QColor(255, 255, 255));
		yAxis->setLabelColor(QColor(255, 255, 255));

		setBackground(plotGradient);
	}
	else if (enStyle == EN_DEFAULT)
	{

	}

}

EN_STYLE ZPlot::GetStyle()
{
	return m_enStyle;
}

void ZPlot::GotoKey(double key)
{
	unsigned int tarIndex = 0;
	QString temp;
	int curvePlotSize = m_vGraph.size() - 1;

	for (int i = 0; i < curvePlotSize; i++)
	{
		bool checked = Dichotomy(key, m_vGraph[i].vTime, &tarIndex);
		if (checked == true)
		{
			m_vGraph[i].pClickedTracer->setGraphKey(m_vGraph[i].vTime[tarIndex]);
			m_vGraph[i].pClickedTracer->setVisible(true);

			temp.append(QString("time:%1\tvalue:%2\n").arg(m_vGraph[i].vTime[tarIndex])
				.arg(m_vGraph[i].vValue[tarIndex]));

			m_vGraph[i].pLastTracer->setVisible(false);

			m_vGraph[i].bClickedTracerShow = true;
			m_vGraph[i].ClickedTracerIndex = tarIndex;

			QPointF clickedPoint(100, 100);
			SetClickedInfo(temp, clickedPoint);

			//xAxis->setRange(m_lastTime, m_stSetting.XTraceWidth, Qt::AlignVCenter);
			xAxis->setRange(m_vGraph[i].vTime[m_vGraph[i].ClickedTracerIndex],
							xAxis->range().upper - xAxis->range().lower,
							Qt::AlignVCenter);

			replot();
		}
	}
}

void ZPlot::Save2Png(QString strPath)
{
	legend->setVisible(false);
	if (m_pClickedText != NULL)
		m_pClickedText->setVisible(false);
	m_vGraph[m_vGraph.size() - 1].pGraph->setVisible(false);
	for (int i = 0; i < m_vGraph.size() - 1; i++)
		m_vGraph[i].pClickedTracer->setVisible(false);

	savePng(strPath);

	legend->setVisible(true);
}