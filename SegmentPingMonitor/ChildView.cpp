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

//�^�C�}�[���ʗpID
#define TIMER_REDRAW_ID  1

// CChildView

CChildView::CChildView()
{
    //IP�A�h���X��ݒ肵�Ă��Ȃ��ꍇ��192.168.1.x�̃Z�O�����g��PING�����s����B
    //���ׂ̈̏����l��0xc0a80101(192.168.1.1)
    dwIPAddress=0xc0a80101;
}

CChildView::~CChildView()
{
    //�_�u���o�b�t�@�̈�̍폜
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
//�E�C���h�E�������̃p�����[�^�ݒ�
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

//��ʂ̕`�揈���i���ۂׂ̍����`���DrawScene�֐����ōs���j
void CChildView::OnPaint() 
{

    CPaintDC dc(this);
    CRect clientRect;
    GetClientRect(&clientRect);

    // ���������̃r�b�g�}�b�v�ƃf�o�C�X�R���e�L�X�g���쐬
    if (!m_bMemDCValid || m_MemBitmap.GetSafeHandle() == nullptr)
        CreateMemDC(&dc, clientRect);

    // �w�i��h��Ԃ�
    m_MemDC.FillSolidRect(clientRect, GetSysColor(COLOR_WINDOW));

    // �V�[����`�悷��
    DrawScene(&m_MemDC);

    // ���������̃r�b�g�}�b�v����ʂɓ]������
    dc.BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &m_MemDC, 0, 0, SRCCOPY);

}



int CChildView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CWnd::OnCreate(lpCreateStruct) == -1)
        return -1;

    // TODO:  Add your specialized creation code here
    workerthread = 0;
    rowLists = 0;

    //Grid�̔z��������ׂ�-1�ŏ���������B���ׂĂ̔z���-1�������Ă���ꍇ�͏��X�ɐF���ς��\���ƂȂ�B
    InitializeGrid();
    //�^�C�}�[���ݒ��ԂłȂ����Ƃ��L������ׂ̏����l0���Z�b�g����B�A�v���P�[�V����s�I������0�ȊO�̏ꍇ�̓^�C�}�[��Kill���鏈��������B
    m_timerID = 0;
    return 0;
}
//�\�������ׂĂ�������N���A�i�i�K�I�ȐF�\���j����ׂ̏����l-1��z��S�̂ɃZ�b�g����B
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
//SETTING�{�^���������̃R�[�h
void CChildView::OnButtonForm()
{
    // TODO: Add your command handler code here
    //IP�A�h���X�w��p�_�C�A���O�̕\��
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
    // �n�������f�[�^��ݒ肵�܂��B
    //m_sendData.msg = _T("192.168.1.0");
    m_sendData.msg = ipAddress;

    // WM_COPYDATA�Ŏg�p����f�[�^�\���̂�ݒ肵�܂��B
    COPYDATASTRUCT data;
    data.dwData = 0;

    // �n�������f�[�^�̃|�C���^��ݒ肵�܂��B
    data.lpData = &m_sendData;

    // �n�������f�[�^�̃T�C�Y��ݒ肵�܂��B
    data.cbData = sizeof(m_sendData);

    //IP�A�h���X�̃Z�O�����g���X�e�[�^�X�o�[�ɕ\��
    AfxGetApp()->m_pMainWnd->SendMessage(WM_CUSTOM_STATUSBAR, 0, (LPARAM)&data);

}

