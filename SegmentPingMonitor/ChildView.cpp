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

//タイマー識別用ID
#define TIMER_REDRAW_ID  1

// CChildView

CChildView::CChildView()
{
    //IPアドレスを設定していない場合は192.168.1.xのセグメントにPINGを実行する。
    //その為の初期値は0xc0a80101(192.168.1.1)
    dwIPAddress=0xc0a80101;
}

CChildView::~CChildView()
{
    //ダブルバッファ領域の削除
    if (m_MemDC.GetSafeHdc())
        m_MemDC.DeleteDC();
    if (m_MemBitmap.GetSafeHandle())
        m_MemBitmap.DeleteObject();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
    ON_WM_CREATE()
    ON_COMMAND(ID_BUTTON_FORM, &CChildView::OnButtonForm)
    ON_COMMAND(ID_BUTTON_PING, &CChildView::OnButtonPing)
    ON_MESSAGE(WM_CUSTOM_STATUS, &CChildView::OnCustomStatus)
    ON_MESSAGE(WM_CUSTOM_FINISH, &CChildView::OnCustomFinish)
    ON_WM_DESTROY()
    ON_WM_SIZE()
    ON_WM_ERASEBKGND()
    ON_WM_TIMER()
END_MESSAGE_MAP()



// CChildView message handlers
//ウインドウ生成時のパラメータ設定
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

//画面の描画処理（実際の細かい描画はDrawScene関数内で行う）
void CChildView::OnPaint() 
{

    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

    // メモリ内のビットマップとデバイスコンテキストを作成
    if (!m_bMemDCValid || m_MemBitmap.GetSafeHandle() == nullptr)
        CreateMemDC(&dc, clientRect);

    // 背景を塗りつぶす
    m_MemDC.FillSolidRect(clientRect, GetSysColor(COLOR_WINDOW));

    // シーンを描画する
    DrawScene(&m_MemDC);

    // メモリ内のビットマップを画面に転送する
    dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &m_MemDC, 0, 0, SRCCOPY);

}



int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here
    workerthread = 0;
    rowLists = 0;

    //Gridの配列内をすべて-1で初期化する。すべての配列に-1が入っている場合は徐々に色が変わる表示となる。
    InitializeGrid();
    //タイマーが設定状態でないことを記憶する為の初期値0をセットする。アプリケーションs終了時に0以外の場合はタイマーをKillする処理がある。
    m_timerID = 0;
    return 0;
}
//表示をすべていったんクリア（段階的な色表示）する為の初期値-1を配列全体にセットする。
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

#include "CIPAddressDialog.h"
//SETTINGボタン押下時のコード
void CChildView::OnButtonForm()
{
    // TODO: Add your command handler code here
    //IPアドレス指定用ダイアログの表示
    CIPAddressDialog dlg(this);
    if (IDCANCEL == dlg.DoModal()) {
        return;
    }

    dwIPAddress = dlg.GetIPDW();

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

    //IPアドレスのセグメントをステータスバーに表示
    AfxGetApp()->m_pMainWnd->SendMessage(WM_CUSTOM_STATUSBAR, 0, (LPARAM)&data);

}

