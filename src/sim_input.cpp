#include "sim_input.h"

#include "WinUser.h"
#include <thread>
#include <chrono>

namespace SextantRoller
{
namespace Input
{
    void setupMouseInput(INPUT& buffer)
    {
        buffer.type = INPUT_MOUSE;
        buffer.mi.dx = (0 * (0xFFFF / GetSystemMetrics(SM_CXSCREEN)));
        buffer.mi.dy = (0 * (0xFFFF / GetSystemMetrics(SM_CXSCREEN)));
        buffer.mi.mouseData = 0;
        buffer.mi.dwFlags = MOUSEEVENTF_ABSOLUTE; // | MOUSEEVENTF_VIRTUALDESK;
        buffer.mi.time = 0;
        buffer.mi.dwExtraInfo = 0;
    }

    // https://stackoverflow.com/questions/49026921/sendinput-always-moves-mouse-pointer-to-left-top-corner
    POINT normalizePointToScreen(const POINT& pt_in_px, const RECT& display_size_in_px)
    {
        POINT pt_normalized;
        const auto width_in_px{display_size_in_px.right - display_size_in_px.left};
        const auto height_in_px{display_size_in_px.bottom - display_size_in_px.top};
        pt_normalized.x = ::MulDiv(pt_in_px.x, 0xffff, width_in_px);
        pt_normalized.y = ::MulDiv(pt_in_px.y, 0xffff, height_in_px);

        return pt_normalized;
    }

    void InputHandler::sendKeyInput(const char inputKey)
    {
        INPUT input = {0};
        SHORT keyCode = VkKeyScan(inputKey);
        UINT mappedKey = MapVirtualKey(LOBYTE(keyCode), 0);

        input.type = INPUT_KEYBOARD;
        input.ki.dwFlags = KEYEVENTF_SCANCODE;
        input.ki.wScan = mappedKey;

        SendInput(1, &input, sizeof(input));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
        SendInput(1, &input, sizeof(input));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // if press is true, it will only send KEY_DOWN flag, else it will release
    void InputHandler::keyReleaseOrHold(DWORD virtualKey, bool press)
    {
        INPUT input = {0};

        input.type = INPUT_KEYBOARD;
        input.ki.wVk = virtualKey;
        
        if(press)
        {
            input.ki.dwFlags = KEYEVENTF_SCANCODE;
            SendInput(1, &input, sizeof(input));
        }
        else
        {
            input.ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;
            SendInput(1, &input, sizeof(input));
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void InputHandler::clickMouse(bool leftClick)
    {
        INPUT mouseInput;
        setupMouseInput(mouseInput);

        DWORD mouseButtonFlag = (leftClick) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
        DWORD defaultFlags = mouseInput.mi.dwFlags;

        // press mouse button down
        mouseInput.mi.dwFlags |= mouseButtonFlag;
        SendInput(1, &mouseInput, sizeof(mouseInput));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        // release mouse button
        mouseInput.mi.dwFlags = defaultFlags | (mouseButtonFlag << 1);
        SendInput(1, &mouseInput, sizeof(mouseInput));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void InputHandler::moveMouse(int x, int y)
    {
        INPUT mouseInput;
        setupMouseInput(mouseInput);
        mouseInput.mi.dwFlags |= MOUSEEVENTF_MOVE;

        RECT rect;
        rect.left = 0; rect.top = 0;
        rect.right = GetSystemMetrics(SM_CXSCREEN);
        rect.bottom = GetSystemMetrics(SM_CYSCREEN);

        POINT p; p.x = x; p.y = y;
        POINT normalized = normalizePointToScreen(p, rect);
        
        mouseInput.mi.dx = normalized.x;
        mouseInput.mi.dy = normalized.y;
        SendInput(1, &mouseInput, sizeof(mouseInput));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // input i is how many wheel clicks to scroll up
    void InputHandler::scrollUp(int i)
    {
        INPUT mouseInput;
        setupMouseInput(mouseInput);

        // how many scrolls
        mouseInput.mi.dwFlags |= MOUSEEVENTF_WHEEL;
        mouseInput.mi.mouseData = WHEEL_DELTA * i;
        SendInput(1, &mouseInput, sizeof(mouseInput));

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void InputHandler::cntrlC()
    {
        INPUT input[4];
        memset(input, 0, 4 * sizeof(input[0]));

        input[0].type = INPUT_KEYBOARD;
        input[1].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_LCONTROL;
        input[1].ki.wVk = towupper('c');
        input[2] = input[1];
        input[3] = input[0];
        input[2].ki.dwFlags = KEYEVENTF_KEYUP;
        input[3].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(4, input, sizeof(input[0]));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void InputHandler::cntrlShiftC()
    {
        INPUT input[5];
        memset(input, 0, 5 * sizeof(input[0]));

        input[0].type = INPUT_KEYBOARD;
        input[1].type = INPUT_KEYBOARD;
        input[2].type = INPUT_KEYBOARD;
        input[0].ki.wVk = VK_LCONTROL;
        input[1].ki.wVk = VK_SHIFT;
        input[2].ki.wVk = towupper('c');

        input[3] = input[0];
        input[4] = input[2];
        input[3].ki.dwFlags = KEYEVENTF_KEYUP;
        input[4].ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput(5, input, sizeof(input[0]));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    void InputHandler::clickAndDrag()
    {
        assert(false);
    }

    void InputHandler::shiftLeftClick()
    {
        INPUT input[3];
        memset(input, 0, 3 * sizeof(input[0]));

        input[0].type = INPUT_KEYBOARD;
        input[1].type = INPUT_MOUSE;
        input[0].ki.wVk = VK_LSHIFT;
        input[1].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN;
        input[2] = input[1];
        input[2].ki.dwFlags =  MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTUP;
        SendInput(3, input, sizeof(input[0]));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
}