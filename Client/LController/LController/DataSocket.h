#pragma once

// CDataSocket 명령 대상입니다.

#pragma once
#define UM_RECEIVE_MESSAGE (WM_USER+101)
#define UM_CLOSE_MESSAGE (WM_USER+102)

class CDataSocket : public CSocket
{
public:
	CDataSocket();
	virtual ~CDataSocket();
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
};


