/* Программа для анализа орбит транснептуновых объектов.
 * Нужна для специфических исследований в области астрономии.
 * Связь с разработчиком - theconclav@gmail.com
 */

#include <iostream>
#include "Params.h"
#include "Processor.h"

ErlLogger pLogger;

void SaveLog(string& str)
{
	pLogger.SaveEvent(0, str.c_str());
}

void SaveLog(string str)
{
	pLogger.SaveEvent(0, str.c_str());
}

void SaveLog(char* str)
{
	pLogger.SaveEvent(0, str);
}

void SaveLog(const char* str)
{
	pLogger.SaveEvent(0, str);
}

int main()
{
	pLogger.InitProtocol(const_cast<char *>("TNOAnalyzer"), const_cast<char*>(Params::GetIni().sLogDir.c_str()), 365);

	
	
	Processor pProcessor;

	pProcessor.SetPriority();
	
	pProcessor.ReadDirectories();
	
    return 0;
}

