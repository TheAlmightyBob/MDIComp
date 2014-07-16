#include "stdafx.h"
#include "MDIComp.h"

#include "Exposer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CMDIComp::CMDIComp(CMDIFrameWnd* pWnd, UINT uiExposeCommand, UINT uiConcealCommand)
    : m_pExposer(new CExposer(pWnd, uiExposeCommand, uiConcealCommand))
{
}

// unique_ptr + forward declared type, incompatible with automatic destructor
CMDIComp::~CMDIComp()
{
}

HRESULT CMDIComp::Initialize()
{
    return m_pExposer->Initialize();
}

void CMDIComp::SetAnimDuration(double dDuration)
{
    m_pExposer->SetAnimDuration(dDuration);
}

void CMDIComp::Expose()
{
    m_pExposer->Expose();
}