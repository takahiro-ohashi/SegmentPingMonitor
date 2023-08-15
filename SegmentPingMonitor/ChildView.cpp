// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "SegmentPingMonitor.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
    dwIPAddress=0xc0a80101;
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
    ON_WM_CREATE()
    ON_COMMAND(ID_BUTTON_FORM, &CChildView::OnButtonForm)
    ON_COMMAND(ID_BUTTON_PING, &CChildView::OnButtonPing)
    ON_MESSAGE(WM_CUSTOM_STATUS, &CChildView::OnCustomStatus)
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
    CRect rectClient;
    GetClientRect(&rectClient);

    dc.SetBkMode(TRANSPARENT); // 背景を透明にする
    CFont font;
    font.CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

    CFont* pOldFont = dc.SelectObject(&font);
    COLORREF originalColor = dc.GetTextColor(); // 元の文字色を保存
    int cellSize = 20; // セルのサイズ（ピクセル単位）
    int numRows = 16;
    int numCols = 16;

    int numSteps = 16; // グラデーションのステップ数

    int ipCount = 1;
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            int red = 255 - row * (255 / numSteps); // 赤成分を変化させる
            int green = col*numSteps; // 緑成分を固定
            int blue = row * (255 / numSteps); // 青成分を変化させる

            COLORREF color;// = RGB(red, green, blue);
            if (GridStatus[row][col] == -1) {
                color = RGB(red, green, blue);
                dc.SetTextColor(RGB(0, 0, 0)); // 新しい文字色を設定
            }else if (GridStatus[row][col] == 0) {
                color = RGB(red*0.5,green*0.5,blue*0.5);
                dc.SetTextColor(RGB(255,255,255)); // 新しい文字色を設定
            }
            else if (GridStatus[row][col] == 4) {
                color = RGB(red * 2 > 255 ? 255 : red * 2,
                    green * 2 > 255 ? 255 : green * 2,
                    blue * 2 > 255 ? 255:blue*255);
                dc.SetTextColor(RGB(0, 0, 0)); // 新しい文字色を設定
            }
            if ((row == 15) && ((col == 14)||(col == 15))) {
                color = RGB(0, 0, 0);
                dc.SetTextColor(RGB(255, 255, 255)); // 新しい文字色を設定
            }
            CBrush brush(color);

            dc.SelectObject(&brush);

            int rectWidth = rectClient.Width() / numSteps;
            int rectHeight = rectClient.Height() / numSteps;

            int left = col * rectWidth;
            int top = row * rectHeight;
            int right = left + rectWidth;
            int bottom = top + rectHeight;

            dc.Rectangle(CRect(left, top, right, bottom));
            CString strData;
            strData.Format(_T("%03d"), ipCount);
            //dc.DrawText(strData, CRect(left, top, right, bottom), DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
            //dc.DrawText(strData, CRect(left, top, right, bottom), DT_LEFT | DT_SINGLELINE | DT_NOPREFIX | DT_TOP);
            dc.DrawText(strData, CRect(left, top, right, bottom), DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
            ipCount++;
        }
    }
    dc.SelectObject(pOldFont); // 元のフォントに戻す
    font.DeleteObject(); // フォントを解放
	// TODO: Add your message handler code here
    dc.SetTextColor(originalColor); // 元の文字色に戻す
	// Do not call CWnd::OnPaint() for painting messages
}



int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here
    workerthread = 0;
    rowLists = 0;

    InitializeGrid();
    return 0;
}
void CChildView::InitializeGrid() 
{
    for (int y = 0; y < 16; y++) 
    {
        for (int x = 0; x < 16; x++) {
            GridStatus[y][x] = -1;
        }
    }
    //GridStatus[5][5] = 0;
    //GridStatus[10][10] = 4;

}
// ダイアログを表示するためのコード
//CIPAddressDialog dlg;
//dlg.DoModal();

