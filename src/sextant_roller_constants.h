#ifndef SEXTANT_ROLLER_CONSTANTS_H
#define SEXTANT_ROLLER_CONSTANTS_H

namespace SextantRoller
{
    // Constants
    const std::string PATH_TO_IMGS = "../../img/";
    const std::string PATH_TO_CONFIGS = "../../src/res/";
    constexpr int DEFAULT_RESOLUTION_X = 1920;
    constexpr int DEFAULT_RESOLUTION_Y = 1080;

    constexpr double INV_TOP_LEFT_X_RATIO = 0.6508;
    constexpr double INV_TOP_LEFT_Y_RATIO = 0.5270;

    constexpr double BADGE_LEFT_X_RATIO = 670.0 / DEFAULT_RESOLUTION_X;
    constexpr double BADGE_RIGHT_X_RATIO = 1255.0 / DEFAULT_RESOLUTION_X;
    constexpr double BADGE_BOTTOM_Y_RATIO = 100.0 / DEFAULT_RESOLUTION_Y;

    constexpr double VOIDSTONE_LEFT_X_RATIO = 740.0 / DEFAULT_RESOLUTION_X;
    constexpr double VOIDSTONE_TOP_Y_RATIO = 625.0 / DEFAULT_RESOLUTION_Y;
    constexpr double VOIDSTONE_MIDDLE_X_RATIO = 980.0 / DEFAULT_RESOLUTION_X;
    constexpr double VOIDSTONE_BOTTOM_Y_RATIO = 1015.0 / DEFAULT_RESOLUTION_Y;
    constexpr double VOIDSTONE_RIGHT_X_RATIO = 1230.0 / DEFAULT_RESOLUTION_X;

    // If template voidstone image is 144 x 132 pixels
    constexpr double PURPLE_VOIDSTONE_RELATIVE_X_RATIO = (double)(985 - 906) / 144;
    constexpr double CYAN_VOIDSTONE_RELATIVE_X_RATIO = (double)(949 - 906) / 144; 
    constexpr double GREEN_VOIDSTONE_RELATIVE_X_RATIO = (double)(1024 - 906) / 144; 
    constexpr double RED_VOIDSTONE_RELATIVE_X_RATIO = (double)(985 - 906) / 144;

    constexpr double PURPLE_VOIDSTONE_RELATIVE_Y_RATIO = (double)(835 - 805) / 132;
    constexpr double CYAN_VOIDSTONE_RELATIVE_Y_RATIO = (double)(869 - 805) / 132; 
    constexpr double GREEN_VOIDSTONE_RELATIVE_Y_RATIO = (double)(869 - 805) / 132; 
    constexpr double RED_VOIDSTONE_RELATIVE_Y_RATIO = (double)(900 - 805) / 132;
}

#endif