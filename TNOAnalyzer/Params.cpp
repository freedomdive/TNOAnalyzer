#include "Params.h"
#include <filesystem>

Params Params::inst;

namespace _Ini
{
	bool _WritePrivateProfileString(std::string&& sSection, std::string&& sKey, std::string&& sValue, const std::filesystem::path& sPath)
	{
		return WritePrivateProfileString(sSection.c_str(), sKey.c_str(), sValue.c_str(), sPath.string().c_str());
	}

	std::string _GetPrivateProfileString(std::string&& sSection, std::string&& sKey, std::string&& sDefault, const std::filesystem::path& sPath)
	{
		static char szBuffer[256];

		GetPrivateProfileString(sSection.c_str(), sKey.c_str(), sDefault.c_str(), szBuffer, sizeof(szBuffer), sPath.string().c_str());

		return szBuffer;
	}

	int _GetPrivateProfileInt(std::string&& sSection, std::string&& sKey, int&& nDefault, const std::filesystem::path& sPath)
	{
		return GetPrivateProfileInt(sSection.c_str(), sKey.c_str(), nDefault, sPath.string().c_str());
	}

	double _GetPrivateProfileDouble(std::string&& sSection, std::string&& sKey, double&& nDefault, const std::filesystem::path& sPath)
	{
		return stod(_GetPrivateProfileString(move(sSection), move(sKey), std::to_string(nDefault), sPath));
	}

	std::vector<std::string> _GetPrivateProfileSectionNames(const std::filesystem::path& sPath)
	{
		static char szBuffer[256];
		GetPrivateProfileSectionNames(szBuffer, sizeof(szBuffer), sPath.string().c_str());

		char* p = szBuffer;

		std::vector<std::string> vRet;

		std::string x = p;

		vRet.push_back(x);

		for (int i = 0; i < 256; i++)
		{
			p++;
			if (*p == 0)
			{
				x = p + 1;

				if (x.empty())
					break;

				vRet.push_back(x);
			}
		}

		return vRet;
	}

}



Params::Params()
{

	ReadIni();
}

void Params::ReadIni()
{
	static char szIniFile[MAX_PATH];
	sprintf_s(szIniFile, "%s.ini", "TNOAnalyzer");
	_fullpath(szIniFile, szIniFile, MAX_PATH - 1);


	if (!filesystem::exists(szIniFile))
	{
		return;
	}

	m_ini.sLogDir = _Ini::_GetPrivateProfileString("all", "LogDir", "", szIniFile);
	m_ini.sResults1 = _Ini::_GetPrivateProfileString("all", "Results1", "", szIniFile);
	m_ini.sResults2 = _Ini::_GetPrivateProfileString("all", "Results2", "", szIniFile);
	m_ini.sData1 = _Ini::_GetPrivateProfileString("all", "Data1", "", szIniFile);
	m_ini.sData2 = _Ini::_GetPrivateProfileString("all", "Data2", "", szIniFile);
	m_ini.sBinaryData1 = _Ini::_GetPrivateProfileString("all", "BinaryData1", "?", szIniFile);
	m_ini.sBinaryData2 = _Ini::_GetPrivateProfileString("all", "BinaryData2", "?", szIniFile);
	
	m_ini.sName1 = _Ini::_GetPrivateProfileString("all", "Name1", "", szIniFile);
	
	m_ini.sName2 = _Ini::_GetPrivateProfileString("all", "Name2", "", szIniFile);

	

	m_ini.nSkipStrings1 = _Ini::_GetPrivateProfileInt("all", "SkipStrings1", 0, szIniFile);
	m_ini.nSkipStrings2 = _Ini::_GetPrivateProfileInt("all", "SkipStrings2", 0, szIniFile);
	
	m_ini.bOnlyParseDataToBinary1 = _Ini::_GetPrivateProfileInt("all", "OnlyParseDataToBinary1", 0, szIniFile);
	
	m_ini.bOnlyParseDataToBinary2 = _Ini::_GetPrivateProfileInt("all", "OnlyParseDataToBinary2", 0, szIniFile);
	
	m_ini.sPrefixIn = _Ini::_GetPrivateProfileString("all", "PrefixIn", "", szIniFile);
	m_ini.sPrefixOut1 = _Ini::_GetPrivateProfileString("all", "PrefixOut1", "", szIniFile);
	m_ini.sPrefixOut2 = _Ini::_GetPrivateProfileString("all", "PrefixOut2", "", szIniFile);
	
	m_ini.sPhysParamsFileName = _Ini::_GetPrivateProfileString("all", "PhysParamsFileName", "", szIniFile);
	
	m_ini.nThreadsCount = _Ini::_GetPrivateProfileInt("all", "ThreadsCount", 1, szIniFile);
	m_ini.bFullMemory = _Ini::_GetPrivateProfileInt("all", "FullMemory", 0, szIniFile);

	m_ini.nThreadsCount = max(m_ini.nThreadsCount, 1);


	m_ini.sResultsFileHeader1 = _Ini::_GetPrivateProfileString("all", "ResultsFileHeader1", "", szIniFile);
	
	m_ini.sResultsFileHeader2 = _Ini::_GetPrivateProfileString("all", "ResultsFileHeader2", "", szIniFile);
	
	m_ini.nPriority = _Ini::_GetPrivateProfileInt("all", "Priority", 3, szIniFile);
}