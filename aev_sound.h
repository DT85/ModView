#if !defined(AFX_AEVSOUND_H__E76C376A_6584_419C_A9E7_A49BF2A18494__INCLUDED_)
#define AFX_AEVSOUND_H__E76C376A_6584_419C_A9E7_A49BF2A18494__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CAEV_Sound dialog

class CAEVSound : public CDialog
{
// Construction
public:
	CAEVSound(LPCSTR pPrompt, CString *pFeedback, LPCSTR psDefault = NULL, CWnd* pParent = NULL);

// Dialog Data
	//{{AFX_DATA(CAEVSound)
	enum { IDD = IDD_DIALOG_GETSTRING };
	CString	m_strEditBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAEVSound)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString *m_pFeedback;
	LPCSTR	 m_pPrompt;
	LPCSTR	 m_pDefault;

	// Generated message map functions
	//{{AFX_MSG(CAEVSound)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // AFX_AEVSOUND_H__E76C376A_6584_419C_A9E7_A49BF2A18494__INCLUDED_
