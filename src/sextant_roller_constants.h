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
}

#endif