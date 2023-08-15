#pragma once


// IPAddressDialog

class CIPAddressDialog : public CDialogEx
{
	DWORD dwIPAddress;
	DECLARE_DYNAMIC(CIPAddressDialog)
	CIPAddressDialog::CIPAddressDialog(CWnd* pParent /*=nullptr*/)
		: CDialogEx(IDD_IPADDRESS_DIALOG, pParent)
	{
		dwIPAddress = 0;
	}
public:
	CIPAddressDialog();
	virtual ~CIPAddressDialog();

protected:
	DECLARE_MESSAGE_MAP()
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
public:
	CIPAddressCtrl m_ipAddressCtrl;
	DWORD GetIPDW() {
		//DWORD dwIPAddress;
		//m_ipAddressCtrl.GetAddress(dwIPAddress);
		return dwIPAddress;
	}

	afx_msg void OnIdok();
	afx_msg void OnIdcancel();
};


