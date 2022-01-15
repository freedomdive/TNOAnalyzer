#include "Params.h"
#include "FilesInteraction.h"
#include <fstream>
#include <iostream>

using namespace std;

extern void SaveLog(string& str);

extern void SaveLog(string str);

extern void SaveLog(char* str);

extern void SaveLog(const char* str);

FilesInteraction::FilesInteraction(bool bBinaryPath1Set, bool bBinaryPath2Set, int nElementsCount)
{
	//���� ������� ����������� ����� � ������, - ����� ��� ������ ������ ��� ������ �������� �� ���� �� ������� ������, ����� �� ���������� �������
	vData2.resize(Params::GetIni().bOnlyParseDataToBinary1 == false && Params::GetIni().bOnlyParseDataToBinary2 == false && Params::GetIni().bFullMemory ? nElementsCount : Params::GetIni().nThreadsCount);
	//������ ������� ���� ������ �� ���������� �������
	vData1.resize(Params::GetIni().nThreadsCount);
	
	for (auto& it : vData1)
	{
		it = new Data[nDataSize];
		
	}

	for (auto& it : vData2)
	{
		it = new Data[nDataSize];
	}
	
	this->bBinaryPath1Set = bBinaryPath1Set;
	this->bBinaryPath2Set = bBinaryPath2Set;
}

FilesInteraction::~FilesInteraction()
{
	for (auto& it : vData1)
	{
		delete[] it;
	}

	for (auto& it : vData2)
	{
		delete[] it;
	}
}

//���������� ���� �������� ������
Data FilesInteraction::ParseLine(fstream& f) const
{
	Data pData{};

	char buf[24];

	char fakebuf[24 * 3];
	
	char fakebuf2[24 * 10 + 1];
	
	f.read(buf, sizeof(buf));

	pData.t = atof(buf);

	f.read(buf, sizeof(buf));

	pData.a = atof(buf);

	f.read(buf, sizeof(buf));

	pData.e = atof(buf);

	f.read(buf, sizeof(buf));

	pData.incl = atof(buf);

	f.read(buf, sizeof(buf));

	pData.node = atof(buf);

	f.read(buf, sizeof(buf));

	pData.per = atof(buf);

	f.read(fakebuf, sizeof(fakebuf));

	f.read(buf, sizeof(buf));

	pData.r = atof(buf);

	f.read(buf, sizeof(buf));

	pData.x = atof(buf);

	f.read(buf, sizeof(buf));

	pData.y = atof(buf);

	f.read(buf, sizeof(buf));

	pData.z = atof(buf);

	f.read(buf, sizeof(buf));

	pData.vx = atof(buf);

	f.read(buf, sizeof(buf));

	pData.vy = atof(buf);

	f.read(buf, sizeof(buf));

	pData.vz = atof(buf);

	f.read(fakebuf2, sizeof(fakebuf2));
	
	/*string s;

	getline(f, s);

	vector<string> vRes;
	
	boost::split(vRes, s, boost::is_any_of(" "), boost::token_compress_on);

	pData.t = stod(vRes[1]);
	pData.a = stod(vRes[2]);
	pData.e = stod(vRes[3]);
	pData.incl = stod(vRes[4]);
	pData.node = stod(vRes[5]);
	pData.per = stod(vRes[6]);
	pData.r = stod(vRes[10]);
	pData.x = stod(vRes[11]);
	pData.y = stod(vRes[12]);
	pData.z = stod(vRes[13]);
	pData.vx = stod(vRes[14]);
	pData.vy = stod(vRes[15]);
	pData.vz = stod(vRes[16]);*/
	
	return pData;
}

void FilesInteraction::ReadBinaryData(filesystem::path& pPath, Data* vData, string& sBinaryData) const
{

	const string sPath = sBinaryData + "\\" + pPath.stem().string() + ".bin";
	
	SaveLog("Start Reading Binary Data " + sPath);
	
	ifstream in(sPath, ios::binary);

	in.read(reinterpret_cast<char*>(vData), nBinaryFileSize);

	in.close();

	SaveLog("End Reading Binary Data " + sPath);
}

void FilesInteraction::WriteBinaryData(filesystem::path& pPath, Data * vData, string& sBinaryData) const
{
	const string sPath = sBinaryData + "\\" + pPath.stem().string() + ".bin";

	SaveLog("Start Writing Binary Data " + sPath);
	
	ofstream out(sPath, ios::binary);

	out.write(reinterpret_cast<char*>(vData), nBinaryFileSize);

	out.close();

	SaveLog("End Writing Binary Data " + sPath);

}

