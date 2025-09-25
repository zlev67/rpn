
// RpnCalculatorDlg.h : header file
//

#pragma once
#include "RpnCalculator.h"

#define HOTKEY_ID 0x1000

class CRpnCalculatorDlg : public CDialogEx
{
    RpnCalculator* calc;
#define HOTKEY_ID 0x1000

    // Construction
public:
    CRpnCalculatorDlg(CWnd* pParent = nullptr);
    virtual ~CRpnCalculatorDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_RPNCALCULATOR_DIALOG};
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

    // Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnHotKey(WPARAM wParam, LPARAM lParam);
    void OnDestroy();
    DECLARE_MESSAGE_MAP()
public:
    CEdit m_editInput;
    CStatic m_staticResult;
    afx_msg void OnBnClickedButtonCalc();
    afx_msg void OnBnClickedRadio1();
    afx_msg void OnBnClickedButton1();
    afx_msg void OnEnChangeEdit1();
    afx_msg void OnBnClickedCancel();
};