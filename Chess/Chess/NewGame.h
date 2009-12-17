#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CNewGame dialog

class CNewGame : public CDialog
{
	DECLARE_DYNAMIC(CNewGame)

public:
	CNewGame(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewGame();

// Dialog Data
	enum { IDD = IDD_NEWGAME };
protected:
	INT m_nSelectedEngine;
	INT m_nSelectedPly;

	virtual void OnOK();
	virtual BOOL OnInitDialog();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CListBox m_SearchEngineList;
	CSpinButtonCtrl m_SetPly;

public:
	inline INT GetSelectedEngine()const;
	inline INT GetSelectedPly()const;
};

inline INT CNewGame::GetSelectedEngine()const
{
	return m_nSelectedEngine;
}

inline INT CNewGame::GetSelectedPly()const
{
	return m_nSelectedPly;
}