#include "CIPAddressDialog.h"
static UINT indicators[] =
{
   ID_SEPARATOR
};
void CChildView::OnButtonForm()
{
    // TODO: Add your command handler code here
    CIPAddressDialog dlg(this);
    if (IDCANCEL == dlg.DoModal()) {
        return;
    }

    //CIPAddressCtrl *m_ipAddressCtrl = &dlg.m_ipAddressCtrl;
    //PostMessage(WM_CUSTOM_STATUSBAR, 0, 0);
    //DWORD dwIPAddress;

    //m_ipAddressCtrl->GetAddress(dwIPAddress);
    dwIPAddress = dlg.GetIPDW();


    
    //ipAddress.Format(_T("%d.%d.%d.%d"),
    //    (int)((dwIPAddress >> 24) & 0xFF),
    //    (int)((dwIPAddress >> 16) & 0xFF),
    //    (int)((dwIPAddress >> 8) & 0xFF),
    //    (int)(dwIPAddress & 0xFF));
    ipAddress.Format(_T("%d.%d.%d.%d"),
        (int)((dwIPAddress >> 24) & 0xFF),
        (int)((dwIPAddress >> 16) & 0xFF),
        (int)((dwIPAddress >> 8) & 0xFF),
        (int)(0));


    tagSendData m_sendData;
    // 渡したいデータを設定します。
    //m_sendData.msg = _T("192.168.1.0");
    m_sendData.msg = ipAddress;

    // WM_COPYDATAで使用するデータ構造体を設定します。
    COPYDATASTRUCT data;
    data.dwData = 0;

    // 渡したいデータのポインタを設定します。
    data.lpData = &m_sendData;

    // 渡したいデータのサイズを設定します。
    data.cbData = sizeof(m_sendData);

    AfxGetApp()->m_pMainWnd->SendMessage(WM_CUSTOM_STATUSBAR, 0, (LPARAM)&data);
    // WM_COPYDATAメッセージを送信します。
    //SendMessage(WM_COPYDATA, MSG_SEND_DATA, (LPARAM)&data);


    //CStatusBar* pBar = (CStatusBar*)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    //char Separator[90];
    //CMFCRibbonStatusBar* pBar = (CMFCRibbonStatusBar*)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    //CMFCRibbonStatusBar* pBar = (CMFCRibbonStatusBar*)AfxGetApp()->m_pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR);
    //((CMainFrame*)(AfxGetApp()->m_pMainWnd)->m_wndStatusBar;
    //char Separator[256];
    //sprintf_s(Separator,256, "TEST");
    //LPCTSTR aa = _T("AAAAAAA");
    //pBar->SetPaneText(0, (LPCTSTR)Separator, TRUE);
    //pBar->SetDlgItemTextW(0, (LPCTSTR)Separator);
    //pBar->SetPaneText(0, (LPCTSTR)Separator, TRUE);

    //pBar->SetWindowText(aa);
    //pBar->SetWindowText((LPCTSTR)Separator);

}

#include "ThreadMaker.h"
void CChildView::OnButtonPing()
{
    // TODO: Add your command handler code here
    work.freeThreads();
    if ((unsigned long long)workerthread != 0) {
        delete workerthread;
        workerthread = (unsigned long long)0;
    }
    workerthread = new ThreadMaker((LPVOID)this, 1);
    work.setNumber(1);
    work.add(workerthread);
    workerthread->priority(1);
    work.start();
}
afx_msg LRESULT CChildView::OnCustomStatus(WPARAM wParam, LPARAM lParam)
{
    //CListCtrl& m_List = GetListCtrl();
    //char Status[5];
    //sprintf_s(Status, 5, "%d", (int)lParam);
    //m_List.SetItemText((int)wParam, 3, Status);
    //(int*)(&GridStatus[0][0])
    int* ptr = &GridStatus[0][0]; // 二次元配列の先頭アドレスをポインタに代入
    ptr[wParam] = (int)lParam;
    //Sleep(1000);
    Invalidate();
    return 0;
}
void CChildView::initRow()
{
    // TODO: Add your implementation code here.
    std::vector<int> rows;
    int* ptr = &GridStatus[0][0]; // 二次元配列の先頭アドレスをポインタに代入
    for (int i = 0; i < 254; i++) {
        ptr[i] = -1;
        //GetListCtrl().SetItemText(i, 3, _T("-1"));
        rows.push_back(i);
    }

    if (rowLists != 0) {
        rowLists->del();
        delete rowLists;
    }
    rowLists = new RowManager(rows);
    //ipAddress = _T("127.0.0.1");
    //rowLists->SetIpAddr(ipAddress.GetBuffer());
    for (int i = 1; i < 255; i++) {
        //char tszText[512];
        //memset(tszText, 0, 512);
        ipAddress.Format(_T("%d.%d.%d.%d"),
            (int)((dwIPAddress >> 24) & 0xFF),
            (int)((dwIPAddress >> 16) & 0xFF),
            (int)((dwIPAddress >> 8) & 0xFF),
            (int)(i & 0xFF));//(int)(dwIPAddress & 0xFF));
        //GetListCtrl().GetItemText(i, 0, tszText, 256);
        //rowLists->SetIpAddr(tszText);
        //ipAddress = _T("192.168.1.2");
        rowLists->SetIpAddr(ipAddress.GetBuffer());
    }

}


int CChildView::GetRow()
{
    // TODO: Add your implementation code here.
    return rowLists->GetNextRow();
}