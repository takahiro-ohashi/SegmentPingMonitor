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

// ChildView.h : interface of the CChildView class
//


#pragma once
#include "CThread.h"
//#include "ThreadMaker.h"
#include "RowManager.h"
#include <vector>

#define WM_CUSTOM_STATUSBAR (WM_USER + 100)
#define WM_CUSTOM_STATUS (WM_USER + 101)
#define WM_CUSTOM_FINISH (WM_USER + 102)
// CChildView window


class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

	int GridStatus[16][16];
// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
	RowManager* rowLists;
	CThreads work;
	CThread* workerthread;
	CString ipAddress;
	DWORD  dwIPAddress;
	void initRow();
	int GetRow();

	CBitmap m_MemBitmap;
	CDC m_MemDC;
	BOOL m_bMemDCValid;
	UINT m_timerID;

	void CreateMemDC(CDC* pDC, const CRect& rect);
	void DrawScene(CDC* pDC);
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	void InitializeGrid();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnButtonForm();
	afx_msg void OnButtonPing();
	afx_msg LRESULT OnCustomStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnCustomFinish(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

