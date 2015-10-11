
// ShowHistDlg.h : 头文件
//

#pragma once
#include "LH_Histogram.h"
#include "Show_CUDA_OpenGL.h"
// CShowHistDlg 对话框
class CShowHistDlg : public CDialogEx
{
// 构造
public:
	CShowHistDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SHOWHIST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void ShowAccodinate();
	CString m_PathName;
	int m_TH;
	int m_H;
	int m_L;
	LH_Histogram* m_histogram;
	CDC* m_pDC;
	CRect m_rect;
	int m_type;
	afx_msg void OnBnClickedPath();
	int m_width;
	int m_height;
	int m_depth;
	afx_msg void OnBnClickedSetth();
	afx_msg void OnBnClickedSettype();
	afx_msg void OnBnClickedPostpocess();
	afx_msg void OnBnClickedRetry();

	Show_CUDA_OpenGL* m_CUDA_show;
	CDC* p_CUDA_DC;
	CRect m_CUDA_rect;
	float4* m_volume;
	float* m_mag;
	cudaExtent volumesize;
	int D;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	float4 Tansferfunction(char type_name, float opacity);
	float m_type1;
	float m_type2;
	float m_type3;
	float m_type4;
	float m_type5;
	float m_type6;

	afx_msg void OnBnClickedButton1();
	float Mag_H;
	float Mag_L;
	float Post_Mag_H;
	float Post_Mag_L;
	int m_M1;
	int m_M2;
	bool is_linear;
	int m_low;
	int m_high;
	int m_middle;
	afx_msg void OnBnClickedMerge();
	afx_msg void OnBnClickedSave();
	afx_msg void OnBnClickedSavemag();
	afx_msg void OnBnClickedIslinear();
	afx_msg void OnBnClickedUp();
	afx_msg void OnBnClickedDown();
};
