
// LControllerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "LController.h"
#include "LControllerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLControllerDlg 대화 상자




CLControllerDlg::CLControllerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CLControllerDlg::IDD, pParent)
	, str_loc(_T(""))
	, m_strSendData(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLControllerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STR_LOC, str_loc);
	DDX_Control(pDX, IDWCAM, mBtnCam);
	DDX_Text(pDX, IDC_EDIT_SEND_DATA, m_strSendData);
}

BEGIN_MESSAGE_MAP(CLControllerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDWCAM, &CLControllerDlg::OnBnClickedWcam)
	ON_BN_CLICKED(IDC_BTNSEND, &CLControllerDlg::OnBnClickedBtnsend)
END_MESSAGE_MAP()


// CLControllerDlg 메시지 처리기

BOOL CLControllerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.
	//image = 0;
	image = (IplImage *)malloc(sizeof(IplImage));
	if(image==NULL) {
		AfxMessageBox(TEXT("메모리 할당 에러"));
		exit(1);
	}
	memset(image, 0x00, sizeof(image));
	image->roi = (struct _IplROI*)malloc(sizeof(struct _IplROI));
	if(image->roi==NULL) {
		AfxMessageBox(TEXT("메모리 할당 에러"));
		exit(1);
	}
	memset(image->roi, 0x00, sizeof(struct _IplROI));
	image->imageData = NULL;

	viewState = FALSE;
	OnBnClickedWcam();

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CLControllerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CLControllerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}



// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CLControllerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CLControllerDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CString strLoc;
	strLoc.Format("X:%4d, Y:%4d", point.x, point.y);
	GetDlgItem(IDC_STR_LOC)->SetWindowText(_T(strLoc));
	//str_loc.SetString("111");
	CDialogEx::OnMouseMove(nFlags, point);
}


BOOL CLControllerDlg::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return CDialogEx::OnMouseWheel(nFlags, zDelta, pt);
}


void CLControllerDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonDown(nFlags, point);
}


void CLControllerDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnLButtonUp(nFlags, point);
}


void CLControllerDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnRButtonDown(nFlags, point);
}


void CLControllerDlg::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CDialogEx::OnRButtonUp(nFlags, point);
}

void CLControllerDlg::ShowIplImage(IplImage* piplimg, HDC hDCDst, RECT* pDstRect)
{
	if( pDstRect && piplimg && piplimg->depth == IPL_DEPTH_8U && piplimg->imageData )
    {
        uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
        BITMAPINFO* bmi = (BITMAPINFO*)buffer;
        int bmp_w = piplimg->width, bmp_h = piplimg->height;
 
        CvRect roi = cvGetImageROI( piplimg );
        CvRect dst = RectToCvRect( *pDstRect );
 
        if( roi.width == dst.width && roi.height == dst.height )
        {
            Show( piplimg, hDCDst, dst.x, dst.y, dst.width, dst.height, roi.x, roi.y );
        }
        else
        {
            SetStretchBltMode(hDCDst, (roi.width > dst.width) ? HALFTONE : COLORONCOLOR);
            FillBmpInfo(bmi, bmp_w, bmp_h, (piplimg->depth & 255) * piplimg->nChannels, piplimg->origin);
 
            ::StretchDIBits(hDCDst, dst.x, dst.y, dst.width, dst.height,
                roi.x, roi.y, roi.width, roi.height, piplimg->imageData, bmi, DIB_RGB_COLORS, SRCCOPY);
        }
	}
}

