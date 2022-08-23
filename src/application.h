#ifndef APPLICATION_H
#define APPLICATION_H

#ifndef WIN32
    static_assert(false, "OTHER PLATFORMS NOT SUPPORTED!");
#endif

#ifdef _DEBUG
    #include <iostream>
    #include <cstdio>
    #define PrintDebug(stuff) std::cout << stuff << "\n"
    #define InputFromDebug(target) std::cin >> target
    #define InitDebugWindow() AllocConsole(); \
                                  SetConsoleTitle(wxT("Debug Window")); \
                                  freopen("conin$", "r", stdin); \
                                  freopen("conout$", "w", stdout); \
                                  freopen("conout$", "w", stderr); \
                                  PrintDebug("Debug Window Initialized");
    #define DestroyDebugWindow() FreeConsole();
#else
    #define PrintDebug(stuff)
    #define InputFromDebug(target)
    #define InitDebugWindow()
    #define DestroyDebugWindow()
#endif 

#include "wx/wx.h"
#include "controller.h"

class Application : public wxApp 
{
public:
    Application();
    ~Application();

    virtual bool OnInit() override;
private:
    SextantRoller::Model* m_Model;
    SextantRoller::View* m_View;
    SextantRoller::Controller* m_Controller;
};

#endif //APPLICATION_H