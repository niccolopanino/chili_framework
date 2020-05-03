/******************************************************************************************
*	Chili DirectX Framework Version 16.07.20											  *
*	MainWindow.cpp																		  *
*	Copyright 2016 PlanetChili.net <http://www.planetchili.net>							  *
*																						  *
*	This file is part of The Chili DirectX Framework.									  *
*																						  *
*	The Chili DirectX Framework is free software: you can redistribute it and/or modify	  *
*	it under the terms of the GNU General Public License as published by				  *
*	the Free Software Foundation, either version 3 of the License, or					  *
*	(at your option) any later version.													  *
*																						  *
*	The Chili DirectX Framework is distributed in the hope that it will be useful,		  *
*	but WITHOUT ANY WARRANTY; without even the implied warranty of						  *
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the						  *
*	GNU General Public License for more details.										  *
*																						  *
*	You should have received a copy of the GNU General Public License					  *
*	along with The Chili DirectX Framework.  If not, see <http://www.gnu.org/licenses/>.  *
******************************************************************************************/
#include "MainWindow.h"
#include "Resource.h"
#include "Graphics.h"
#include "ChiliException.h"
#include "Game.h"
#include <assert.h>

MainWindow::MainWindow(HINSTANCE hInst, wchar_t *pArgs) : m_args(pArgs), m_hinst(hInst)
{
    // register window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX),CS_CLASSDC,handle_msg_setup,0,0,
        hInst,nullptr,nullptr,nullptr,nullptr,
        m_wnd_class_name,nullptr };
    wc.hIconSm = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0);
    wc.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    RegisterClassEx(&wc);

    // create window & get hWnd
    RECT wr;
    wr.left = 350;
    wr.right = Graphics::k_screen_width + wr.left;
    wr.top = 100;
    wr.bottom = Graphics::k_screen_height + wr.top;
    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE);
    m_hwnd = CreateWindow(m_wnd_class_name, L"Chili DirectX Framework",
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        wr.left, wr.top, wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInst, this);

    // throw exception if something went terribly wrong
    if (m_hwnd == nullptr)
    {
        throw Exception(_CRT_WIDE(__FILE__), __LINE__,
            L"Failed to get valid window handle.");
    }

    // show and update
    ShowWindow(m_hwnd, SW_SHOWDEFAULT);
    UpdateWindow(m_hwnd);
}

MainWindow::~MainWindow()
{
    // unregister window class
    UnregisterClass(m_wnd_class_name, m_hinst);
}

bool MainWindow::is_active() const
{
    return GetActiveWindow() == m_hwnd;
}

bool MainWindow::is_minimized() const
{
    return IsIconic(m_hwnd) != 0;
}

void MainWindow::show_message_box(const std::wstring &title, const std::wstring &message, UINT type) const
{
    MessageBox(m_hwnd, message.c_str(), title.c_str(), type);
}

void MainWindow::kill()
{
    PostQuitMessage(0);
}

bool MainWindow::process_message()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (msg.message == WM_QUIT)
            return false;
    }
    return true;
}

const std::wstring &MainWindow::get_args() const
{
    return m_args;
}

LRESULT WINAPI MainWindow::handle_msg_setup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // use create parameter passed in from CreateWindow() to store window class pointer at WinAPI side
    if (msg == WM_NCCREATE)
    {
        // extract ptr to window class from creation data
        const CREATESTRUCTW *const pCreate = reinterpret_cast<CREATESTRUCTW *>(lParam);
        MainWindow *const pWnd = reinterpret_cast<MainWindow *>(pCreate->lpCreateParams);
        // sanity check
        assert(pWnd != nullptr);
        // set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&MainWindow::handle_msg_thunk));
        // forward message to window class handler
        return pWnd->handle_msg(hWnd, msg, wParam, lParam);
    }
    // if we get a message before the WM_NCCREATE message, handle with default handler
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT WINAPI MainWindow::handle_msg_thunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // retrieve ptr to window class
    MainWindow *const pWnd = reinterpret_cast<MainWindow *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    // forward message to window class handler
    return pWnd->handle_msg(hWnd, msg, wParam, lParam);
}

LRESULT MainWindow::handle_msg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_KILLFOCUS:
            m_kbd.clear_state();
            break;

        // ************ KEYBOARD MESSAGES ************ //
        case WM_KEYDOWN:
            if (!(lParam & 0x40000000) || m_kbd.is_autorepeat_enabled()) // no thank you on the autorepeat
                m_kbd.on_key_pressed(static_cast<unsigned char>(wParam));
            break;
        case WM_KEYUP:
            m_kbd.on_key_released(static_cast<unsigned char>(wParam));
            break;
        case WM_CHAR:
            m_kbd.on_char(static_cast<unsigned char>(wParam));
            break;
        // ************ END KEYBOARD MESSAGES ************ //

        // ************ MOUSE MESSAGES ************ //
        case WM_MOUSEMOVE:
        {
            POINTS pt = MAKEPOINTS(lParam);
            if (pt.x > 0 && pt.x < Graphics::k_screen_width && pt.y > 0 && pt.y < Graphics::k_screen_height)
            {
                m_mouse.on_mouse_move(pt.x, pt.y);
                if (!m_mouse.is_in_window()) {
                    SetCapture(hWnd);
                    m_mouse.on_mouse_enter();
                }
            }
            else
            {
                if (wParam & (MK_LBUTTON | MK_RBUTTON))
                {
                    pt.x = std::max(short(0), pt.x);
                    pt.x = std::min(short(Graphics::k_screen_width - 1), pt.x);
                    pt.y = std::max(short(0), pt.y);
                    pt.y = std::min(short(Graphics::k_screen_height - 1), pt.y);
                    m_mouse.on_mouse_move(pt.x, pt.y);
                }
                else
                {
                    ReleaseCapture();
                    m_mouse.on_mouse_leave();
                    m_mouse.on_left_released(pt.x, pt.y);
                    m_mouse.on_right_released(pt.x, pt.y);
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            m_mouse.on_left_pressed(pt.x, pt.y);
            SetForegroundWindow(hWnd);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            m_mouse.on_right_pressed(pt.x, pt.y);
            break;
        }
        case WM_LBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            m_mouse.on_left_released(pt.x, pt.y);
            break;
        }
        case WM_RBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            m_mouse.on_right_released(pt.x, pt.y);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            if (GET_WHEEL_DELTA_WPARAM(wParam) > 0) {
                m_mouse.on_wheel_up(pt.x, pt.y);
            } else if (GET_WHEEL_DELTA_WPARAM(wParam) < 0) {
                m_mouse.on_wheel_down(pt.x, pt.y);
                break;
            }
        }
        // ************ END MOUSE MESSAGES ************ //

    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