void CLControllerDlg::Show(IplImage* piplimg, HDC dc,
	int x, int y, int w, int h, int from_x, int from_y)
{
	if( piplimg && piplimg->depth == IPL_DEPTH_8U )
    {
        uchar buffer[sizeof(BITMAPINFOHEADER) + 1024];
        BITMAPINFO* bmi = (BITMAPINFO*)buffer;
        int bmp_w = piplimg->width, bmp_h = piplimg->height;
 
        FillBmpInfo( bmi, bmp_w, bmp_h, (piplimg->depth & 255) * piplimg->nChannels, piplimg->origin );
 
        from_x = MIN( MAX( from_x, 0 ), bmp_w - 1 );
        from_y = MIN( MAX( from_y, 0 ), bmp_h - 1 );
 
        int sw = MAX( MIN( bmp_w - from_x, w ), 0 );
        int sh = MAX( MIN( bmp_h - from_y, h ), 0 );
 
        SetDIBitsToDevice(
            dc, x, y, sw, sh, from_x, from_y, from_y, sh,
            piplimg->imageData + from_y * piplimg->widthStep,
            bmi, DIB_RGB_COLORS );
    }
}

void CLControllerDlg::FillBmpInfo(BITMAPINFO* bmi, int width, int height, int bpp, int origin)
{
	assert( bmi && width >= 0 && height >= 0 && (bpp == 8 || bpp == 24 || bpp == 32));
 
    BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);
 
    memset( bmih, 0, sizeof(*bmih));
    bmih->biSize = sizeof(BITMAPINFOHEADER);
    bmih->biWidth = width;
    bmih->biHeight = origin ? abs(height) : -abs(height);
    bmih->biPlanes = 1;
    bmih->biBitCount = (unsigned short)bpp;
    bmih->biCompression = BI_RGB;
 
    if( bpp == 8 )
    {
        RGBQUAD* palette = bmi->bmiColors;
        int i;
        for( i = 0; i < 256; i++ )
        {
            palette[i].rgbBlue = palette[i].rgbGreen = palette[i].rgbRed = (BYTE)i;
            palette[i].rgbReserved = 0;
        }
    }
}

void CLControllerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	CWnd *pWnd = GetDlgItem(IDC_STATIC);
	CDC *pDc;

	if(1 == nIDEvent){
		if(send_msg("1")<0) {
			OnBnClickedWcam();
			AfxMessageBox(TEXT("서버와 연결이 끊겼습니다."));
			return;
		}
		if(recv_msg()<0) {
			OnBnClickedWcam();
			AfxMessageBox(TEXT("서버와 연결이 끊겼습니다."));
			return;
		}
		//cvGrabFrame(capture);
		//image = cvQueryFrame(capture);
		RECT rect;
		pWnd->GetClientRect(&rect);
		pDc = pWnd->GetDC();
		ShowIplImage(image, pDc->GetSafeHdc(), &rect);
	
		ReleaseDC(pDc);
	}

	CDialogEx::OnTimer(nIDEvent);
}

BOOL CLControllerDlg::DestroyWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	//cvReleaseCapture(&capture);
	return CDialogEx::DestroyWindow();
}


