// RpnMfcAppDlg.cpp
#include "framework.h"
#include "Calculator.h"
#include "RpnCalculatorDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "RpnCalculator.h" // Include your RPN logic
#include "../MoreFuncs/CurrencyAndStock.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CRpnCalculatorDlg::CRpnCalculatorDlg(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_RPNCALCULATOR_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME); // Load the application icon
    calc = new RpnCalculator();
    calc->addStandardFunctions();
    calc->addStandardOperators();
    calc->addFunction("currency", std::unique_ptr<Currency>(new Currency()));
    calc->addFunction("stock", std::unique_ptr<Stock>(new Stock()));
}

CRpnCalculatorDlg::~CRpnCalculatorDlg()
{
    delete calc;
}

BOOL CRpnCalculatorDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // Register global hotkey: Ctrl+Alt+R
    RegisterHotKey(m_hWnd, HOTKEY_ID, MOD_CONTROL | MOD_ALT, 'R');
    CheckRadioButton(IDC_RADIO1, IDC_RADIO2, IDC_RADIO1);
    // Hide window at startup
    ShowWindow(SW_HIDE);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

LRESULT CRpnCalculatorDlg::OnHotKey(WPARAM wParam, LPARAM lParam)
{
    if (wParam == HOTKEY_ID)
    {
        if (IsWindowVisible())
            ShowWindow(SW_HIDE);
        else
        {
            ShowWindow(SW_SHOW);
            SetForegroundWindow();
        }
    }
    return 0;
}

void CRpnCalculatorDlg::OnBnClickedButtonCalc()
{
    CString expr;
    m_editInput.GetWindowText(expr);

    // Convert CString to std::string
    CT2A asciiStr(expr);
    std::string input(asciiStr);
    std::string result;
    try {
        if (IsDlgButtonChecked(IDC_RADIO1) == BST_CHECKED) 
        {
            result = calc->calculate(input);
        }
        else
        {
            result = calc->calculate_rpn(input);
        }
        CString cresult(result.c_str());
        m_staticResult.SetWindowText(cresult);
    }
    catch (const std::exception& e) {
        CString err(e.what());
        m_staticResult.SetWindowText(L"Error: " + err);
    }
}

void CRpnCalculatorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_INPUT, m_editInput);
    DDX_Control(pDX, IDC_STATIC_RESULT, m_staticResult);
    DDX_Control(pDX, IDC_RADIO1, InfixButton);
    DDX_Control(pDX, IDC_RADIO2, RpnButton);
}

void CRpnCalculatorDlg::OnDestroy()
{
    CDialogEx::OnDestroy();
    UnregisterHotKey(m_hWnd, HOTKEY_ID);
}

void CRpnCalculatorDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(x, y, m_hIcon);
    }
    else
    {
        CDialogEx::OnPaint();
    }
}

BEGIN_MESSAGE_MAP(CRpnCalculatorDlg, CDialogEx)
    ON_MESSAGE(WM_HOTKEY, &CRpnCalculatorDlg::OnHotKey)
    ON_BN_CLICKED(IDC_BUTTON_CALC, &CRpnCalculatorDlg::OnBnClickedButtonCalc)
    ON_BN_CLICKED(IDC_RADIO1, &CRpnCalculatorDlg::OnBnClickedRadio1)
    ON_BN_CLICKED(IDC_BUTTON_HELP, &CRpnCalculatorDlg::OnBnClickedButtonHelp)
    ON_BN_CLICKED(IDCANCEL, &CRpnCalculatorDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

void CRpnCalculatorDlg::OnBnClickedButtonHelp()
{
    HINSTANCE result = ShellExecute(NULL, _T("open"),
        _T("https://www.example.com"),
        NULL, NULL, SW_SHOWNORMAL);
}

void CRpnCalculatorDlg::OnBnClickedRadio1()
{
    // TODO: Add your control notification handler code here
}


void CRpnCalculatorDlg::OnEnChangeEdit1()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialogEx::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}

void CRpnCalculatorDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    CDialogEx::OnCancel();
}
