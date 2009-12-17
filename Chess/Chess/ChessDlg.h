// ChessDlg.h : ͷ�ļ�
//

#pragma once
//#include "NegamaxEngine.h"
//#include "ChessMoveGenerator.h"
//#include "ChessEveluation.h"
//#include "AlphaBetaEngine.h"
#include "afxwin.h"

typedef struct
{
	CHESSMANTYPE Type;
	POINT        From;
	POINT        To;
}MOVECHESS;

// CChessDlg �Ի���
class CChessDlg : public CDialog
{
// ����
public:
	CChessDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_CHESS_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��

protected:

// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	BASECHESSBOARD m_ChessBoard;
	BASECHESSBOARD m_BackupChessBoard;

	MOVECHESS  m_MoveChess;
	CBitmap    m_BoardBitmap;
	CImageList m_Chessman;

	INT        m_nBoardWidth;
	INT        m_nBoardHeight;

	CSearchEngine CHESS* m_pSearchEngine;
public:
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	
	afx_msg void OnNewGame();
	CStatic m_OutputInfo;
};
