#include "model.h"
#include "sextant_roller_cv.h"
#include "sim_input.h"
#include "sextant_roller_constants.h"
#include "wx/clipbrd.h"

#include <algorithm>

namespace SextantRoller 
{
    std::vector<std::string> parseModsFromItemDesc(const std::string& item)
    {
        /* Example of item description
        Item Class: Atlas Upgrade Items
        Rarity: Normal
        Omniscient Voidstone
        --------
        Item Level: 1
        --------
        Your Maps contain 25 additional Clusters of Mysterious Barrels (enchant)
        3 uses remaining (enchant)
        --------
        Maps Dropped in Areas have 25% chance to be 1 tier higher
        --------
        The Searing Exarch spread the Word of
        Enlightenment for countless eons without
        ever understanding its master's message.
        --------
        Socket this into your Atlas to increase the Tier of all Maps.
        */
        std::vector<std::string> mods;
        size_t index = std::string::npos;
        constexpr std::string_view FIRST_IDENTIFIER = "Item Class";
        constexpr size_t LEN_OF_FIRST_IDENTIFIER = 10;
        constexpr size_t NUM_CATEGORY_TO_SKIP_REVERSE = 3;
        constexpr size_t NUM_DASHES = 8;
        constexpr size_t NUM_CHARS_PAST_DASH_TO_NEWLINE = 1;
        constexpr size_t NUM_CHARS_REMOVED_ENCHANT = 10;

        // verify it is an item description
        if (item.substr(0, FIRST_IDENTIFIER.size()) != FIRST_IDENTIFIER) {
            mods.emplace_back(FIRST_IDENTIFIER);
            return mods;
        }

        for(int i = 0; i < NUM_CATEGORY_TO_SKIP_REVERSE; i++)
        { 
            index = item.rfind('-', index);
            if(index == std::string::npos) return mods;
            index -= NUM_DASHES;
        }

        size_t startMods = item.rfind('-', index) + NUM_CHARS_PAST_DASH_TO_NEWLINE;
        size_t next = startMods + 1;
        while(next <= index) {
            size_t newLine = item.find('\n', next);
            std::string mod = item.substr(next, newLine - next - NUM_CHARS_REMOVED_ENCHANT);
            std::transform(mod.begin() , mod.end(), mod.begin(), ::tolower);
            std::cout << "mod grabbed from clip: " << mod << '\n';
            mods.emplace_back(mod);
            next = newLine + 1;
        }

        return mods;
    }

    std::vector<std::string> getModsFromClipboard()
    {
        std::string itemDescription;
        bool copied = false;

        if (wxTheClipboard->Open())
        {
            if (wxTheClipboard->IsSupported(wxDF_TEXT))
            {
                wxTextDataObject data;
                wxTheClipboard->GetData(data);
                itemDescription.reserve(data.GetDataSize());
                itemDescription = data.GetText();  
                copied = true;
            }
            wxTheClipboard->Close();
            wxTheClipboard->Clear();
        }

        if(!copied) return std::vector<std::string>();

        // parse mods from item description
        return parseModsFromItemDesc(itemDescription);
    }

    void grabObjFromInvAndClickVoidstone(const cv::Point& srcPoint, const cv::Point& destPoint, bool useShiftClick)
    {
        std::cout << "SRC LOC: " << srcPoint << '\n';
        std::cout << "DEST LOC: " << destPoint << '\n';
        Input::InputHandler::moveMouse(srcPoint.x, srcPoint.y);
        Input::InputHandler::clickMouse(false);
        Input::InputHandler::moveMouse(destPoint.x, destPoint.y);

        if(useShiftClick) Input::InputHandler::keyReleaseOrHold(VK_SHIFT, true);
        Input::InputHandler::clickMouse(true);
        if(useShiftClick) Input::InputHandler::keyReleaseOrHold(VK_SHIFT, false);
    }

    cv::Point templateMatchCenter(const cv::Point& topLeft, const CV::template_match& match) {
        cv::Point res(topLeft.x + (match.m_TemplateImg.cols / 2), 
               topLeft.y + (match.m_TemplateImg.rows / 2));
        return res;                                                
    }


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
                std::transform(mod.begin(), mod.end(), mod.begin(), ::tolower);
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
        RECT wRect;
        GetWindowRect(hWND, &wRect);

        // Signal user to tab into game
        m_View->popup("Tab into the game and press numpad 0 to start.");
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

        // Zoom in to atlas badge
        int screen_x = GetSystemMetrics(SM_CXSCREEN);
        int screen_y = GetSystemMetrics(SM_CYSCREEN);
        int voidstone_x = wRect.left + ((double)(wRect.right - wRect.left)*VOIDSTONE_MIDDLE_X_RATIO);
        int voidstone_y = wRect.top + ((double)(wRect.bottom - wRect.top)*VOIDSTONE_BOTTOM_Y_RATIO);
        Input::InputHandler::moveMouse(voidstone_x, voidstone_y);
        Input::InputHandler::scrollUp(5);

        // Get new screensho w/ voidstones and inventory open
        screenshot = CV::bitmapToMat(hWND);

        // template match inventory items
        CV::template_match matchesEmpty = CV::getInvItems("empty_inv_cell.png", screenshot, 1);
        CV::template_match matchesCompass = CV::getInvItems("compass.png", screenshot, 1);
        CV::template_match matchesSextant = CV::getInvItems("awakened_sextant.png", screenshot, 1);

        // detect watchstones
        CV::template_match matchesVoidstones = CV::getVoidstones("voidstones.png", screenshot);

        CV::drawRectOverMatches(screenshot, matchesSextant, CV_RGB(0, 255, 0), 2);
        CV::drawRectOverMatches(screenshot, matchesEmpty, CV_RGB(0, 150, 255), 2);
        CV::drawRectOverMatches(screenshot, matchesCompass, CV_RGB(255, 0, 0), 2);
        CV::drawRectOverMatches(screenshot, matchesVoidstones, CV_RGB(200, 100, 100), 2);
        
        // get each voidstone's center pixel location relative to template match as ratio
        std::vector<cv::Point> voidstoneCenters = CV::getEachVoidstoneCenterPixel(matchesVoidstones);

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
