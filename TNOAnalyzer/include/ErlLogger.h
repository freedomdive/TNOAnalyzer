// ==========================================================================================
//	File name:		ErlLogger.h
//	Created:		06.2006
//	Contents:		���������������� ������� ������������ � �������
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

//��������� <����� �����> ����� ���������: <�����_�����>_��������.erl
#define PROT_FILE_TEMPL	"FileProt"

//���������� ����� ���������
#define PROT_FILE_EXT	"erl"

//����� ������ � ��������� ������
#define BUF_SIZE 1000
class ManageRemoteFile;
//----------------------------------------------

class MODULEAPI ErlLogger
{
public:
	//-----------------------------------------------------------------
	//����������� - ��������� �� ������
	//-----------------------------------------------------------------
	ErlLogger();
	
	//-----------------------------------------------------------------
	//������������� ���������� ���������
	//[in] FileTemplate			- ��������� <����� �����> ����� ���������: <�����_�����>_��������.erl
	//[in] ProtocolDir			- ���� � �������� � �����������, ���� 0, �� �������� � �������
	//[in] ProtocolValidTime	- ���� �������� ����������, �����
	//
	//��������������:
	//oProtCS				- ����������� ������ 
	//pProtocolFileTempl	- ��������� <����� �����> ����� ���������: <�����_�����>_��������.erl
	//pProtocolDir			- ���� � �������� � ����������� 
	//bInited				- ����, ������������� ������ �������
	//
	//������� �������, ���� ����������
	//������� ������ ��������� �� ����� ProtocolValidTime
	//
	//Return  ERROR_SUCCESS ��� �������� ����������
	//		  ������ ��� �������� �������� ��. ���� WINERROR.H
	//-----------------------------------------------------------------
	ULONG InitProtocol(PCHAR FileTemplate, PCHAR ProtocolDir,long ProtocolValidTime);

	//-----------------------------------------------------------------
	//���������� - �������� ������ ���� CloseProtocol()
	//-----------------------------------------------------------------
	~ErlLogger();
	
	//-----------------------------------------------------------------
	//������� ������ ���� ������ � ���� ���������
	//[in] ErrorCode	- ��� ��������� ������
	//[in] pStrDescr	- c����� � ���. ��������� ������
	//
	//���������� ����������� ������ ��� ������������ ������� � ��������
	//��������� ���� ���������, ���� �� �� ��� ������
	//��������� ������ � ���� � ��������� �������: <mm/dd> <hh:mm:ss-ms> <��� �������> <������ �������, ����� �������>
	//Return:	ERROR_SUCCESS			- �� ������
	//			ERROR_NOT_READY			- �� ���� �������������
	//			ERROR_OPEN_FAILED		- ������ ��� �������� �����
	//-----------------------------------------------------------------
	ULONG SaveEvent	(DWORD	ErrorCode, const char * pStrDescr,...);

protected:
	//-----------------------------------------------------------------
	//������� ���� ���������
	//����������������:
	//pProtocolFile			- ����������� ��� <�����_�����>_��������.erl, ���� ��������� �� ��������
	//CurProtocolFileDate	- ������� ���� �������� ����� ���������	
	//Return:	ERROR_SUCCESS			- �� ������
	//			ERROR_NOT_READY			- �� ���� �������������
	//			ERROR_OPEN_FAILED		- ������ ��� �������� �����
	//			ERROR_BUSY				- ������ ���� ��� ������������
	//-----------------------------------------------------------------
	ULONG OpenProtocol();

	//-----------------------------------------------------------------
	//������� ���� ���������
	//Return:	ERROR_SUCCESS			- �� ������
	//			ERROR_NOT_READY			- �� ���� �������������
	//			ERROR_INVALID_HANDLE	- ��������� ��������� �� ����
	//-----------------------------------------------------------------
	ULONG CloseProtocol();

	//-----------------------------------------------------------------

	//�������� ���� ���������
	ManageRemoteFile *ManageFile;

	//���� �������� ����� ���������
	ULONG	CurProtocolFileDate;

	//���� � �������� � �����������
	CHAR	pProtocolDir[MAX_PATH];

	//��������� <����� �����> ����� ���������: <�����_�����>_��������.erl
	CHAR	pProtocolFileTempl[MAX_PATH];

	//������ ������ � ��������
	CRITICAL_SECTION	oProtCS;

	//���� �������� �������������
	bool	bInited;

};

// ���������, ������� ����� ��������� ����� ���� ������ ���
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
// ��� �������� ����� ���������
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

