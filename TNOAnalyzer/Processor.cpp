#include "Processor.h"
#include "Params.h"
#include "FilesInteraction.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <thread>
using namespace std;

extern void SaveLog(string& str);

extern void SaveLog(string str);

extern void SaveLog(char* str);

extern void SaveLog(const char* str);


void Processor::ReadDirectories()
{
	auto t = clock();
	
	filesystem::path pPath1(Params::GetIni().sData1);
	filesystem::path pPath2(Params::GetIni().sData2);
	


	filesystem::path pPhysParamsFileName(Params::GetIni().sPhysParamsFileName);

	if (filesystem::exists(pPhysParamsFileName) == false)
	{
		cout << "No phys params filename " + Params::GetIni().sPhysParamsFileName << endl;
		SaveLog("No phys params filename " + Params::GetIni().sPhysParamsFileName);
		return;
	}
	else
	{
		if (GetPhysParams(Params::GetIni().sPhysParamsFileName) == false)
		{
			cout << "No phys params of objects (check " + Params::GetIni().sPhysParamsFileName + ")";
			SaveLog("No phys params of objects (check " + Params::GetIni().sPhysParamsFileName + ")");
			return;
		}
	}

	
	
	filesystem::path pBinaryPath1(Params::GetIni().sBinaryData1);
	filesystem::path pBinaryPath2(Params::GetIni().sBinaryData2);

	auto bBinaryPath1Set = Params::GetIni().sBinaryData1 != "?";
	auto bBinaryPath2Set = Params::GetIni().sBinaryData2 != "?";
	
	if (bBinaryPath1Set && filesystem::exists(pBinaryPath1) == false)
	{
		filesystem::create_directories(pBinaryPath1);
	}
	if (bBinaryPath2Set && filesystem::exists(pBinaryPath2) == false)
	{
		filesystem::create_directories(pBinaryPath2);
	}
	
	filesystem::path pResults1(Params::GetIni().sResults1);

	if (filesystem::exists(pResults1) == false)
	{
		filesystem::create_directories(pResults1);
	}
	else
	{
		if (filesystem::exists(Params::GetIni().sResults1 + "\\" + Params::GetIni().sPrefixOut1 + Params::GetIni().sName1 + "_" + Params::GetIni().sName2 + ".txt"))
		{
			filesystem::remove(Params::GetIni().sResults1 + "\\" + Params::GetIni().sPrefixOut1 + Params::GetIni().sName1 + "_" + Params::GetIni().sName2 + ".txt");

			fstream f(Params::GetIni().sResults1 + "\\" + Params::GetIni().sPrefixOut1 + Params::GetIni().sName1 + "_" + Params::GetIni().sName2 + ".txt", ios::out);

			f << Params::GetIni().sResultsFileHeader1 << endl;

			f.close();
		}
	}

	filesystem::path pResults2(Params::GetIni().sResults2);

	if (filesystem::exists(pResults2) == false)
	{
		filesystem::create_directories(pResults2);
	}
	else
	{
		if (filesystem::exists(Params::GetIni().sResults2 + "\\" + Params::GetIni().sPrefixOut2 + Params::GetIni().sName1 + "_" + Params::GetIni().sName2 + ".txt"))
		{
			filesystem::remove(Params::GetIni().sResults2 + "\\" + Params::GetIni().sPrefixOut2 + Params::GetIni().sName1 + "_" + Params::GetIni().sName2 + ".txt");

			fstream f(Params::GetIni().sResults2 + "\\" + Params::GetIni().sPrefixOut2 + Params::GetIni().sName1 + "_" + Params::GetIni().sName2 + ".txt", ios::out);

			f << Params::GetIni().sResultsFileHeader2 << endl;

			f.close();
		}
	}

	
	if (Params::GetIni().bOnlyParseDataToBinary1 == true)
	{
		//≈сли задан режим только парсинга текстовых данных в бинарные, а сами пути дл€ бинарных данных не заданы в параметрах, то ничего не надо делать
		if (bBinaryPath1Set == false)
		{
			SaveLog("No BinaryData1 Param, but set OnlyParseDataToBinary1");
			return;
		}
		//“акже могут быть не заданы пути с текстовыми файлами, это бессмысленно
		if (filesystem::exists(pPath1) == false)
		{
			cout << "No path " << Params::GetIni().sData1 << endl;
			return;
		}
	}

	if (Params::GetIni().bOnlyParseDataToBinary2 == true)
	{
		//≈сли задан режим только парсинга текстовых данных в бинарные, а сами пути дл€ бинарных данных не заданы в параметрах, то ничего не надо делать
		if (bBinaryPath2Set == false)
		{
			SaveLog("No BinaryData2 Param, but set OnlyParseDataToBinary2");
			return;
		}
		//“акже могут быть не заданы пути с текстовыми файлами, это бессмысленно
		if (filesystem::exists(pPath2) == false)
		{
			cout << "No path " << Params::GetIni().sData2 << endl;
			return;
		}
	}
	
	string sName1 = Params::GetIni().sName1;
	string sName2 = Params::GetIni().sName2;


	vector<filesystem::path> vFileNames1;
	vector<filesystem::path> vFileNames2;

	//‘ункци€ формировани€ списка нужных файлов.
	auto FindDataFiles = [](filesystem::path& pPath, string& sName, vector<filesystem::path>& vFileNames, bool bBinaryExt)
	{
		string sExt = bBinaryExt ? ".bin" : ".txt";
		
		for (auto& it : filesystem::directory_iterator(pPath))
		{
			if (it.is_directory() == false)
			{
				auto sFileName = it.path().filename().string();

				//»щем файлы по префиксу
				if (sFileName.find(Params::GetIni().sPrefixIn) == 0)
				{

					//»щем файлы с заданным именем
					if (sFileName.find(sName) != string::npos && sFileName.find(sExt) != string::npos)
					{
						vFileNames.push_back(it.path());
					}

				}
			}
		}
	};

	
	bool bBinaryData1 = true;
	bool bBinaryData2 = true;
	
	if (bBinaryPath1Set && Params::GetIni().bOnlyParseDataToBinary1 == false)
	{
		FindDataFiles(pBinaryPath1, sName1, vFileNames1, true);
	}
	if (bBinaryPath2Set && Params::GetIni().bOnlyParseDataToBinary2 == false)
	{
		FindDataFiles(pBinaryPath2, sName2, vFileNames2, true);
	}

	if (vFileNames1.empty())
	{
		if (filesystem::exists(pPath1) == false)
		{
			cout << "No path " << Params::GetIni().sData1 << endl;
			return;
		}
		
		FindDataFiles(pPath1, sName1, vFileNames1, false);
		bBinaryData1 = false;
	}

	if (vFileNames2.empty())
	{
		if (filesystem::exists(pPath2) == false)
		{
			cout << "No path " << Params::GetIni().sData2 << endl;
			return;
		}
		
		FindDataFiles(pPath2, sName2, vFileNames2, false);
		bBinaryData2 = false;
	}

	int nThreadsCount = min(Params::GetIni().nThreadsCount, vFileNames1.size());
	
	FilesInteraction pFilesInteraction(bBinaryPath1Set, bBinaryPath2Set, vFileNames2.size());

	
	if (Params::GetIni().bOnlyParseDataToBinary1)
	{
		
		vector<thread> vThreads;
		
		for (int iThread = 0; iThread < nThreadsCount; iThread++)
		{
			const int a = iThread * vFileNames1.size() / nThreadsCount;

			const int b = (iThread + 1) * vFileNames1.size() / nThreadsCount;
			
			vThreads.emplace_back([&pFilesInteraction, &vFileNames1, a, b, iThread]()
			{
				for (int i = a; i < b; i++)
				{
					pFilesInteraction.ParseData(vFileNames1[i], pFilesInteraction.vData1[iThread], Params::GetIni().nSkipStrings1);
					pFilesInteraction.WriteBinaryData(vFileNames1[i], pFilesInteraction.vData1[iThread], Params::GetIni().sBinaryData1);
						
				}
			});
		}

		for (auto& it : vThreads)
		{
			it.join();
		}
	}

	if (Params::GetIni().bOnlyParseDataToBinary2)
	{
		nThreadsCount = min(Params::GetIni().nThreadsCount, vFileNames2.size());

		vector<thread> vThreads;

		for (int iThread = 0; iThread < nThreadsCount; iThread++)
		{
			const int a = iThread * vFileNames2.size() / nThreadsCount;

			const int b = (iThread + 1) * vFileNames2.size() / nThreadsCount;

			vThreads.emplace_back([&pFilesInteraction, &vFileNames2, a, b, iThread]()
				{
					for (int i = a; i < b; i++)
					{
						pFilesInteraction.ParseData(vFileNames2[i], pFilesInteraction.vData2[iThread], Params::GetIni().nSkipStrings2);
						pFilesInteraction.WriteBinaryData(vFileNames2[i], pFilesInteraction.vData2[iThread], Params::GetIni().sBinaryData2);

					}
				});
			
		}

		for (auto& it : vThreads)
		{
			it.join();
		}
	}
	
	if (Params::GetIni().bOnlyParseDataToBinary1 || Params::GetIni().bOnlyParseDataToBinary2)
	{
		cout << "Sum Time = " << clock() - t << endl;

		SaveLog("Calc time = " + to_string(clock() - t));
		
		return;
	}

	if (Params::GetIni().bFullMemory)
	{
		nThreadsCount = min(Params::GetIni().nThreadsCount, vFileNames2.size());
		
		vector<thread> vThreads;

		for (int iThread = 0; iThread < nThreadsCount; iThread++)
		{
			const int a = iThread * vFileNames1.size() / nThreadsCount;

			const int b = (iThread + 1) * vFileNames1.size() / nThreadsCount;

			if (bBinaryData2 == true)
			{

				vThreads.emplace_back([&pFilesInteraction, &vFileNames2, a, b]()
					{
						for (int i = a; i < b; i++)
						{
							pFilesInteraction.ReadBinaryData(vFileNames2[i], pFilesInteraction.vData2[i], Params::GetIni().sBinaryData2);

						}
					});
			}
			else
			{
				vThreads.emplace_back([&pFilesInteraction, &vFileNames2, a, b]()
					{
						for (int i = a; i < b; i++)
						{
							pFilesInteraction.ParseData(vFileNames2[i], pFilesInteraction.vData2[i], Params::GetIni().nSkipStrings2);
							pFilesInteraction.WriteBinaryData(vFileNames2[i], pFilesInteraction.vData2[i], Params::GetIni().sBinaryData2);

						}
					});
			}
		}

		for (auto& it : vThreads)
		{
			it.join();
		}
	}
	
	vector<atomic_bool> vBinaryState1(vFileNames1.size());

	vector<atomic_bool> vBinaryState2(vFileNames2.size());


	for (auto& it : vBinaryState1)
	{
		it = bBinaryData1;
	}

	for (auto& it : vBinaryState2)
	{
		it = bBinaryData2;
	}

	

	pFilesInteraction.InitPrevNumberData(nThreadsCount);
	
	vector<thread> vThreads;


	cout << "ThreadsCount = " << nThreadsCount << endl;

	SaveLog("ThreadsCount = " + to_string(nThreadsCount));



	for (int iThread = 0; iThread < nThreadsCount; iThread++)
	{
		const int a = iThread * vFileNames1.size() / nThreadsCount;

		const int b = (iThread + 1) * vFileNames1.size() / nThreadsCount;

		vThreads.emplace_back([&pFilesInteraction, &vFileNames1, &vFileNames2, &vBinaryState1, &vBinaryState2, a, b, iThread]()
			{
				for (auto iIndex1 = a; iIndex1 < b; iIndex1++)
				{
					for (auto iIndex2 = 0; iIndex2 < vFileNames2.size(); iIndex2++)
					{
						pFilesInteraction.ReadFiles(iIndex1, vFileNames1[iIndex1], vFileNames2[iIndex2], vBinaryState1[iIndex1], vBinaryState2[iIndex2], iThread);
					}
				}
			});
	}

	
	for (auto& it : vThreads)
	{
		it.join();
	}

	cout << "Sum Time = " << clock() - t << endl;

	SaveLog("Calc time = " + to_string(clock() - t));
}