#include "ThreadMaker.h"
//PING�{�^���������̎��s�R�[�h
void CChildView::OnButtonPing()
{
    // TODO: Add your command handler code here
    //�ĕ`��Ԋu �P�b
    UINT interval = 1 * 1000;
    //CALLBACK* timerHandler = NULL;
    //�^�C�}�[��ݒ肷��B
    m_timerID = SetTimer(TIMER_REDRAW_ID, interval, NULL);
    // �^�C�}�[��ݒ�ł��Ȃ��ꍇ
    if (m_timerID == 0)
    {
        AfxMessageBox(_T("�^�C�}�[��ݒ�ł��܂���ł����B"));
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
//�z��̏�Ԃ��X�V wParam�Ŏw�肷��ꏊ��lParam�̒l����������
//wParam��0�`253�̒l����肤�� 0�̏ꍇ��IP�A�h���X�S�I�N�e�b�g�ڂ�1�������i1���΂ׁ̈j
//lParam��-1,0,4�̒l����肤�� -1(PING���s�O) 0(PING���s��a�ʂȂ�) 4(PING���s��a�ʂ���)
afx_msg LRESULT CChildView::OnCustomStatus(WPARAM wParam, LPARAM lParam)
{

    int* ptr = &GridStatus[0][0]; // �񎟌��z��̐擪�A�h���X���|�C���^�ɑ��
    ptr[wParam] = (int)lParam;
    //Invalidate(); //�����I�ɏ������������e����ʂɕ\������B
    return 0;
}
//���ׂẴX���b�h����������Ƀ^�C�}�[���~�߂鏈���̎��s
afx_msg LRESULT CChildView::OnCustomFinish(WPARAM wParam, LPARAM lParam)
{
    if (m_timerID != 0)
    {
        Invalidate();
        // �^�C�}�[�̐ݒ���������܂��B
        BOOL err = KillTimer(m_timerID);
        // �^�C�}�[�̐ݒ肪�����ł��Ȃ��ꍇ
        if (!err)
        {
            ::AfxMessageBox(_T("�^�C�}�̐ݒ�������ł��܂���ł����B"));
        }
        m_timerID = 0;
    }
    return 0;
}
//Row�Ɠǂ�ł���̂́A�Q�����z����P�����z��ŊǗ����Ă��̈ꎟ���̍s�ԍ��݂̂ōl���Ă����
void CChildView::initRow()
{
    // TODO: Add your implementation code here.
    std::vector<int> rows;
    int* ptr = &GridStatus[0][0]; // �񎟌��z��̐擪�A�h���X���|�C���^�ɑ��
    for (int i = 0; i < 254; i++) {
        //PING���`�F�b�N��Ԃł�-1�����Ă����B-1�������Ă���Ɖ�ʂł͓��F�̕\���ɂȂ�B
        ptr[i] = -1;
        rows.push_back(i);
    }

    //���łɍs�i��4�I�N�e�b�g1��0�s�ڂ̈����j��񂪂���ꍇ�͍s����delete����B
    if (rowLists != 0) {
        rowLists->del();
        delete rowLists;
    }
    //�s�̕����o������������������B�����ɑΏۂƂȂ�s�̐���^����
    rowLists = new RowManager(rows);
    //��S�I�N�e�b�h��1�`254�ɂȂ�܂ł�ΏۂƂ��邽�߂�IP�A�h���X�����񐶐����s��
    for (int i = 1; i < 255; i++) {
        //�`�F�b�N������IP�A�h���X�𐶐�����B
        ipAddress.Format(_T("%d.%d.%d.%d"),
            (int)((dwIPAddress >> 24) & 0xFF),
            (int)((dwIPAddress >> 16) & 0xFF),
            (int)((dwIPAddress >> 8) & 0xFF),
            (int)(i & 0xFF));//(int)(dwIPAddress & 0xFF));
        //��������IP�A�h���X�𕥂��o���N���X�ɓo�^���ĕR�Â���
        rowLists->SetIpAddr(ipAddress.GetBuffer());
    }

}

//�����o���̍ς�ł��Ȃ��s���擾����B(�s��0���΂ŕԂ�)
int CChildView::GetRow()
{
    // TODO: Add your implementation code here.
    //��S�Z�O�����g��1��0�s�ڂƍl���āA�܂�PING���Ă��Ȃ��s�̕����o�����Ăяo���A�s��Ԃ��B
    return rowLists->GetNextRow();
}

void CChildView::OnDestroy()
{
    CWnd::OnDestroy();
    // �^�C�}���ݒ肳��Ă���ꍇ
    if (m_timerID != 0)
    {
        // �^�C�}�[�̐ݒ���������܂��B
        BOOL err = KillTimer(m_timerID);
        // �^�C�}�[�̐ݒ肪�����ł��Ȃ��ꍇ
        if (!err)
        {
            ::AfxMessageBox(_T("�^�C�}�̐ݒ�������ł��܂���ł����B"));
        }
        m_timerID = 0;
    }
    //���[�J�[�X���b�h�����݂���ꍇ��delete����B
    if ((unsigned long)workerthread != 0) {
        delete workerthread;
    }
    //����R�ꂪ��������ׁA�v�f������ꍇ�͉������B
    if (rowLists != 0) {
        rowLists->del();
        delete rowLists;
    }

    // TODO: Add your message handler code here
}
void CChildView::CreateMemDC(CDC* pDC, const CRect& rect)
{
    //�_�u���o�b�t�@�����O�p�̃������̈���쐬���܂��B
    //������ʕ\���Ȃǂ�pDC��NULL��Ԃ̏ꍇ�ɗ�����󋵂��I�񂷂�
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
    // �_�u���o�b�t�@�����O���ꂽ�`�揈���������ɋL�q����
    // pDC �̓��������̃f�o�C�X�R���e�L�X�g (m_MemDC) ���w��
    // TODO: Add your message handler code here
    CRect rectClient;
    GetClientRect(&rectClient);

    pDC->SetBkMode(TRANSPARENT); // �w�i�𓧖��ɂ���
    CFont font;
    font.CreateFont(25, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET,
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("Arial"));

    CFont* pOldFont = pDC->SelectObject(&font);
    COLORREF originalColor = pDC->GetTextColor(); // ���̕����F��ۑ�
    int cellSize = 20; // �Z���̃T�C�Y�i�s�N�Z���P�ʁj
    int numRows = 16;
    int numCols = 16;

    int numSteps = 16; // �O���f�[�V�����̃X�e�b�v��

    int ipCount = 1;
    for (int row = 0; row < numRows; ++row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            int red = 255 - row * (255 / numSteps); // �Ԑ�����ω�������
            int green = col * numSteps; // �ΐ������Œ�
            int blue = row * (255 / numSteps); // ������ω�������

            COLORREF color;// = RGB(red, green, blue);
            //�O���b�h�z��i�Q�����z��ł݂āA�l��-1�̏ꍇ�͒i�K�I�ȐF�w����s���B�����F�͍��j
            if (GridStatus[row][col] == -1) {
                color = RGB(red, green, blue);
                pDC->SetTextColor(RGB(0, 0, 0)); // �V���������F��ݒ�
            }//�O���b�h�z��i�Q�����z��Ō��āA�l��0�̏ꍇ�͑a�ʂȂ��Ɣ��f���āARGB�e�F�𔼕��ɐݒ肷��B�����F�͔��j
            else if (GridStatus[row][col] == 0) {
                color = RGB(red * 0.5, green * 0.5, blue * 0.5);
                pDC->SetTextColor(RGB(255, 255, 255)); // �V���������F��ݒ�
            }//�O���b�h�z��i�Q�����z��ōl���āA�l��4�̏ꍇ�͑a�ʂ���Ƃ��āARGB�e�F�̖��邳��{�ɐݒ肷��B255�𒴂���ꍇ��255�Ɋۂ߂�B�����F�͍��j
            else if (GridStatus[row][col] == 4) {
                color = RGB(red * 2 > 255 ? 255 : red * 2,
                    green * 2 > 255 ? 255 : green * 2,
                    blue * 2 > 255 ? 255 : blue * 255);
                pDC->SetTextColor(RGB(0, 0, 0)); // �V���������F��ݒ�
            }
            //��ʂ̈�ԉE������Q���̓u���[�h�L���X�g�⊄�蓖�Ă��Ȃ��A�h���X�ƂȂ�ׁA�F�����A�����F�𔒂ɐݒ肷��B
            if ((row == 15) && ((col == 14) || (col == 15))) {
                color = RGB(0, 0, 0);
                pDC->SetTextColor(RGB(255, 255, 255)); // �V���������F��ݒ�
            }
            CBrush brush(color);

            pDC->SelectObject(&brush);

            int rectWidth = rectClient.Width() / numSteps;
            int rectHeight = rectClient.Height() / numSteps;

            int left = col * rectWidth;
            int top = row * rectHeight;
            int right = left + rectWidth;
            int bottom = top + rectHeight;

            //�i�K�I�ɐF�h���ύX���Ȃ���F�h����s���i���łɓh��Ԃ��F�͐ݒ�ς݂Řg�t���l�p�œh��Ԃ��݂̂̏������s���j
            pDC->Rectangle(CRect(left, top, right, bottom));
            //��L�œh��Ԃ������S��IP�A�h���X��4�I�N�e�b�h�̐������L�ڂ���
            CString strData;
            strData.Format(_T("%03d"), ipCount);
            //�����̕`�揈��
            pDC->DrawText(strData, CRect(left, top, right, bottom), DT_CENTER | DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER);
            //ip�A�h���X�̑�l�I�N�e�b�h�����̒l���P�i�߂�
            ipCount++;
        }
    }
    pDC->SelectObject(pOldFont); // ���̃t�H���g�ɖ߂�
    font.DeleteObject(); // �t�H���g�����
    pDC->SetTextColor(originalColor); // ���̕����F�ɖ߂�
}

//��ʃT�C�Y�ύX���C�x���g
void CChildView::OnSize(UINT nType, int cx, int cy)
{
    CWnd::OnSize(nType, cx, cy);

    // ��ʃT�C�Y���ύX���ꂽ�Ƃ��ɁA���������̃r�b�g�}�b�v�ƃf�o�C�X�R���e�L�X�g���č쐬����
    if (cx > 0 && cy > 0)
    {
        CRect clientRect(0, 0, cx, cy);

        if (m_MemBitmap.GetSafeHandle())
            m_MemBitmap.DeleteObject();
        if (m_MemDC.GetSafeHdc())
            m_MemDC.DeleteDC();

        CreateMemDC(NULL, clientRect);
        //Invalidate();  // �ĕ`���v��
    }
}


BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
// �_�u���o�b�t�@�����O���s���ɁA��ʂ����ɂ�����i��ʂ̔��������s���j�������s��Ȃ��悤�ɂ���B
// TODO: Add your message handler code here and/or call default
//    return CWnd::OnEraseBkgnd(pDC);
    return TRUE;
}

void CChildView::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == TIMER_REDRAW_ID)
    {
        // �^�C�}�[�����������Ƃ��̏����������ɋL�q
        Invalidate();//�ĕ`���v��
    }
    CWnd::OnTimer(nIDEvent);
}
