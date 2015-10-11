
// ShowHistDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "ShowHist.h"
#include "ShowHistDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CShowHistDlg 对话框




CShowHistDlg::CShowHistDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CShowHistDlg::IDD, pParent)
	, m_PathName(_T("CTData.img"))
	, m_TH(0)
	, m_H(0)
	, m_L(0)
	, m_width(256)
	, m_height(256)
	, m_depth(256)
	, m_type(0)
	,D(1)
	, m_type1(1)
	, m_type2(1)
	, m_type3(1)
	, m_type4(1)
	, m_type5(1)
	, m_type6(1)
	, Mag_H(444)
	, Mag_L(5)
	, Post_Mag_H(444)
	, Post_Mag_L(5)
	, m_M1(0)
	, m_M2(0)
	, is_linear(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CShowHistDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PATHNAME, m_PathName);
	DDX_Text(pDX, IDC_TH, m_TH);
	DDX_Text(pDX, IDC_H, m_H);
	DDX_Text(pDX, IDC_L, m_L);
	DDX_Text(pDX, IDC_WIDTH, m_width);
	DDX_Text(pDX, IDC_HEIGHT, m_height);
	DDX_Text(pDX, IDC_DEPTH, m_depth);
	DDX_Text(pDX, IDC_TYPE1, m_type1);
	DDX_Text(pDX, IDC_TYPE2, m_type2);
	DDX_Text(pDX, IDC_TYPE3, m_type3);
	DDX_Text(pDX, IDC_TYPE4, m_type4);
	DDX_Text(pDX, IDC_TYPE5, m_type5);
	DDX_Text(pDX, IDC_TYPE6, m_type6);
	DDX_Text(pDX, IDC_MAG_H, Mag_H);
	DDX_Text(pDX, IDC_MAG_L, Mag_L);
	DDX_Text(pDX, IDC_PSOT_MAG_H, Post_Mag_H);
	DDX_Text(pDX, IDC_POST_MAG_L, Post_Mag_L);
	DDX_Text(pDX, IDC_M1, m_M1);
	DDX_Text(pDX, IDC_M2, m_M2);
}

BEGIN_MESSAGE_MAP(CShowHistDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_PATH, &CShowHistDlg::OnBnClickedPath)
	ON_BN_CLICKED(IDC_SETTH, &CShowHistDlg::OnBnClickedSetth)
	ON_BN_CLICKED(IDC_SETTYPE, &CShowHistDlg::OnBnClickedSettype)
	ON_BN_CLICKED(IDC_POSTPOCESS, &CShowHistDlg::OnBnClickedPostpocess)
	ON_BN_CLICKED(IDC_RETRY, &CShowHistDlg::OnBnClickedRetry)
	ON_BN_CLICKED(IDC_BUTTON1, &CShowHistDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_MERGE, &CShowHistDlg::OnBnClickedMerge)
	ON_BN_CLICKED(IDC_SAVE, &CShowHistDlg::OnBnClickedSave)
	ON_BN_CLICKED(IDC_SaveMag, &CShowHistDlg::OnBnClickedSavemag)
	ON_BN_CLICKED(IDC_ISLINEAR, &CShowHistDlg::OnBnClickedIslinear)
	ON_BN_CLICKED(IDC_UP, &CShowHistDlg::OnBnClickedUp)
	ON_BN_CLICKED(IDC_Down, &CShowHistDlg::OnBnClickedDown)
END_MESSAGE_MAP()


// CShowHistDlg 消息处理程序

BOOL CShowHistDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_pDC = GetDlgItem(IDC_HIST)->GetDC();
	GetDlgItem(IDC_HIST)->GetClientRect(&m_rect);
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CShowHistDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CShowHistDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	ShowAccodinate();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CShowHistDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
float4 CShowHistDlg::Tansferfunction(char typenum, float opacity)
{
	switch(typenum)
	{
		case 0:
			return make_float4(0, 0, 0, 0);
			break;
		case 1:
			return make_float4(1, 0, 0, opacity*m_type1);
			break;
		case 2:
			return make_float4(0, 1, 0, opacity*m_type2);
			break;
		case 3:
			return make_float4(0, 0, 1, opacity*m_type3);
			break;
		case 4:
			return make_float4(1, 1, 0, opacity*m_type4);
			break;
		case 5:
			return make_float4(0, 1, 1, opacity*m_type5);
			break;
		case 6:
			return make_float4(1, 0, 1, opacity*m_type6);
			break;
	}
}


