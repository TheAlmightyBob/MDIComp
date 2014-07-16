/*
Responsible for the actual "Expose" feature, including
sorting out window placement and processing messages.
*/
#pragma once

#include "CompositionSurface.h"
#include "ComposedFrame.h"
#include <vector>

// Forward declarations
class CCapturedFrame;

class CExposer
{
public:
    CExposer(CMDIFrameWnd* pWnd, UINT uiExposeCommand, UINT uiConcealCommand);

    HRESULT Initialize();

    void Expose();

    void SetAnimDuration(double dDuration);

protected:
    template<class Func>
    void IterateFrames(Func F);

    CComPtr<IDCompositionDevice> GetDevice();
    CComPtr<IDCompositionVisual> GetContainer();

    void Conceal(); // immediately reverts, without animation
    void AnimateBack();

    CComposedFrame* HitTest(const CPoint& pt);
    void HighlightFrame(CComposedFrame* pFrame);
    void SelectFrame(CComposedFrame* pFrame);

    static LRESULT CALLBACK SubclassProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

    bool HandleMainWndMsg(UINT msg, WPARAM wParam, LPARAM lParam);
    bool HandleMdiWndMsg(UINT msg, WPARAM wParam, LPARAM lParam);

    void OnMdiNext(CWnd* pFrame, bool bNext);

    void ShowCurtain(CWnd* pWnd);
    void HideCurtain();

    void HandleDeviceLoss();

protected:
    CMDIFrameWnd* m_pMainWnd;

    bool m_bDirectCompInitialized;
    bool m_bExpose;
    bool m_bAnimating;

    double m_dDuration;

    CCompositionRoot m_compSurface;
    std::vector<CComposedFrame> m_vFrames;

    CComposedFrame* m_pHighlightedFrame;

    CCapturedFrame* m_pCurtain;

    UINT m_uiExposeCommand;
    UINT m_uiConcealCommand;

    UINT m_uiExposeTimer;
    UINT m_uiConcealTimer;
    UINT m_uiCurtainTimer;
};