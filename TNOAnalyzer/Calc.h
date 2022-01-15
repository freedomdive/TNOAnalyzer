#pragma once
#include <vector>
#include <string>
using namespace std;

struct Data
{
	double t;
	double a;
	double e;
	double incl;
	double node;
	double per;
	double r;
	double x;
	double y;
	double z;
	double vx;
	double vy;
	double vz;
};

struct DeltaR
{
	double fDeltaR;
	double fDeltaV;

	double fR_H;
	double fV2;

	double fsigmaR_H;
	double fsigmaV2;
};

struct Rho
{
	double fMetric2;

	DeltaR pDeltaR;
};

const double AE_m = 149597870700.0;
const double AE_km = 149597870.7;

const double seconds_per_day = 86400;

const double AE_days_to_m_s = AE_m / seconds_per_day;

const double PI = 3.141592653589793238462643;

const double Ms_kg = 1.989E30;

const double Km_to_m = 1000;

const double G = 6.67408E-11;

const double Grad_to_Rad = PI / 180.0;

class Calc
{
	
	
public:
	static DeltaR AnalyzeFirstItem(Data& pData1, Data& pData2);
	
	static Rho AnalyzeSecondItem(Data& pData1, Data& pData2);

	static double FindFirstFunction(Data& pData1, Data& pData2);

	static double FindSecondPreFunction(Data& pData1, Data& pData2, double& h1, double& h2, double& h3);
	
	static double FindSecondFunction(Data& pData1, Data& pData2, double& h1, double& h2, double& h3);

	static int FindFirstExtremum(Data* vData1, Data* vData2, int nDataSize);
	
	static int FindSecondExtremum(Data* vData1, Data* vData2, int nDataSize);
};