void CShowHistDlg::OnBnClickedPath()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	SetDlgItemText(IDC_STATE, "Busy");
	m_histogram = new LH_Histogram(m_PathName, m_width, m_height, m_depth);
	volumesize = make_cudaExtent(m_width-2*D, m_height-2*D, m_depth-2*D);

	p_CUDA_DC = GetDlgItem(IDC_CUDASHOW)->GetDC();
	GetDlgItem(IDC_CUDASHOW)->GetClientRect(&m_CUDA_rect);
	m_volume = new float4[(m_width-2*D)*(m_height-2*D)*(m_depth-2*D)];
	m_mag = new float[(m_width-2*D)*(m_height-2*D)*(m_depth-2*D)];

	for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
	{
		m_volume[i].x = 0;
		m_volume[i].y = 0;
		m_volume[i].z = 0;
		m_volume[i].w = 0;
		m_mag[i] = 0;
 	}
	m_CUDA_show = new Show_CUDA_OpenGL(p_CUDA_DC, volumesize, m_volume, m_CUDA_rect);
	m_CUDA_show->Init();
	SetDlgItemText(IDC_STATE,"Ready");
}



void CShowHistDlg::OnBnClickedSetth()
{
	// TODO: 在此添加控件通知处理程序代码

	SetDlgItemText(IDC_STATE, "Busy");
	UpdateData(TRUE);
	m_low = 0;
	m_high = m_TH;
	m_middle = (m_low+m_high)/2;
	m_TH = m_middle;
	UpdateData(FALSE);
	m_histogram->GetNewHistgram(m_TH, m_type);
	m_histogram->ShowNewImage(m_pDC, m_rect);
	SetDlgItemText(IDC_STATE, "Ready");
}


void CShowHistDlg::OnBnClickedSettype()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_STATE, "Busy");
		UpdateData(TRUE);
		m_type += 1;
		char* classfiy_data = m_histogram->NewClassify(m_L, m_H, Mag_L, Mag_H, m_type, m_mag, is_linear);

		m_histogram->GetNewHistgram(m_TH*0.8, m_type);
		m_histogram->ShowNewImage(m_pDC, m_rect);

		for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
		{
			m_volume[i] = Tansferfunction(classfiy_data[i], m_mag[i]);
		}
		m_CUDA_show->ChangeData(m_volume);
		SetDlgItemText(IDC_STATE, "Ready");
}

void CShowHistDlg::OnBnClickedPostpocess()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_STATE, "Busy");
	UpdateData(TRUE);
	char* classify_data = m_histogram->PostProcess(m_type, Post_Mag_L, Post_Mag_H, m_mag, is_linear);
	/*m_histogram->GetNewHistgram(m_TH*0.8, m_type);
	m_histogram->ShowNewImage(m_pDC, m_rect);*/
	for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
	{
		m_volume[i] = Tansferfunction(classify_data[i], m_mag[i]);
	}
	m_CUDA_show->ChangeData(m_volume);
	SetDlgItemText(IDC_STATE, "Ready");
}



void CShowHistDlg::OnBnClickedRetry()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_STATE, "Busy");
	if(m_type-1 >= 0)
	{
		char* classify_data = m_histogram->Retry(m_type, m_mag);
		m_type -= 1;
		m_H = 0;
		m_L = 0;
		UpdateData(FALSE);
		m_histogram->GetNewHistgram(m_TH, m_type);
		m_histogram->ShowNewImage(m_pDC, m_rect);
		for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
		{
			m_volume[i] = Tansferfunction(classify_data[i], m_mag[i]);
		}
		m_CUDA_show->ChangeData(m_volume);
	}
	SetDlgItemText(IDC_STATE, "Ready");
}

BOOL CShowHistDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	
	if(pMsg->message==WM_KEYDOWN)
	{
		SetDlgItemText(IDC_STATE, "Busy");
		m_CUDA_show->EventResponse(pMsg->wParam);
		SetDlgItemText(IDC_STATE, "Ready");
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CShowHistDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	SetDlgItemText(IDC_STATE, "Busy");
	for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
	{
		m_volume[i] = Tansferfunction(m_histogram->newclassify_data[i], m_mag[i]);
	}
	m_CUDA_show->ChangeData(m_volume);
	SetDlgItemText(IDC_STATE, "Ready");
}

