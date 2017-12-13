#pragma once

#include "qcustomplot.h"
#include <QVector>


typedef struct
{
	QCPGraph* pGraph;
	QCPItemText* pClickPointInfo;
	QCPItemTracer* pClickedTracer;
	QCPItemTracer* pLastTracer;
	bool bClickedTracerShow;
	unsigned int ClickedTracerIndex;
	QVector<double> vTime;
	QVector<double> vValue;
}STRU_GRAPH;

typedef enum
{
	EN_DEFAULT = 0,
	EN_DARK
}EN_STYLE;

class ZPlot : public QCustomPlot
{
	Q_OBJECT

public:
	ZPlot(QWidget* parent = 0);
	~ZPlot();

	void SetTitle(QString strTitle);

	void SetData(QVector<double> vTime, QVector<double> vValue, QString strGraphName);
	int AddData(QVector<double> vTime, QVector<double> vValue, QString strGraphName);

	void RescaleAll();
	void RescaleTime();
	void RescaleValue();
	void SetRangeTime(double min, double max);
	void SetRangeValue(double min, double max);

	void SetViewFocus(bool timeFocus, bool valueFocus);

	double GetTimeMax(int graphIndex);
	double GetTimeMin(int graphIndex);
	double GetValueMax(int graphIndex);
	double GetValueMin(int graphIndex);

	void SetStyle(EN_STYLE enStyle);
	EN_STYLE GetStyle();

	void GotoKey(double key);

	void test();

	void Save2Png(QString strPath);

	void RemoveGraph(int index);
	void RemoveGraph(QString strName);

private slots:
	void contextMenuRequest(QPoint pos);
	void MouseMoveEvent(QMouseEvent* event);
	void MousePressEvent(QMouseEvent* event);
	void MouseReleaseEvent(QMouseEvent* event);
	void moveLegend();
	void SelectionChanged();

private:
	QCPGraph* AddNormalGraph();
	QCPGraph* AddIndicatorGraph();
	
	
	void RemoveAllGraph();

	int LocalGraphIndex(const QCPGraph* pGraph);
	bool Dichotomy(double key, QVector<double>& data, unsigned int* pTarIndex = 0);
	void SetupVerticalLine(double x, double y_length, bool visibale);
	void keyPressEvent(QKeyEvent *event);
	void SetClickedInfo(QString strInfo, QPointF point);
private:
	QVector<STRU_GRAPH> m_vGraph;
	QVector<double> m_vTimeValue;

	QRubberBand* m_pRubberBand;
	QPoint m_RubberOrigin;

	QCPTextElement* m_pTitle;

	EN_STYLE m_enStyle;

	QCPItemText* m_pClickedText;
};

