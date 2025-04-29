#include "Framework.h"
WNDPROC OldWindowProc;
LRESULT NewWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
void WPHook();