#include "ThreadMaker.h"
//PINGボタン押下時の実行コード
void CChildView::OnButtonPing()
{
    // TODO: Add your command handler code here
    //再描画間隔 １秒
    UINT interval = 1 * 1000;
    //CALLBACK* timerHandler = NULL;
    //タイマーを設定する。
    m_timerID = SetTimer(TIMER_REDRAW_ID, interval, NULL);
    // タイマーを設定できない場合
    if (m_timerID == 0)
    {
        AfxMessageBox(_T("タイマーを設定できませんでした。"));
    }

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
//配列の状態を更新 wParamで指定する場所にlParamの値を書き込む
//wParamは0～253の値を取りうる 0の場合はIPアドレス４オクテット目で1を示す（1相対の為）
//lParamは-1,0,4の値を取りうる -1(PING実行前) 0(PING実行後疎通なし) 4(PING実行後疎通あり)
afx_msg LRESULT CChildView::OnCustomStatus(WPARAM wParam, LPARAM lParam)
{

    int* ptr = &GridStatus[0][0]; // 二次元配列の先頭アドレスをポインタに代入
    ptr[wParam] = (int)lParam;
    //Invalidate(); //実験的に書き換えた内容を画面に表示する。
    return 0;
}
//すべてのスレッド処理完了後にタイマーを止める処理の実行
afx_msg LRESULT CChildView::OnCustomFinish(WPARAM wParam, LPARAM lParam)
{
    if (m_timerID != 0)
    {
        Invalidate();
        // タイマーの設定を解除します。
        BOOL err = KillTimer(m_timerID);
        // タイマーの設定が解除できない場合
        if (!err)
        {
            ::AfxMessageBox(_T("タイマの設定を解除できませんでした。"));
        }
        m_timerID = 0;
    }
    return 0;
}
//Rowと読んでいるのは、２次元配列を１次元配列で管理してその一次元の行番号のみで考えている為
void CChildView::initRow()
{
    // TODO: Add your implementation code here.
    std::vector<int> rows;
    int* ptr = &GridStatus[0][0]; // 二次元配列の先頭アドレスをポインタに代入
    for (int i = 0; i < 254; i++) {
        //PING未チェック状態では-1を入れておく。-1が入っていると画面では虹色の表示になる。
        ptr[i] = -1;
        rows.push_back(i);
    }

    //すでに行（第4オクテット1が0行目の扱い）情報がある場合は行情報をdeleteする。
    if (rowLists != 0) {
        rowLists->del();
        delete rowLists;
    }
    //行の払い出し処理を初期化する。引数に対象となる行の数を与える
    rowLists = new RowManager(rows);
    //第４オクテッドが1～254になるまでを対象とするためのIPアドレス文字列生成を行う
    for (int i = 1; i < 255; i++) {
        //チェックしたいIPアドレスを生成する。
        ipAddress.Format(_T("%d.%d.%d.%d"),
            (int)((dwIPAddress >> 24) & 0xFF),
            (int)((dwIPAddress >> 16) & 0xFF),
            (int)((dwIPAddress >> 8) & 0xFF),
            (int)(i & 0xFF));//(int)(dwIPAddress & 0xFF));
        //生成したIPアドレスを払い出しクラスに登録して紐づける
        rowLists->SetIpAddr(ipAddress.GetBuffer());
    }

}

//払い出しの済んでいない行を取得する。(行を0相対で返す)
int CChildView::GetRow()
{
    // TODO: Add your implementation code here.
    //第４セグメントの1を0行目と考えて、まだPINGしていない行の払い出しを呼び出し、行を返す。
    return rowLists->GetNextRow();
}

void CChildView::OnDestroy()
{
    CWnd::OnDestroy();
    // タイマが設定されている場合
    if (m_timerID != 0)
    {
        // タイマーの設定を解除します。
        BOOL err = KillTimer(m_timerID);
        // タイマーの設定が解除できない場合
        if (!err)
        {
            ::AfxMessageBox(_T("タイマの設定を解除できませんでした。"));
        }
        m_timerID = 0;
    }
    //ワーカースレッドが存在する場合はdeleteする。
    if ((unsigned long)workerthread != 0) {
        delete workerthread;
    }
    //解放漏れが発生する為、要素がある場合は解放する。
    if (rowLists != 0) {
        rowLists->del();
        delete rowLists;
    }

    // TODO: Add your message handler code here
}
void CChildView::CreateMemDC(CDC* pDC, const CRect& rect)
{
    //ダブルバッファリング用のメモリ領域を作成します。
    //初期画面表示などでpDCがNULL状態の場合に落ちる状況を迂回する
    if (pDC != nullptr) {
        if (m_MemBitmap.GetSafeHandle())
            m_MemBitmap.DeleteObject();
        if (m_MemDC.GetSafeHdc())
            m_MemDC.DeleteDC();

        CDC* pTempDC = GetDC();
        m_MemDC.CreateCompatibleDC(pTempDC);
        ReleaseDC(pTempDC);

        m_MemBitmap.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
        m_MemDC.SelectObject(&m_MemBitmap);
        m_bMemDCValid = TRUE;
    }
}

void CChildView::DrawScene(CDC* pDC)
{
    // ダブルバッファリングされた描画処理をここに記述する
    // pDC はメモリ内のデバイスコンテキスト (m_MemDC) を指す
    // TODO: Add your message handler code here
    CRect rectClient;
    GetClientRect(&rectClient);

    pDC->SetBkMode(TRANSPARENT); // 背景を透明にする
    CFont font;
    font.CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

    CFont* pOldFont = pDC->SelectObject(&font);
    COLORREF originalColor = pDC->GetTextColor(); // 元の文字色を保存
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
            int green = col * numSteps; // 緑成分を固定
            int blue = row * (255 / numSteps); // 青成分を変化させる

            COLORREF color;// = RGB(red, green, blue);
            //グリッド配列（２次元配列でみて、値が-1の場合は段階的な色指定を行う。文字色は黒）
            if (GridStatus[row][col] == -1) {
                color = RGB(red, green, blue);
                pDC->SetTextColor(RGB(0, 0, 0)); // 新しい文字色を設定
            }//グリッド配列（２次元配列で見て、値が0の場合は疎通なしと判断して、RGB各色を半分に設定する。文字色は白）
            else if (GridStatus[row][col] == 0) {
                color = RGB(red * 0.5, green * 0.5, blue * 0.5);
                pDC->SetTextColor(RGB(255, 255, 255)); // 新しい文字色を設定
            }//グリッド配列（２次元配列で考えて、値が4の場合は疎通ありとして、RGB各色の明るさを倍に設定する。255を超える場合は255に丸める。文字色は黒）
            else if (GridStatus[row][col] == 4) {
                color = RGB(red * 2 > 255 ? 255 : red * 2,
                    green * 2 > 255 ? 255 : green * 2,
                    blue * 2 > 255 ? 255 : blue * 255);
                pDC->SetTextColor(RGB(0, 0, 0)); // 新しい文字色を設定
            }
            //画面の一番右下から２つ分はブロードキャストや割り当てがないアドレスとなる為、色を黒、文字色を白に設定する。
            if ((row == 15) && ((col == 14) || (col == 15))) {
                color = RGB(0, 0, 0);
                pDC->SetTextColor(RGB(255, 255, 255)); // 新しい文字色を設定
            }
            CBrush brush(color);

            pDC->SelectObject(&brush);

            int rectWidth = rectClient.Width() / numSteps;
            int rectHeight = rectClient.Height() / numSteps;

            int left = col * rectWidth;
            int top = row * rectHeight;
            int right = left + rectWidth;
            int bottom = top + rectHeight;

            //段階的に色塗りを変更しながら色塗りを行う（すでに塗りつぶす色は設定済みで枠付き四角で塗りつぶすのみの処理を行う）
            pDC->Rectangle(CRect(left, top, right, bottom));
            //上記で塗りつぶした中心にIPアドレス第4オクテッドの数字を記載する
            CString strData;
            strData.Format(_T("%03d"), ipCount);
            //数字の描画処理
            pDC->DrawText(strData, CRect(left, top, right, bottom), DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
            //ipアドレスの第四オクテッド部分の値を１進める
            ipCount++;
        }
    }
    pDC->SelectObject(pOldFont); // 元のフォントに戻す
    font.DeleteObject(); // フォントを解放
    pDC->SetTextColor(originalColor); // 元の文字色に戻す
}

//画面サイズ変更時イベント
void CChildView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // 画面サイズが変更されたときに、メモリ内のビットマップとデバイスコンテキストを再作成する
    if (cx > 0 && cy > 0)
    {
        CRect clientRect(0, 0, cx, cy);

        if (m_MemBitmap.GetSafeHandle())
            m_MemBitmap.DeleteObject();
        if (m_MemDC.GetSafeHdc())
            m_MemDC.DeleteDC();

        CreateMemDC(NULL, clientRect);
        //Invalidate();  // 再描画を要求
    }
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
// ダブルバッファリング実行時に、画面が白にちらつく（画面の白消去を行う）処理を行わないようにする。
// TODO: Add your message handler code here and/or call default
//    return CWnd::OnEraseBkgnd(pDC);
    return TRUE;
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == TIMER_REDRAW_ID)
    {
        // タイマーが発生したときの処理をここに記述
        Invalidate();//再描画を要求
    }
    CWnd::OnTimer(nIDEvent);
}
