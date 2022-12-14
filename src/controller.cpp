#include "controller.h"
#include <thread>
#include <filesystem>

namespace SextantRoller
{
    Controller::Controller(Model* m)  :
            m_Model(m)
    {}

    void Controller::startRollerThread()
    {
        std::thread worker(std::function<void()>(std::bind(&Model::AttemptSextantRoll, m_Model)));
        worker.detach();
    }

    void Controller::signalModListChange(const std::string& fp) 
    {
        PrintDebug(std::filesystem::current_path());
        PrintDebug("[STATUS]: Loading new modlist: " + fp + '\n');
        m_Model->loadNewModifiers(fp);
    }

    void Controller::signalVoidstoneSelection(const int voidstoneIndex)
    {
        m_Model->setVoidstoneSelection(voidstoneIndex);
    }

    int Controller::signalModListSize()
    {
        if(m_Model)
            return m_Model->getModListSize();
        return 0;
    }

    std::vector<std::string>& Controller::signalGetModList()
    {
        return m_Model->getListOfMods();
    }  
}