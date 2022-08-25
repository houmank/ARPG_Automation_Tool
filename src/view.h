#ifndef VIEW_H
#define VIEW_H

#include "wx/wx.h"
#include "hgrid.h"

namespace SextantRoller 
{
    class View : public wxFrame
    {
    public:
        View(const wxString &title, const wxPoint& pos, const wxSize& size);

        void updateModList(std::vector<std::string>& modList);
        void highlightMenuItem(int row);
        void initModList();
        void start();
        void bindUnbind(void* controller);
        void popup(const std::string& msg);

    private:
        void OnExit(wxCommandEvent &event);
        void OnAbout(wxCommandEvent &event);
        void OnFileOpen(wxCommandEvent &event);
        void OnInstructions(wxCommandEvent& event);
        void OnVoidstoneChange(wxCommandEvent& event);
        void OnStart(wxCommandEvent &event);

        void* m_Controller;
        HwxGrid* m_StrTable;
        wxMenu* m_VoidstoneMenu;
    };
}

#endif // VIEW_H