void FilesInteraction::ParseData(filesystem::path& pPath, Data* vData, int nSkipStrings) const
{
	SaveLog("Start Reading Text Data " + pPath.string());
	
	fstream f(pPath.string(), ios::in);

	string s;

	for (int i = 0; i < nSkipStrings; i++)
	{
		getline(f, s);
	}
	
	for (int i = 0; i < nDataSize; i++)
	{
		auto pData = ParseLine(f);

		vData[i] = pData;
	}

	f.close();

	SaveLog("End Reading Text Data " + pPath.string());
}

void FilesInteraction::ReadFiles(int iIndex1, int iIndex2, filesystem::path& pPath1, filesystem::path& pPath2, fstream& f1, fstream& f2, atomic_bool& bBinaryState1, atomic_bool& bBinaryState2, int iThreadNumber)
{

	const auto pData1 = vData1[iThreadNumber];
	//���� ����������� ����� ������� ���� ������ � ������, ������ ������ ������ ������������� ������� ������ � ������, ����� ������� �������� ������
	const auto pData2 = vData2[Params::GetIni().bFullMemory ? iIndex2 : iThreadNumber];

	
	if (bBinaryState1 == false)
	{
		if (iIndex1 != iPrevIndex[iThreadNumber])
		{
			ParseData(pPath1, pData1, Params::GetIni().nSkipStrings1);

			iPrevIndex[iThreadNumber] = iIndex1;
		}
	}
	if (Params::GetIni().bFullMemory == false && bBinaryState2 == false)
	{
		ParseData(pPath2, pData2, Params::GetIni().nSkipStrings2);
	}

	bool bDontRead1 = false;
	bool bDontRead2 = false;
	
	if (bBinaryState1 == false && bBinaryPath1Set == true)
	{
		WriteBinaryData(pPath1, pData1, Params::GetIni().sBinaryData1);

		bBinaryState1 = true;

		bDontRead1 = true;

	}
	
	if (bBinaryState2 == false && bBinaryPath2Set == true)
	{
		WriteBinaryData(pPath2, pData2, Params::GetIni().sBinaryData2);

		bBinaryState2 = true;

		bDontRead2 = true;

	}

	
	
	if (bBinaryState1 == true && bDontRead1 == false)
	{
		if (iIndex1 != iPrevIndex[iThreadNumber])
		{
			ReadBinaryData(pPath1, pData1, Params::GetIni().sBinaryData1);
			iPrevIndex[iThreadNumber] = iIndex1;
		}
	}
	if (Params::GetIni().bFullMemory == false && bBinaryState2 == true && bDontRead2 == false)
	{
		ReadBinaryData(pPath2, pData2, Params::GetIni().sBinaryData2);
	}

	const auto iMinIndex1 = Calc::FindFirstExtremum(pData1, pData2, nDataSize);
	
	const auto iMinIndex2 = Calc::FindSecondExtremum(pData1, pData2, nDataSize);

	
	const auto pResultDeltaR = Calc::AnalyzeFirstItem(pData1[iMinIndex1], pData2[iMinIndex1]);
	
	const auto pResultRho = Calc::AnalyzeSecondItem(pData1[iMinIndex2], pData2[iMinIndex2]);

	const string sPostfix = pPath1.filename().string().substr(Params::GetIni().sPrefixIn.length(), pPath1.filename().string().length() - 4 - Params::GetIni().sPrefixIn.length()) + "-" + pPath2.filename().string().substr(Params::GetIni().sPrefixIn.length(), pPath2.filename().string().length() - 4 - Params::GetIni().sPrefixIn.length());
	
	WriteResults(pData1, iMinIndex1, iMinIndex2, pResultDeltaR, pResultRho, sPostfix, f1, f2);


	

	SaveLog("End Writing Results");
}

void FilesInteraction::WriteResults(Data* pData, int iMinIndex1, int iMinIndex2, const DeltaR& pResultDeltaR, const Rho& pResultRho, const string& sPostfix, fstream& f1, fstream& f2)
{
	mWriteResults.lock();
	
	f1 << pData[iMinIndex1].t << " " << pResultDeltaR.fDeltaR << " " << pResultDeltaR.fDeltaV << " " << pResultDeltaR.fR_H << " " << pResultDeltaR.fV2 << " " << pResultDeltaR.fsigmaR_H << " " << pResultDeltaR.fsigmaV2 << " " << sPostfix <<  endl;

	f2 << pData[iMinIndex2].t << " " << pResultRho.fMetric2 << " " << pResultRho.pDeltaR.fDeltaR << " " << pResultRho.pDeltaR.fDeltaV << " " << pResultRho.pDeltaR.fR_H << " " << pResultRho.pDeltaR.fV2 << " " << pResultRho.pDeltaR.fsigmaR_H << " " << pResultRho.pDeltaR.fsigmaV2 << " " << sPostfix << endl;

	mWriteResults.unlock();
}