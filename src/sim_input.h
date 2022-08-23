#ifndef SIM_INPUT_H
#define SIM_INPUT_H

namespace SextantRoller
{
namespace Input
{
    class InputHandler
    {
    public:
        static void InputHandler::sendKeyInput(const char inputKey);
        static void InputHandler::clickMouse(bool leftClick);
        static void InputHandler::moveMouse(int x, int y);
        static void clickAndDrag();
    };
}
}



#endif //SIM_INPUT_H