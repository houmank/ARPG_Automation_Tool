#include "application.h"

wxIMPLEMENT_APP(Application);

bool Application::OnInit()
{
    // create MVC
    m_Model = new SextantRoller::Model();
    m_Controller = new SextantRoller::Controller(m_Model);
    m_View = new SextantRoller::View("Main Menu", wxDefaultPosition, wxSize(600, 500));

    // bind
    m_Model->addView(m_View);
    m_View->bindUnbind(static_cast<void*>(m_Controller));
    

    m_View->start();
    return true;
}

Application::Application() : m_Model(nullptr), m_View(nullptr), m_Controller(nullptr) 
{
#ifdef _DEBUG
    InitDebugWindow();
#endif
}

Application::~Application() {
    if(m_Model) delete m_Model;
    // if(m_View) delete m_View; // Wxwidget cleans up its own structures (view is wxFrame)
    if(m_Controller) delete m_Controller;

#ifdef _DEBUG
    DestroyDebugWindow();
#endif
}