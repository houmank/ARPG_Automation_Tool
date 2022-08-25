#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "model.h"

namespace SextantRoller
{
    class Controller
    {
    public:
        Controller(Model* m);
        void startRollerThread();
        void signalModListChange(const std::string& fp);
        void signalVoidstoneSelection(const int voidstoneIndex);
        int signalModListSize();
        std::vector<std::string>& signalGetModList();

    private:
        Model* m_Model = nullptr;
    };
}

#endif //CONTROLER_H