#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <atlhost.h>

#include <windows.h>
#include <scrnsave.h>
#include "resource.h"

/* Need to link Scrnsave.lib comctl32.lib in project settings */

CComModule _Module;
BEGIN_OBJECT_MAP(ObjectMap)
END_OBJECT_MAP()

#define DEFAULT_URL    "www.google.com"

BOOL GetConfig(LPCTSTR lpValue, TCHAR *lpData, TCHAR *lpDefualtData, DWORD len)
{
    DWORD dwDataSize = len;
    HKEY hKey = NULL;

    RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\HtmlScrnSaver\\Config", 0, KEY_QUERY_VALUE, &hKey);

    if (hKey != NULL)
    {
        RegQueryValueEx(hKey, lpValue, NULL, NULL, (unsigned char *)lpData, &dwDataSize);
        RegCloseKey(hKey);
        return TRUE;
    }

    memcpy(lpData, lpDefualtData, len);
    return FALSE;
}

BOOL SetConfig(LPCTSTR lpValue, TCHAR *lpData)
{
    DWORD dw;
    HKEY hKey = NULL;

    RegCreateKeyEx(HKEY_CURRENT_USER, "Software\\HtmlScrnSaver\\Config", 0, NULL, 0, KEY_WRITE|KEY_READ, NULL, &hKey, &dw);

    if (hKey != NULL)
    {
        RegSetValueEx(hKey, lpValue, NULL, REG_SZ, (BYTE *)lpData, strlen((char *)lpData));
        RegCloseKey(hKey);
        return TRUE;
    }

    return FALSE;
}

LRESULT WINAPI ScreenSaverProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hBrowser;
    static CComQIPtr<IWebBrowser2> pWB;
    
    switch (message)
    {
        case WM_CREATE:
        {
            RECT rc;
            CComPtr<IUnknown> pUnkWB;

            GetClientRect(hWnd, &rc);

            TCHAR currUrl[MAX_PATH +1] = {0};
            GetConfig("URL", currUrl, DEFAULT_URL, MAX_PATH);
            CComVariant vUrl(currUrl);
            
            _Module.Init(ObjectMap, hMainInstance);
            AtlAxWinInit();
            void *init = GetProcAddress(LoadLibrary("atl"), "AtlAxWinInit");
            _asm call init;

			ShowCursor(0);

            hBrowser = CreateWindow(
                "AtlAxWin",
                "Shell.Explorer.2",
                WS_CHILD | WS_VISIBLE,
                rc.left,
                rc.top,
                rc.right - rc.left,
                rc.bottom - rc.top,
                hWnd,
                (HMENU)0,
                ::GetModuleHandle(NULL),
                NULL
            );
            
            if (S_OK == AtlAxGetControl(hBrowser, &pUnkWB))
            {
                pWB = pUnkWB;
                if (pWB)
                {
                    pWB->Navigate2(&vUrl, 0, 0, 0, 0);
                }
            }
        }
        break;

        case WM_DESTROY:
            pWB.Release();
            break;
    }

    // DefScreenSaverProc processes any messages ignored by ScreenSaverProc
    return DefScreenSaverProc(hWnd, message, wParam, lParam);
}

BOOL WINAPI ScreenSaverConfigureDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    TCHAR currUrl[MAX_PATH + 1] = {0};

    switch(message)
    {
        case WM_INITDIALOG:
        {
            GetConfig("URL", currUrl, DEFAULT_URL, MAX_PATH);
            SetDlgItemText(hDlg, IDC_EDIT_URL, currUrl);
        }
        return TRUE;
        
        case WM_COMMAND:
        switch(LOWORD(wParam))
        {            
            case IDOK:
                GetDlgItemText(hDlg, IDC_EDIT_URL, currUrl, MAX_PATH);
                SetConfig("URL", currUrl);
                EndDialog(hDlg, IDOK);
                return TRUE;
            
            case IDCANCEL:
                EndDialog(hDlg, IDCANCEL);
                return TRUE;

            case IDC_BUTTON_SELECT:
                OPENFILENAME ofn = {0};
                ofn.lStructSize = sizeof(OPENFILENAME);
                ofn.hwndOwner = hDlg;
                ofn.lpstrFilter = "html(*.htm;*html)\0*.htm;*html\0\0";
                ofn.lpstrFile = currUrl;
                ofn.nMaxFile = sizeof(currUrl);
                ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;
                if (GetOpenFileName(&ofn))
                {
                    SetDlgItemText(hDlg, IDC_EDIT_URL, currUrl);
                }
                return TRUE;
        }
        break;
    }

    return FALSE;
}

BOOL WINAPI RegisterDialogClasses(HANDLE hInst)
{
    return TRUE;
}