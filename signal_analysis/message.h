#pragma once

#include "signalfloat.h"
#include "signalint.h"
#include "signaluint.h"
#include <QVector>

class CMessage
{
public:
	CMessage();
	void OpenBinFile(QString strFilePath);

	void AddSignal(CSignal* pSignal);
	void SaveFile(QString strPath);

	void Analysis();

	unsigned int GetLength();

	QVector<QString> GetSignalNameList();

	QVector<double>* GetValue(unsigned int index);

	double GetMax();
	double GetMin();
private:
	QVector<CSignal*> m_vSignal;
	QByteArray m_binBytes;

	QVector<QString> m_signalNameList;

	unsigned int m_length;
};