#include "message.h"
#include <QFileDialog>
#include <QFile>
#include <string.h>
#include <QTextStream>

CMessage::CMessage()
{
	m_length = 0;
}

void CMessage::OpenBinFile(QString strFilePath)
{
	QFile tmpfile(strFilePath);
	tmpfile.open(QIODevice::ReadOnly);

	m_binBytes = tmpfile.readAll();

	tmpfile.close();
}

void CMessage::AddSignal(CSignal* pSignal)
{
	if (pSignal == NULL)
		return;

	m_length += pSignal->GetLength();

	m_vSignal << pSignal;

	m_signalNameList << pSignal->GetName();
}

QVector<QString> CMessage::GetSignalNameList()
{
	return m_signalNameList;
}

void CMessage::Analysis()
{
	char* pBin = m_binBytes.data();
	int length = m_binBytes.size();

	unsigned char* pTemp = (unsigned char*)pBin;

	bool ret = false;
	while (ret == false)
	{
		for (int i = 0; i < m_vSignal.size(); i++)
		{
			m_vSignal[i]->InputData(pTemp);
			length -= m_vSignal[i]->GetLength();
			if (length <= 0)
			{
				ret = true;
				break;
			}
		}
	}
}

void CMessage::SaveFile(QString strPath)
{
	char* pBin = m_binBytes.data();
	int length = m_binBytes.size();

	unsigned char* pTemp = (unsigned char*)pBin;

	bool ret = false;
	while (ret == false)
	{
		for (int i = 0; i < m_vSignal.size(); i++)
		{
			m_vSignal[i]->InputData(pTemp);
			length -= m_vSignal[i]->GetLength();
			if (length <= 0)
			{
				ret = true; 
				break;
			}	
		}
	}

	QFile file(strPath);
	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream ts(&file);
		QString strHead;
		for (int i = 0; i < m_vSignal.size(); i++)
		{
			strHead += QString("%1\t").arg(m_vSignal[i]->GetName());
		}

		ts << strHead << endl;

		QString strInfo;
		for (int j = 0; j < m_vSignal[0]->GetDataSize(); j++)
		{
			strInfo = "";
			for (int i = 0; i < m_vSignal.size(); i++)
			{
				if (m_vSignal[i]->GetType() == EN_FLOAT)
				{
					strInfo += QString("%1\t").arg(((CSignalFloat*)m_vSignal[i])->m_vData[j]);
				}
				else if (m_vSignal[i]->GetType() == EN_INT)
				{
					strInfo += QString("%1\t").arg(((CSignalInt*)m_vSignal[i])->m_vData[j]);
				}
				else if (m_vSignal[i]->GetType() == EN_UINT)
				{
					strInfo += QString("%1\t").arg(((CSignalUInt*)m_vSignal[i])->m_vData[j]);
				}
			}

			ts << strInfo << endl;
		}
		
	}
}

unsigned int CMessage::GetLength()
{
	return m_length;
}

QVector<double>* CMessage::GetValue(unsigned int index)
{
	if (m_vSignal[index]->GetType() == EN_FLOAT)
	{
		return &(((CSignalFloat*)m_vSignal[index])->m_vData);
	}
	else if (m_vSignal[index]->GetType() == EN_UINT)
	{
		return &(((CSignalUInt*)m_vSignal[index])->m_vDataf);
	}
	else if (m_vSignal[index]->GetType() == EN_INT)
	{
		return &(((CSignalInt*)m_vSignal[index])->m_vDataf);
	}
}

double CMessage::GetMax()
{
	double max = 0;
	for (int i = 0; i < m_vSignal.size(); i++)
	{
		if (m_vSignal[i]->max >= max)
			max = m_vSignal[i]->max;
	}

	return max;
}

double CMessage::GetMin()
{
	double min = 0;
	for (int i = 0; i < m_vSignal.size(); i++)
	{
		if (m_vSignal[i]->min <= min)
			min = m_vSignal[i]->min;
	}

	return min;
}