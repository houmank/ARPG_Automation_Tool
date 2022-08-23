#ifndef MODEL_H
#define MODEL_H

#include <unordered_set>
#include <string>
#include <fstream>
#include <iostream>

#include "view.h"

const std::string PATH_TO_CONFIGS = "../../src/res/";

namespace SextantRoller
{
    class Model
    {
    public:
        Model(const std::string &fp = PATH_TO_CONFIGS + "defaultModList.txt");
        ~Model();

        void loadNewModifiers(const std::string& fp);
        std::vector<std::string>& getListOfMods();
        int getModListSize();
        void AttemptSextantRoll();

        void addView(View* view);
        void removeView(View* view);

    private:
        View* m_View;
        std::unordered_set<std::string> m_ModifierSet;
        std::vector<std::string> m_ModOrder;
    };
}

#endif // MODEL_H