bool Processor::GetPhysParams(string& sFileName)
{
	fstream f(sFileName, ios::in);

	string s;

	Params::Phys pPhys1 = { 0.0, 0.0, false };
	Params::Phys pPhys2 = { 0.0, 0.0, false };
	
	while (getline(f, s))
	{
		if (s.find(Params::GetIni().sName1) != string::npos)
		{
			getline(f, s);
			if (s.find("diameter") != string::npos)
			{
				auto iIndex1 = s.find('=');
				auto iIndex2 = s.find('[');

				string sInfo = s.substr(iIndex1 + 1, iIndex2 - iIndex1 - 1);

				pPhys1.fDiameter = stod(sInfo);
			}
			getline(f, s);
			if (s.find("dencity") != string::npos)
			{
				auto iIndex1 = s.find('=');
				auto iIndex2 = s.find('[');

				string sInfo = s.substr(iIndex1 + 1, iIndex2 - iIndex1 - 1);

				pPhys1.fDencity = stod(sInfo);
			}
		}

		if (s.find(Params::GetIni().sName2) != string::npos)
		{
			getline(f, s);
			if (s.find("diameter") != string::npos)
			{
				auto iIndex1 = s.find('=');
				auto iIndex2 = s.find('[');

				string sInfo = s.substr(iIndex1 + 1, iIndex2 - iIndex1 - 1);

				pPhys2.fDiameter = stod(sInfo);
			}
			getline(f, s);
			if (s.find("dencity") != string::npos)
			{
				auto iIndex1 = s.find('=');
				auto iIndex2 = s.find('[');

				string sInfo = s.substr(iIndex1 + 1, iIndex2 - iIndex1 - 1);

				pPhys2.fDencity = stod(sInfo);
			}
		}
	}

	
	if (pPhys1.fDiameter > 0 && pPhys1.fDencity > 0 && pPhys2.fDiameter > 0 && pPhys2.fDencity > 0)
	{
		const double PI = 3.141592653589793238462643;

		//Ќоходим €кобы самое массивное тело, потом в формулах нужны тольно его физические данные
		if (PI * pPhys1.fDiameter * pPhys1.fDiameter * pPhys1.fDiameter * pPhys1.fDencity / 6 > PI * pPhys2.fDiameter * pPhys2.fDiameter * pPhys2.fDiameter * pPhys2.fDencity / 6)
		{
			Params::GetPhys().fDiameter = pPhys1.fDiameter;
			Params::GetPhys().fDencity = pPhys1.fDencity;
			Params::GetPhys().bFirst = true;
		}
		else
		{
			Params::GetPhys().fDiameter = pPhys2.fDiameter;
			Params::GetPhys().fDencity = pPhys2.fDencity;
			Params::GetPhys().bFirst = false;
		}
		return true;
	}
	else
		return false;
}


void Processor::SetPriority()
{

	int nPriority = Params::GetIni().nPriority;

	HANDLE hCurrentProcess = GetCurrentProcess();
	switch (nPriority)
	{
	case 0:
		SetPriorityClass(hCurrentProcess, REALTIME_PRIORITY_CLASS);
		break;
	case 1:
		SetPriorityClass(hCurrentProcess, HIGH_PRIORITY_CLASS);
		break;
	case 2:
		SetPriorityClass(hCurrentProcess, ABOVE_NORMAL_PRIORITY_CLASS);
		break;
	case 3:
		SetPriorityClass(hCurrentProcess, NORMAL_PRIORITY_CLASS);
		break;
	default:
		SetPriorityClass(hCurrentProcess, NORMAL_PRIORITY_CLASS);
	}
}