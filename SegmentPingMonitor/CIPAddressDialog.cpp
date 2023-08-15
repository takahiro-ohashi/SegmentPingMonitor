// IPAddressDialog.cpp : implementation file
//

#include "pch.h"
#include "SegmentPingMonitor.h"
#include "CIPAddressDialog.h"


// IPAddressDialog

IMPLEMENT_DYNAMIC(CIPAddressDialog, CWnd)
void CIPAddressDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    //DDX_Control(pDX, IDC_IPADDRESS_CONTROL, m_ipAddressCtrl);
}

CIPAddressDialog::CIPAddressDialog()
{

}

CIPAddressDialog::~CIPAddressDialog()
{
}


BEGIN_MESSAGE_MAP(CIPAddressDialog, CWnd)
    ON_COMMAND(IDOK, &CIPAddressDialog::OnIdok)
    ON_COMMAND(IDCANCEL, &CIPAddressDialog::OnIdcancel)
END_MESSAGE_MAP()

// IPAddressDialog message handlers

void CIPAddressDialog::OnIdok()
{
    CIPAddressCtrl* pIPAddCtrl
        = (CIPAddressCtrl*)this->GetDlgItem(IDC_IPADDRESS_CONTROL);
    pIPAddCtrl->GetAddress(dwIPAddress);
    //dwIPAddress = pIPAddCtrl;
    CDialogEx::OnOK();
    // TODO: Add your command handler code here
}


void CIPAddressDialog::OnIdcancel()
{
    // TODO: Add your command handler code here
    CDialogEx::OnCancel();
}
