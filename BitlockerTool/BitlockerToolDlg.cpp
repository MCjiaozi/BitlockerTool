
// BitlockerToolDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "BitlockerTool.h"
#include "BitlockerToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>


// CBitlockerToolDlg 对话框

TCHAR* StringToChar(CString& str);

CBitlockerToolDlg::CBitlockerToolDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_BITLOCKERTOOL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBitlockerToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBitlockerToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOCK, &CBitlockerToolDlg::OnBnClickedLOCK)
	ON_BN_CLICKED(IDC_BUTTON_UNLOCK, &CBitlockerToolDlg::OnBnClickedButtonUnlock)
END_MESSAGE_MAP()


// CBitlockerToolDlg 消息处理程序

BOOL CBitlockerToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBitlockerToolDlg::OnPaint()
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
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBitlockerToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CBitlockerToolDlg::OnBnClickedLOCK()
{
	CString DriveLetter;
	GetDlgItem(IDC_COMBO_LOCK)->GetWindowText(DriveLetter);
	if (DriveLetter == _T("")) {
		GetDlgItem(IDC_EDIT_OUT)->SetWindowText(_T("请选择盘符！"));
		return;
	}
	CString Para = _T("-lock ") + DriveLetter + _T(":");
	executeCmd(_T("manage-bde.exe ") + Para, TRUE);
	// TODO: 在此添加控件通知处理程序代码
}


void CBitlockerToolDlg::OnBnClickedButtonUnlock()
{
	CString DriveLetter;
	GetDlgItem(IDC_COMBO_LOCK)->GetWindowText(DriveLetter);
	if (DriveLetter == _T("")) {
		GetDlgItem(IDC_EDIT_OUT)->SetWindowText(_T("请选择盘符！"));
		return;
	}
	CString Para = _T("-unlock ") + DriveLetter + _T(":") + _T(" -pw");
	executeCmd(_T("cmd.exe /c @echo off&&title 请输入密码以解锁卷")+ DriveLetter +_T("&&mode 45,10&&manage-bde.exe ") + Para, FALSE);
	// TODO: 在此添加控件通知处理程序代码
}

void CBitlockerToolDlg::executeCmd(CString command, BOOL LOCK)
{
	HANDLE hReadPipe, hWritePipe;
	SECURITY_ATTRIBUTES safety;
	safety.nLength = sizeof(SECURITY_ATTRIBUTES);
	safety.lpSecurityDescriptor = NULL;
	safety.bInheritHandle = TRUE;
	if (!CreatePipe(&hReadPipe, &hWritePipe, &safety, 0))
	{
		MessageBox(_T("创建管道错误！"));
	}
	TCHAR* cmdStr = StringToChar(command);
	STARTUPINFO startupInfo = { sizeof(startupInfo) };
	startupInfo.hStdError = hWritePipe;
	startupInfo.hStdOutput = hWritePipe;
	startupInfo.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
	if (LOCK) {
		startupInfo.wShowWindow = SW_HIDE;
	}
	else {
		startupInfo.wShowWindow = SW_SHOW;
	}
	PROCESS_INFORMATION pinfo;
	if (!CreateProcess(NULL, cmdStr, NULL, NULL, TRUE, NULL, NULL, NULL, &startupInfo, &pinfo))
	{
		MessageBox(_T("创建进程错误！"));
	}
	CloseHandle(pinfo.hProcess);
	CloseHandle(pinfo.hThread);
	CloseHandle(hWritePipe);
	char buffer[4096];
	DWORD byteRead;
	CString output;
	while (true)
	{
		memset(buffer, 0, 4096);
		if (ReadFile(hReadPipe, buffer, 4095, &byteRead, NULL) == NULL)
		{
			break;
		}
		output += buffer;
	}
	CloseHandle(hReadPipe);
	GetDlgItem(IDC_EDIT_OUT)->SetWindowText(output);
}
TCHAR* StringToChar(CString& str)
{
	int len = str.GetLength();
	TCHAR* tr = str.GetBuffer(len);
	str.ReleaseBuffer();
	return tr;
}