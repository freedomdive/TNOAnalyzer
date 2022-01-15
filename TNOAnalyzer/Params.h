#pragma once
#include <string>
#include <windows.h>
#include "ErlLogger.h"
using namespace std;

class Params
{
public:
	Params();
	
	static Params inst;
	
	struct Ini
	{
		string sLogDir;
		string sResults1;
		string sResults2;
		string sData1;
		string sData2;

		string sBinaryData1;
		string sBinaryData2;
		
		string sName1;
		string sName2;
		
		int nSkipStrings1;
		int nSkipStrings2;

		bool bOnlyParseDataToBinary1;
		
		bool bOnlyParseDataToBinary2;

		string sPrefixIn;
		string sPrefixOut1;
		string sPrefixOut2;

		string sResultsFileHeader1;
		string sResultsFileHeader2;
		
		
		string sPhysParamsFileName;

		int nThreadsCount;
		bool bFullMemory;

		int nPriority;
	};

	struct Phys
	{
		double fDiameter;
		double fDencity;
		bool bFirst;
	};
	static Ini& GetIni() { return inst.m_ini; }
	static Phys& GetPhys() { return inst.m_phys; }
	
protected:
	Ini m_ini{};
	Phys m_phys{};
	
	void ReadIni();
};