void CShowHistDlg::ShowAccodinate()
{
	CRect window_rect;
	GetWindowRect(window_rect);
	CDC* pDC = GetDC();
	CRect rect;
	GetDlgItem(IDC_HIST)->GetWindowRect(&rect);
	CPoint begin_point;
	begin_point.x = rect.left-100;//有问题，为什么是100？？？
	begin_point.y = rect.bottom-100;
	float jiange = rect.Width()/16.0f;
	float i=jiange;
	int j = 8;
	for(; j <= 256; j= j+16)
	{
		CString str;
		str.Format(_T("%d"), j);
		begin_point.x += jiange;
		pDC->TextOutA(begin_point.x, begin_point.y, str);
	}
	ReleaseDC(pDC);
}


void CShowHistDlg::OnBnClickedMerge()
{
	UpdateData(TRUE);
	SetDlgItemText(IDC_STATE, "Ready");
	if(m_M1 < m_type && m_M2 <= m_type)
	{
		m_histogram->Mergedata(m_M1, m_M2, m_mag, is_linear);
	}
	m_type -= 1;
	for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
	{
		m_volume[i] = Tansferfunction(m_histogram->newclassify_data[i], m_mag[i]);
	}
	m_CUDA_show->ChangeData(m_volume);
	SetDlgItemText(IDC_STATE, "Ready");
	// TODO: 在此添加控件通知处理程序代码
}



void CShowHistDlg::OnBnClickedSave()
{
	// TODO: 在此添加控件通知处理程序代码
	FILE *fp;
	fp = fopen("classify_data.dat", "wb");
	fwrite(m_histogram->newclassify_data, 1, (m_width-2*D)*(m_height-2*D)*(m_depth-2*D), fp);
	fclose(fp);
}


void CShowHistDlg::OnBnClickedSavemag()
{
	// TODO: 在此添加控件通知处理程序代码
	SetDlgItemText(IDC_STATE, "Busy");
	float* temp = new float[(m_width-2*D)*(m_height-2*D)*(m_depth-2*D)];
	float temp_max = 0;
	for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
	{
		temp[i] = m_histogram->data_matitude[i].magnititude_value;
		if(temp_max < temp[i])
			temp_max = temp[i];
	}
	FILE* fp;
	fp = fopen("mag_data.dat", "wb");
	fwrite(temp, sizeof(float), (m_width-2*D)*(m_height-2*D)*(m_depth-2*D), fp);
	fclose(fp);
	SetDlgItemText(IDC_STATE, "Ready");
}


void CShowHistDlg::OnBnClickedIslinear()
{
	is_linear = !is_linear;
	SetDlgItemText(IDC_STATE, "Ready");

	m_histogram->ChangeOpacity(m_type, m_mag, is_linear);
	for(int i = 0; i < (m_width-2*D)*(m_height-2*D)*(m_depth-2*D); i++)
	{
		m_volume[i] = Tansferfunction(m_histogram->newclassify_data[i], m_mag[i]);
	}
	m_CUDA_show->ChangeData(m_volume);
	SetDlgItemText(IDC_STATE, "Ready");
	// TODO: 在此添加控件通知处理程序代码
}


void CShowHistDlg::OnBnClickedUp()
{
	// TODO: 在此添加控件通知处理程序代码
	m_low = m_middle;
	m_middle = (m_low+m_high)/2;
	m_TH = m_middle;
	SetDlgItemText(IDC_STATE, "Busy");
	UpdateData(FALSE);

	m_histogram->GetNewHistgram(m_TH, m_type);
	m_histogram->ShowNewImage(m_pDC, m_rect);
	SetDlgItemText(IDC_STATE, "Ready");
}




void CShowHistDlg::OnBnClickedDown()
{
	// TODO: 在此添加控件通知处理程序代码
	m_high = m_middle;
	m_middle = (m_low+m_high)/2;
	m_TH = m_middle;
	SetDlgItemText(IDC_STATE, "Busy");
	UpdateData(FALSE);

	m_histogram->GetNewHistgram(m_TH, m_type);
	m_histogram->ShowNewImage(m_pDC, m_rect);
	SetDlgItemText(IDC_STATE, "Ready");
}
