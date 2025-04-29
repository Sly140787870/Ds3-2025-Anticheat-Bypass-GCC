#include "WPHook.h"
#include "Funcs.h"
#include "BanBypass.h"
#include "Mem.h"
LRESULT NewWindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam) 
{
    switch(uMsg)
	{
        case WM_KEYDOWN:
			if(GetKeyState(VK_LSHIFT) < 0)  //https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes 0xA0 -> LEFT SHIFT
			{ 
				if(wParam==0x70)
				{
					int bulletArray[] = { 6, 7, 8, 63, 113, 132, 9100, 9116, 5030231, 5030700, 13720300, 100063240, 100110000, 100160900};
					for (auto nx : bulletArray) 
					{
						ModifyBullet(nx, 0x10, int, 1);
						if (nx == 113)
						{
							ModifyBullet(nx, 0x48, float, 1);
							ModifyBullet(nx, 0x4C, float, 0.5);
						}
						__imp_wprintf(L"[INFO] Modified bullet [%d] \n", nx);
					}
				}
				break;
			}
        case WM_KEYUP:
			break;
    }
    return CallWindowProcW(OldWindowProc,hwnd,uMsg,wParam,lParam);
}

void WPHook() 
{
    static bool done;
    if(!done)
	{
        HWND hwnd=FindWindowW(0, L"DARK SOULS III");
		DWORD pid;
        if(hwnd&&GetWindowThreadProcessId(hwnd,&pid)&&pid==GetCurrentProcessId()&&GetWindowLongPtrW(hwnd,GWLP_WNDPROC)!=(LONG_PTR)NewWindowProc)
		{
            done=true;
            OldWindowProc=(WNDPROC)SetWindowLongPtrW(hwnd,GWLP_WNDPROC,(LONG_PTR)NewWindowProc);
        }
    }
}