void CLControllerDlg::OnBnClickedWcam()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if(viewState == TRUE) {
		GetDlgItem(IDWCAM)->SetWindowText(_T("▶"));
		//cvReleaseCapture(&capture);
		dataSocket.Close();
		pictureSocket.Close();
		viewState = FALSE;
		KillTimer(1);
	}
	else {
		// 소켓연결 or 장치입력
		//capture = cvCaptureFromCAM(0);

		// Odroid
		//m_strServerAddress = "192.168.0.22";
		// Nexus S
		m_strServerAddress = "192.168.0.29";

		// keyboard connect
		if(dataSocket.m_hSocket != INVALID_SOCKET)
		{
			AfxMessageBox(TEXT("이미 키보드가 연결 되어 있습니다."));
			return;
		}
		if(!dataSocket.Create())
		{
			AfxMessageBox(TEXT("소켓 생성 실패 - 1"));
			GetDlgItem(IDWCAM)->SetWindowText(_T("▶"));
			return;
		}
		// IP와 포트로 연결에 실패하는 경우
		if(!dataSocket.Connect(m_strServerAddress, KEYBOARD_PORT))
		{
			dataSocket.Close();
			AfxMessageBox(TEXT("키보드 연결 실패"));
			GetDlgItem(IDWCAM)->SetWindowText(_T("▶"));
			return;
		}
		
		// picture connect
		if(pictureSocket.m_hSocket != INVALID_SOCKET)
		{
			AfxMessageBox(TEXT("이미 화면이 연결 되어 있습니다."));
			return;
		}
		if(!pictureSocket.Create())
		{
			dataSocket.Close();
			AfxMessageBox(TEXT("소켓 생성 실패 - 2"));
			GetDlgItem(IDWCAM)->SetWindowText(_T("▶"));
			return;
		}
		// IP와 포트로 연결에 실패하는 경우
		if(!pictureSocket.Connect(m_strServerAddress, PICTURE_PORT))
		{
			dataSocket.Close();
			pictureSocket.Close();
			AfxMessageBox(TEXT("화면 연결 실패"));
			GetDlgItem(IDWCAM)->SetWindowText(_T("▶"));
			return;
		}
		
		/* // 해상도설정
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 200); 
		cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 400);
		*/

		// 타이머 속도설정 선택
		SetTimer(1,300,NULL);
		//SetTimer(1,((double)1/12)*1000,NULL);

		viewState = TRUE;
		//AfxMessageBox(TEXT("연결 성공"));
		GetDlgItem(IDWCAM)->SetWindowText(_T("■"));
	}
}

LRESULT CLControllerDlg::OnReceiveData(WPARAM wParam, LPARAM lParam)
{
	char Rcvdata[MAXLINE];

	CDataSocket* pDataSocket = (CDataSocket*)wParam;

	pDataSocket->Receive(Rcvdata, sizeof(Rcvdata));

	CString strMsg = Rcvdata;

	if(!strcmp(strMsg, "2")){
		AfxMessageBox(TEXT(strMsg));
	}
	
	return 0;
	
}

