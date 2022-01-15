#include "Params.h"
#include "Calc.h"
#include <fstream>
#include <iostream>

extern void SaveLog(string& str);

extern void SaveLog(string str);

extern void SaveLog(char* str);

extern void SaveLog(const char* str);


DeltaR Calc::AnalyzeFirstItem(Data& pData1, Data& pData2)
{
	//1:
	DeltaR pDeltaR = {};
	
	pDeltaR.fDeltaR = sqrt((pData1.x - pData2.x) * (pData1.x - pData2.x) + (pData1.y - pData2.y) * (pData1.y - pData2.y) + (pData1.z - pData2.z) * (pData1.z - pData2.z));
	pDeltaR.fDeltaV = sqrt((pData1.vx - pData2.vx) * (pData1.vx - pData2.vx) + (pData1.vy - pData2.vy) * (pData1.vy - pData2.vy) + (pData1.vz - pData2.vz) * (pData1.vz - pData2.vz));

	pDeltaR.fDeltaR *= AE_km;
	pDeltaR.fDeltaV *= AE_days_to_m_s;

	const double D = Params::GetPhys().fDiameter;

	const double p = Params::GetPhys().fDencity;
	
	pDeltaR.fR_H = (Params::GetPhys().bFirst ? pData1.r : pData2.r) * D * pow(p, 1.0 / 3) * 0.5 * pow(4 * PI / (9 * Ms_kg), 1.0 / 3) * AE_km;

	const double rRel = pDeltaR.fDeltaR * Km_to_m;

	pDeltaR.fV2 = 1 / sqrt(rRel) * sqrt(p * D * D * D) * sqrt(PI * G / 3);

	pDeltaR.fsigmaR_H = pDeltaR.fDeltaR / pDeltaR.fR_H;
	pDeltaR.fsigmaV2 = pDeltaR.fDeltaV / pDeltaR.fV2;


	return pDeltaR;
}

Rho Calc::AnalyzeSecondItem(Data& pData1, Data& pData2)
{
	Rho pRho = {};

	pRho.pDeltaR = AnalyzeFirstItem(pData1, pData2);

	double h1, h2, h3;
	
	FindSecondPreFunction(pData1, pData2, h1, h2, h3);
	pRho.fMetric2 = sqrt(FindSecondFunction(pData1, pData2, h1, h2, h3));
	
	return pRho;
}

//Поиск первой функции для нахождения экстремума
double Calc::FindFirstFunction(Data& pData1, Data& pData2)
{
	const double fResult = (pData1.x - pData2.x) * (pData1.x - pData2.x) + (pData1.y - pData2.y) * (pData1.y - pData2.y) + (pData1.z - pData2.z) * (pData1.z - pData2.z);

	return fResult;
}


//Предварительный поиск второй функции для нахождения экстремума. Использовать в паре с FindSecondFunction
double Calc::FindSecondPreFunction(Data& pData1, Data& pData2, double& h1, double& h2, double& h3)
{
	const double e1 = pData1.e * pData1.e;
	const double e2 = pData2.e * pData2.e;

	const double e12 = pData1.e * pData2.e;

	const double p1 = pData1.a * (1 - e1);
	const double p2 = pData2.a * (1 - e2);

	h1 = (1 + e1) * p1 + (1 + e2) * p2;

	h2 = -2 * sqrt(p1 * p2);

	h3 = e12;

	return h1 + h2 * (1 + h3);
}

//Поиск второй функции для нахождения экстремума
double Calc::FindSecondFunction(Data& pData1, Data& pData2, double& h1, double& h2, double& h3)
{
	const double fIncl1 = pData1.incl * Grad_to_Rad;
	const double fIncl2 = pData2.incl * Grad_to_Rad;

	const double fPer1 = pData1.per * Grad_to_Rad;
	const double fPer2 = pData2.per * Grad_to_Rad;
	
	const double s1 = sin(fIncl1);
	const double s2 = sin(fIncl2);

	const double c1 = cos(fIncl1);
	const double c2 = cos(fIncl2);

	const double sin_o1 = sin(fPer1);
	const double sin_o2 = sin(fPer2);

	const double cos_o1 = cos(fPer1);
	const double cos_o2 = cos(fPer2);

	const double fNodeDiff = (pData1.node - pData2.node) * Grad_to_Rad;
	
	const double sin_node = sin(fNodeDiff);
	const double cos_node = cos(fNodeDiff);

	
	const double cosI = c1 * c2 + s1 * s2 * cos_node;

	const double cosP = s1 * s2 * sin_o1 * sin_o2 + cos_node * (cos_o1 * cos_o2 + c1 * c2 * sin_o1 * sin_o2) + (c2 * cos_o1 * sin_o2 - c1 * sin_o1 * cos_o2) * sin_node;

	
	return h1 + h2 * (cosI + h3 * cosP);
}


int Calc::FindFirstExtremum(Data* vData1, Data* vData2, int nDataSize)
{
	//SaveLog("Start Finding First Extremum");

	double fMinResult = FindFirstFunction(vData1[0], vData2[0]);

	int iMinIndex = 0;

	for (int i = 1; i < nDataSize; i++)
	{
		double fResult = FindFirstFunction(vData1[i], vData2[i]);

		if (fResult < fMinResult)
		{
			fMinResult = fResult;

			iMinIndex = i;
		}
	}


	//SaveLog("Found min Value = " + to_string(fMinResult) + " at index = " + to_string(iMinIndex));

	return iMinIndex;
}

int Calc::FindSecondExtremum(Data* vData1, Data* vData2, int nDataSize)
{
	SaveLog("Start Finding Second Extremum");

	double h1, h2, h3;
	
	FindSecondPreFunction(vData1[0], vData2[0], h1, h2, h3);
	
	double fMinResult = FindSecondFunction(vData1[0], vData2[0], h1, h2, h3);

	int iMinIndex = 0;
	
	for (int i = 1; i < nDataSize; i++)
	{
		const double fPreResult = FindSecondPreFunction(vData1[i], vData2[i], h1, h2, h3);

		if (fPreResult > fMinResult)
		{
			continue;
		}

		const double fResult = FindSecondFunction(vData1[i], vData2[i], h1, h2, h3);
		
		if (fResult < fMinResult)
		{
			fMinResult = fResult;

			iMinIndex = i;
		}
	}
	
	SaveLog("Found min Value = " + to_string(fMinResult) + " at index = " + to_string(iMinIndex));

	return iMinIndex;
}
