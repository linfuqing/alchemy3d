// ChessDoc.h : CChessDoc ��Ľӿ�
//


#pragma once


class CChessDoc : public CDocument
{
protected: // �������л�����
	CChessDoc();
	DECLARE_DYNCREATE(CChessDoc)

// ����
public:

// ����
public:

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// ʵ��
public:
	virtual ~CChessDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
};


