
// LControllerDlg.h : 헤더 파일
//

#pragma once
#include <highgui.h>
#include <cv.h>
#include <cxcore.h>
#include "afxwin.h"
#include "DataSocket.h"

#define PORT 4001
// CLControllerDlg 대화 상자
class CLControllerDlg : public CDialogEx
{
// 생성입니다.
public:
	CLControllerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	CDataSocket dataSocket;
// 대화 상자 데이터입니다.
	enum { IDD = IDD_LCONTROLLER_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnCloseSocket(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReceiveData(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	CString str_loc;
	CvCapture *capture;
	IplImage *image;
	bool viewState;
	void ShowIplImage(IplImage* piplimg, HDC hDCDst, RECT* pDstRect);
	void Show(IplImage* piplimg, HDC dc, int x, int y, int w, int h, int from_x, int from_y);
	void FillBmpInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	virtual BOOL DestroyWindow();
	CButton mBtnCam;
	afx_msg void OnBnClickedWcam();
};
