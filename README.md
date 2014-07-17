MDI Composer
=======

A library to enhance classic MDI applications with Windows 8 DirectComposition.

This is intended for old-fashioned non-tabbed MDI applications, where managing many open windows can be a bit of a pain. Windows 8 support for layered child windows and DirectComposition are leveraged to display all open windows for selection. (based on a similar feature in Compiz)

Normal:
![before mdicomp](https://cloud.githubusercontent.com/assets/7707864/3594849/2e30edbe-0ca8-11e4-9268-5035001dd455.PNG)

Composed & Exposed:
![after mdicomp](https://cloud.githubusercontent.com/assets/7707864/3594848/2817f706-0ca8-11e4-8bd0-e3643b78e5d3.png)


This is *not* intended as a general purpose DirectComposition library. For that, check out Kenny Kerr's http://moderncpp.com

Usage Notes:
- Include MDIComp.h
- Define your own command IDs for exposing/restoring windows, hook them up to your preferred accelerators/menus/etc, and pass them to the constructor.
- The Expose command is left to the application to handle. MDIComp just needs to know because it generally assumes it should cancel if another command is invoked.
- The Cancel command is handled by MDIComp.
- Call Initialize after the window has been created.
- Setting animation time is optional. Default is 300ms.

Compatibility Notes:
- This will convert child MDI frames, and sometimes their views, to layered windows via WS_EX_LAYERED and SetLayeredWindowAttributes, relying on simple automatic redirection to render the DirectComposition visuals. It would likely work better with windows that use UpdateLayeredWindow, but my goal was to just plug into existing applications. 
- It is designed to work with MFC apps, although it would be fairly trivial to remove that requirement.

Known Limitations:
- Child windows that are larger than the main window will be clipped. This is due to how the redirection works. If the original window is clipped, so too is the DirectComposition visual. (all windows are temporarily moved to 0,0 to minimize clipping)
- There is some flicker when exposing from maximized windows and when exposing a window for the first time. This is because in general there is flicker anytime layering is toggled. Maximized windows need to have their views layered for exposure (since their frame is invisible... using that looks strange), but leaving them layered causes problems with operations like Ctrl+Tab and closing windows.
- In general, maximized child windows do not like being layered. I worked around some other issues with the frame by using a DirectComposition visual to cover up changes. It is not ideal...
- Minimized windows are not fully supported.
