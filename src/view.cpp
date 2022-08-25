#include "view.h"
#include "id.h"
#include "controller.h"
#include "wx/event.h"
#include <assert.h>

namespace SextantRoller
{
    View::View(const wxString &title, const wxPoint& pos, const wxSize& size)
        : wxFrame(nullptr, wxID_ANY, title, pos, size)
    {
        SetMinSize(wxSize(600, 500));

        wxMenu *menuFile = new wxMenu;
        menuFile->Append(ID::Start, "&Start\tCtrl-R",
                         "Starts attempting to roll sextants.");
        menuFile->Append(ID::OpenFile, "&Open Mod List\tCtrl-F",
                         "Select a new modlist to compare to when rolling mods.");
        menuFile->AppendSeparator();
        menuFile->Append(wxID_EXIT);

        wxMenu *menuHelp = new wxMenu;
        menuHelp->Append(ID::Instructions, "&Instructions\tCtrl-H", "How to get started.");
        menuHelp->Append(wxID_ABOUT);

        m_VoidstoneMenu = new wxMenu;
        m_VoidstoneMenu->AppendCheckItem(ID::VoidstoneChangePurple, "&Purple (top) Voidstone", "Selects the purple voidstone to be rolled.");
        m_VoidstoneMenu->FindItemByPosition(0)->Check(true); // default selection
        m_VoidstoneMenu->AppendCheckItem(ID::VoidstoneChangeGreen, "&Green (right) Voidstone", "Selects the green voidstone to be rolled.");
        m_VoidstoneMenu->AppendCheckItem(ID::VoidstoneChangeRed, "&Red (bottom) Voidstone", "Selects the red voidstone to be rolled.");
        m_VoidstoneMenu->AppendCheckItem(ID::VoidstoneChangeCyan, "&Cyan (left) Voidstone", "Selects the purple Cyan to be rolled.");


        wxMenuBar *menuBar = new wxMenuBar;
        menuBar->Append(menuFile, "&File");
        menuBar->Append(m_VoidstoneMenu, "&Voidstone selected");
        menuBar->Append(menuHelp, "&Help");

        SetMenuBar(menuBar);

        CreateStatusBar();
        SetStatusText("Welcome to POE Sextant Roller!");

        Bind(wxEVT_MENU, &View::OnStart, this, ID::Start);
        Bind(wxEVT_MENU, &View::OnFileOpen, this, ID::OpenFile);
        Bind(wxEVT_MENU, &View::OnInstructions, this, ID::Instructions);
        Bind(wxEVT_MENU, &View::OnAbout, this, wxID_ABOUT);
        Bind(wxEVT_MENU, &View::OnExit, this, wxID_EXIT);

        Bind(wxEVT_MENU, &View::OnVoidstoneChange, this, ID::VoidstoneChangePurple);
        Bind(wxEVT_MENU, &View::OnVoidstoneChange, this, ID::VoidstoneChangeGreen);
        Bind(wxEVT_MENU, &View::OnVoidstoneChange, this, ID::VoidstoneChangeRed);
        Bind(wxEVT_MENU, &View::OnVoidstoneChange, this, ID::VoidstoneChangeCyan);
    }

    void View::updateModList(std::vector<std::string>& modList) {
        if (m_StrTable->GetNumberRows() > 0)
            m_StrTable->DeleteRows(0, m_StrTable->GetNumberRows());

        if (modList.size() == 0) return;

        m_StrTable->AppendRows(modList.size());
        for (int i = 0; i < modList.size(); i++)
        {
            m_StrTable->SetCellValue(wxGridCellCoords(i, 0), modList[i]);
        }

    }

    void View::highlightMenuItem(int row)
    {
    }

    void View::initModList()
    {
        // setup wxGridStringTable
        int initModList = (static_cast<Controller*>(m_Controller))->signalModListSize();
        m_StrTable = new HwxGrid(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS, "Mod List");
        m_StrTable->CreateGrid(initModList, 1, wxGrid::wxGridSelectRows);

        // Set properties
        m_StrTable->EnableEditing(false);
        m_StrTable->DisableColResize(0);
        m_StrTable->EnableDragGridSize(false);
        m_StrTable->HideRowLabels();
        m_StrTable->SetColLabelAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

        // Column
        m_StrTable->SetColLabelValue(0, "Mod Name");
        m_StrTable->SetColStretch(0, 1);

        wxGridSizer* sizer = new wxGridSizer(0);
        sizer->Add(m_StrTable, 1, wxEXPAND | wxALL, 10);

        if (initModList <= 0) {

            (static_cast<Controller*>(m_Controller)->signalModListChange(PATH_TO_CONFIGS + "defaultModList.txt"));
            updateModList((static_cast<Controller*>(m_Controller))->signalGetModList());
        }
            
    }

    void View::start()
    {   
        initModList();
        this->Show(true);
    }

    void View::bindUnbind(void* controller) {
        static int boundStatus = 0;
        if(!boundStatus)
            m_Controller = controller;
        else
            m_Controller = nullptr;
        
        boundStatus = (boundStatus + 1) % 2;
    }

    void View::popup(const std::string& msg)
    {
        wxMessageBox(msg, "ERROR", wxICON_ERROR | wxOK);
    }

    
    void View::OnExit(wxCommandEvent &event)
    {
        Close(true);
        event.Skip();
    }

    void View::OnAbout(wxCommandEvent &event)
    {
        wxMessageBox("This is a simple game automation tool for rolling item modifiers.\nProgram by: github.com/houmank",
                     "Program Info", wxOK | wxICON_INFORMATION);
        event.Skip();
    }

    void View::OnFileOpen(wxCommandEvent &event)
    {
        wxFileDialog
            openFileDialog(this, _("Open new mod list"), wxEmptyString, wxEmptyString, 
                           "Text files (*.txt)|*.txt", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
        
        if(openFileDialog.ShowModal() == wxID_CANCEL) return; // user canceled fd
        static_cast<Controller*>(m_Controller)->signalModListChange(std::string(openFileDialog.GetPath()));
        event.Skip();
    }

    void View::OnInstructions(wxCommandEvent& event)
    {

    }

    void View::OnVoidstoneChange(wxCommandEvent& event)
    {
        constexpr int NUM_VOIDSTONES = 4;
        for (int i = 0; i < NUM_VOIDSTONES; ++i) 
        {
            wxMenuItem* voidstone = m_VoidstoneMenu->FindItemByPosition(i);
            if (voidstone->GetId() == event.GetId())
            {
                voidstone->Check(true);
                static_cast<Controller*>(m_Controller)->signalVoidstoneSelection(voidstone->GetId() - ID::VoidstoneChangePurple);
            }
            else
                voidstone->Check(false);
        }
    }

    void View::OnStart(wxCommandEvent &event)
    {
        // Call controller to handle rolling through modifiers
        // TODO: Play sound
        static_cast<Controller*>(m_Controller)->startRollerThread();
        event.Skip();
    }
}