// 서버와의 연결 종료 처리
LRESULT CLControllerDlg::OnCloseSocket(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

int CLControllerDlg::send_msg(CString data)
{
	return pictureSocket.Send(data, data.GetLength()+1);
}


int CLControllerDlg::recv_msg(void)
{
	int ret;
	BMPHEAD bh;
	int imgSize;
	char* pData = NULL;
	
	memset((void*)&bh, 0x00, sizeof(bh));
	ret = pictureSocket.Receive(&bh, sizeof(bh), 0);
	if(ret<0) {
		return -1;
	}
	// BMPHEAD to IplImage
	image->nChannels = 3;
	image->depth = bh.bits / image->nChannels;
	image->origin = 0;
	image->width = bh.width;
	image->height = bh.depth; 
	image->roi->height = image->height;
	image->roi->width = image->width;

	imgSize = image->width * image->height
		* image->depth * image->nChannels / 8;
	if(image->imageData){
		realloc(image->imageData, imgSize);
	}
	else {
		image->imageData = (char*) malloc(imgSize);
	}
	if(!image->imageData)
		return -1;

	ret = 0;
	pData = image->imageData;
	do {
		ret = pictureSocket.Receive(pData, imgSize, 0);
		if(ret<0) {
			return -1;
		}
		imgSize -= ret;
		pData += ret;
	}while(imgSize>0);

	/*
	image->nSize;                           
    image->ID;                                                     
    image->alphaChannel;                                    
    image->colorModel[4];             
    image->channelSeq[4];            
    image->dataOrder;                                                    
    image->align;        
    image->roi;                
    image->maskROI;    
    image->imageId;                    
    image->tileInfo;
    image->imageSize;    
    image->widthStep;                     
    image->BorderMode[4];
    image->BorderConst[4];
    image->imageDataOrigin;
	*/

	return 0;
}


void CLControllerDlg::inputDirectioinKey(int key)
{
	CString strCmd;
	switch(key){
	case INPUT_UP:
		strCmd.Format("%s", "\\\\CONTROL_U");
		break;
	case INPUT_DOWN:
		strCmd.Format("%s", "\\\\CONTROL_D");
		break;
	case INPUT_RIGHT:
		strCmd.Format("%s", "\\\\CONTROL_R");
		break;
	case INPUT_LEFT:
		strCmd.Format("%s", "\\\\CONTROL_L");
		break;
	case INPUT_ENTER:
		strCmd.Format("%s", "\\\\CONTROL_E");
		break;
	case INPUT_BACK:
		strCmd.Format("%s", "\\\\CONTROL_B");
		break;
	default:
		// no-effect
		break;
	}	

	sendMsg(strCmd);
}


BOOL CLControllerDlg::PreTranslateMessage(MSG* pMsg)
{
	CWnd * pWnd = GetFocus ();

	if (pWnd == NULL)
	return FALSE;

	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if ((GetFocus()->GetDlgCtrlID() == IDC_EDIT_SEND_DATA)) {
		if (pMsg->message == WM_KEYDOWN){
			switch( pMsg->wParam ) {
				case VK_RETURN:
					OnBnClickedBtnsend();
					return TRUE;
				case VK_ESCAPE:
					GetDlgItem(IDC_STR_LOC)->SetFocus();
					GetDlgItem(IDC_STR_LOC)->SendMessage(WM_KILLFOCUS, NULL); 
					
					return TRUE; 
			}
		}
	} else if (pMsg->message == WM_KEYDOWN) {
		switch( pMsg->wParam ) {
			case VK_UP:
				inputDirectioinKey(INPUT_UP);
				return TRUE;
			case VK_DOWN:
				inputDirectioinKey(INPUT_DOWN);
				return TRUE;
			case VK_LEFT:
				inputDirectioinKey(INPUT_LEFT);
				return TRUE;
			case VK_RIGHT:
				inputDirectioinKey(INPUT_RIGHT);
				return TRUE;
			case VK_RETURN:
				inputDirectioinKey(INPUT_ENTER);
				return TRUE;
			case VK_ESCAPE:
				inputDirectioinKey(INPUT_BACK);
				return TRUE; 
			default:
				// no effect
				break;
			}
		}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CLControllerDlg::sendMsg(CString strMsg)
{
	int ret = 0;
	ret = dataSocket.Send(AnsiToUTF8RetCString(strMsg), 
		AnsiToUTF8RetCString(strMsg).GetLength()+1);

	if(ret < 1){
		return;
	}
}


CString CLControllerDlg::AnsiToUTF8RetCString(CString inputStr)
{
	WCHAR szUnicode[MAXLINE];
	char szUTF8char[MAXLINE];

	CString strConvert;
	char* szSrc = (LPSTR)(LPCTSTR)inputStr;

	char szRetValue[MAXLINE] = "";

	int unicodeSize = MultiByteToWideChar(CP_ACP, 0,
		szSrc, (int)strlen(szSrc), 
		szUnicode, sizeof(szUnicode));

	int UTF8CodeSize = WideCharToMultiByte(CP_UTF8, 0, 
		szUnicode, unicodeSize, szUTF8char,
		sizeof(szUTF8char), NULL, NULL);

	memcpy(szRetValue, szUTF8char, UTF8CodeSize);
	strConvert = szRetValue;

	return strConvert;
}


void CLControllerDlg::OnBnClickedBtnsend()
{ 
	UpdateData();
	CString strSend;
	strSend.Format("%s", m_strSendData);

	// Ansi를 UTF8로 변경하고 CString으로 리턴된 것을 전송
	if(!strSend.Compare((_T(""))) ){
		return;
	}
	sendMsg(strSend);

	GetDlgItem(IDC_EDIT_SEND_DATA)->SetFocus();
	m_strSendData = "";
	UpdateData(FALSE);
	
}
