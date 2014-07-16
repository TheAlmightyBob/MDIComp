// This is the main header for inclusion by client applications.
// 
// Usage notes:
// - Define your own command IDs, hook them up to your preferred
//   accelerators/menus/etc, and pass them to the constructor.
// - The Expose command is left to the application to handle. MDIComp
//   just needs to know because it generally assumes it should cancel
//   if another command is invoked.
// - The Cancel command is handled by MDIComp.
// - Call Initialize after the window has been created.
// - Setting animation time is optional. Default is 300ms.

#pragma once

#include <memory>

// Forward declarations
class CExposer;

class CMDIComp
{
public:
    CMDIComp(CMDIFrameWnd* pWnd, UINT uiExposeCommand, UINT uiConcealCommand);
    ~CMDIComp();

    HRESULT Initialize();

    void Expose();

    void SetAnimDuration(double dDuration);

protected:
    std::unique_ptr<CExposer> m_pExposer;
};