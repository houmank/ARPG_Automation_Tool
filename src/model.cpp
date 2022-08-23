#include "model.h"
#include "sextant_roller_cv.h"
#include "sim_input.h"

namespace SextantRoller 
{
    // MODEL CLASS

    Model::Model(const std::string &fp)
        : m_View(nullptr),
          m_ModifierSet{},
          m_ModOrder{}
          {}

    Model::~Model() {}

    void Model::loadNewModifiers(const std::string &fp)
    {
        if(!m_View)
            return;
        
        std::ifstream file(fp);
        if(file.is_open())
        {
            m_ModifierSet.clear();
            while(file.good())
            {
                std::string mod;
                std::getline(file, mod);
                m_ModOrder.push_back(mod);
                m_ModifierSet.insert(std::move(mod));
            }
        }
        
        m_View->updateModList(getListOfMods());
    }

    std::vector<std::string>& Model::getListOfMods()
    {
        return m_ModOrder;
    }

    int Model::getModListSize()
    {
        return m_ModifierSet.size();
    }

    void Model::AttemptSextantRoll()
    {
        // Get POE window
        LPCWSTR classWString = L"POEWindowClass";
        LPCWSTR windowWString = L"Path of Exile";
        HWND hWND = FindWindow(classWString, windowWString);

        if (hWND == NULL)
        {
            m_View->popup("Couldn't find game, Start game and try again.");
            return;
        }

        // Signal user to tab into game
        m_View->popup("Tab into the game and press numpad 0 to start. To stop press numpad 1");
        while(!GetAsyncKeyState(VK_NUMPAD0));

        // Get img matrix
        cv::Mat screenshot = CV::bitmapToMat(hWND);
        if(screenshot.rows == 0 && screenshot.cols == 0) {
            m_View->popup("Could not see POE, make sure to have the window maximized.");
            return;
        }

        // Ensure inventory and atlas are open
        bool atlasOpen = CV::CheckAtlas(screenshot);
        if(atlasOpen == true)
        {
            Input::InputHandler::sendKeyInput('g');
            Input::InputHandler::sendKeyInput('g');
        }
        else
        {
            Input::InputHandler::sendKeyInput('g');
        }
        Input::InputHandler::sendKeyInput('i');
            
        // template match inventory items
        CV::template_match matchesEmpty = CV::getInvItems("empty_inv_cell.png", screenshot, 1);
        CV::template_match matchesCompass = CV::getInvItems("compass.png", screenshot, 1);
        CV::template_match matchesSextant = CV::getInvItems("awakened_sextant.png", screenshot, 1);
        

        CV::drawRectOverMatches(screenshot, matchesSextant, CV_RGB(0, 255, 0), 2);
        CV::drawRectOverMatches(screenshot, matchesEmpty, CV_RGB(0, 150, 255), 2);
        CV::drawRectOverMatches(screenshot, matchesCompass, CV_RGB(255, 0, 0), 2);

        cv::imshow("output", screenshot);
        cv::waitKey(0);
    }

    void Model::addView(View *view) {
        m_View = view;
    }

    void Model::removeView(View *view) {
        m_View = nullptr;
    }
}
