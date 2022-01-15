// ==========================================================================================
//	File name:		ErlLogger.h
//	Created:		06.2006
//	Contents:		Протоколирование событий происходящих в системе
//	Autor:			Yuliya Kirpikova, Izvolsky Andrey
//	Office:			Data-Center
// ==========================================================================================
#ifndef __ERLLOGGER_H__
#define __ERLLOGGER_H__

#ifndef ERLLOGGER_EXPORTS
  #define MODULEAPI __declspec(dllimport)
#else
  #define MODULEAPI __declspec(dllexport)
#endif

#ifndef _WIN32
#undef MODULEAPI
#define MODULEAPI
#endif

#include <stdio.h>
#include <windows.h>

//----------------------------------------------

//Начальная <часть имени> файла протокола: <часть_имени>_ГГГГММДД.erl
#define PROT_FILE_TEMPL	"FileProt"

//Расширение файла протокола
#define PROT_FILE_EXT	"erl"

//Длина строки с описанием ошибки
#define BUF_SIZE 1000
class ManageRemoteFile;
//----------------------------------------------

class MODULEAPI ErlLogger
{
public:
	//-----------------------------------------------------------------
	//Конструктор - заполняет всё нулями
	//-----------------------------------------------------------------
	ErlLogger();
	
	//-----------------------------------------------------------------
	//Инициализация переменных протокола
	//[in] FileTemplate			- Начальная <часть имени> файла протокола: <часть_имени>_ГГГГММДД.erl
	//[in] ProtocolDir			- путь к каталогу с протоколами, если 0, то работаем в текущем
	//[in] ProtocolValidTime	- Срок хранения протоколов, сутки
	//
	//Инициализирует:
	//oProtCS				- критическая секция 
	//pProtocolFileTempl	- Начальная <часть имени> файла протокола: <часть_имени>_ГГГГММДД.erl
	//pProtocolDir			- путь к каталогу с протоколами 
	//bInited				- флаг, инициализация прошла успешно
	//
	//Создает каталог, если необходимо
	//Удаляет старые протоколы по сроку ProtocolValidTime
	//
	//Return  ERROR_SUCCESS при успешном завершении
	//		  ошибки при создании каталога см. файл WINERROR.H
	//-----------------------------------------------------------------
	ULONG InitProtocol(PCHAR FileTemplate, PCHAR ProtocolDir,long ProtocolValidTime);

	//-----------------------------------------------------------------
	//Деструктор - вызывает внутри себя CloseProtocol()
	//-----------------------------------------------------------------
	~ErlLogger();
	
	//-----------------------------------------------------------------
	//Функция записи кода ошибки в файл протокола
	//[in] ErrorCode	- код системной ошибки
	//[in] pStrDescr	- cтрока с доп. описанием ошибки
	//
	//Использует критическую секцию для монопольного доступа к ресурсам
	//Открывает файл протокола, если он не был открыт
	//Сохраняет данные в файл в следующем формате: <mm/dd> <hh:mm:ss-ms> <код события> <данные события, через запятую>
	//Return:	ERROR_SUCCESS			- всё хорошо
	//			ERROR_NOT_READY			- не было инициализации
	//			ERROR_OPEN_FAILED		- ошибка при открытии файла
	//-----------------------------------------------------------------
	ULONG SaveEvent	(DWORD	ErrorCode, const char * pStrDescr,...);

protected:
	//-----------------------------------------------------------------
	//Открыть файл протокола
	//Инициализируются:
	//pProtocolFile			- формируется имя <часть_имени>_ГГГГММДД.erl, файл открывает на дозапись
	//CurProtocolFileDate	- текущий день открытия файла протокола	
	//Return:	ERROR_SUCCESS			- всё хорошо
	//			ERROR_NOT_READY			- не было инициализации
	//			ERROR_OPEN_FAILED		- ошибка при открытии файла
	//			ERROR_BUSY				- данный файл уже используется
	//-----------------------------------------------------------------
	ULONG OpenProtocol();

	//-----------------------------------------------------------------
	//Закрыть файл протокола
	//Return:	ERROR_SUCCESS			- всё хорошо
	//			ERROR_NOT_READY			- не было инициализации
	//			ERROR_INVALID_HANDLE	- ошибочный указатель на файл
	//-----------------------------------------------------------------
	ULONG CloseProtocol();

	//-----------------------------------------------------------------

	//Открытый файл протокола
	ManageRemoteFile *ManageFile;

	//Дата открытия файла протокола
	ULONG	CurProtocolFileDate;

	//Путь к каталогу с протоколами
	CHAR	pProtocolDir[MAX_PATH];

	//Начальная <часть имени> файла протокола: <часть_имени>_ГГГГММДД.erl
	CHAR	pProtocolFileTempl[MAX_PATH];

	//Защита записи в протокол
	CRITICAL_SECTION	oProtCS;

	//Флаг успешной инициализации
	bool	bInited;

};

// Наследник, который будет создавать новый файл каждый час
class MODULEAPI ErlLoggerFilePerHour : public ErlLogger
{
public:
	ErlLoggerFilePerHour() : ErlLogger()
	{
		CurProtocolFileHour = 0;
	}
	ULONG SaveEvent	(DWORD	ErrorCode, const char * pStrDescr,...);
protected:
	ULONG OpenProtocol();
// Час открытия файла протокола
	ULONG	CurProtocolFileHour;
};

class MODULEAPI ErlLoggerWithTimeDiff : public ErlLogger
{
public:
	ULONG SaveEvent	(DWORD	ErrorCode, const char * pStrDescr,...);
	ULONG InitProtocol(PCHAR FileTemplate, PCHAR ProtocolDir,long ProtocolValidTime);
protected:
	long timer;
};
#endif		//__ERLLOGGER_H__

