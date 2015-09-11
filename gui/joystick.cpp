#include <SDL2/SDL.h>
#include <thread>
#include "model/gamemodel.h"
#include "movement/four_omni_motion/omni4_velcalculator.h"
#include "gui/guiinterface.h"
#include "gui/joystick.h"

namespace joystick
{

bool configure(const std::string& name, int& jAxisMoveUp, int& jAxisMoveSide, int& jAxisRotate)
{
    if(name == "Saitek P990 Dual Analog Pad") {
        jAxisMoveUp = 1;
        jAxisMoveSide = 0;
        jAxisRotate = 3;
    } else if(name == "Logitech Logitech Dual Action") {
        jAxisMoveUp = 1;
        jAxisMoveSide = 0;
        jAxisRotate = 2;
    } else if(name == "Logitech Logitech Freedom 2.4") {
        jAxisMoveUp = 1;
        jAxisMoveSide = 0;
        jAxisRotate = 2;
    } else {
        return false;
    }
    return true;
}

}

namespace joystick
{

//Thread to listen for joystick inputs
std::thread joystickThread;

//Is a *supported* joystick conencted? Used in hasSupport
bool hasSupportedJoystick = true;

//Calculated from Joystick movements
float LB, LF, RB, RF;
bool  Kick, Dribble;

void listener(int jAxisMoveUp, int jAxisMoveSide, int jAxisRotate)
{
    int quit = 0;                      //Quit?
    float axes[8] = {0};               //Joystick axis readings
    Uint8 buttons[50] = {0};           //Joystick button readings
    SDL_Event event;                   //SDL event
    Movement::FourWheelCalculator fwc; //Used to calculate velocites from joystick

    LB = LF = RB = RF = Kick = 0;
    
    //Open joystick 0. Assumed to be aviliable.
    SDL_JoystickOpen(0);

    while(!quit)
    {
        SDL_Delay(5);
        
        //Use SDL to tell us axis position and button presses
        while (SDL_PollEvent(&event)) {
            switch(event.type)
            {
            case SDL_JOYAXISMOTION:
                axes[event.jaxis.axis] = 127 * ((float)event.jaxis.value / 32768);
                break;
            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                buttons[event.jbutton.button] = event.jbutton.state;
                break;
            case SDL_QUIT:
                quit = 1;
                break;
            }
        }
        
        //The vels calculated are for the selected overridden robot.
        Robot* r = gameModel->findMyTeam(GuiInterface::getGuiInterface()->getSelOverBot());
        if(r != NULL)
        {
            Point p = r->getRobotPosition();
            float o = r->getOrientation();
            Kick = buttons[1];
            Dribble = buttons[0];
            float tPos =   o +     -(M_PI/180)*axes[jAxisRotate];
            float xPos = p.x + 3 *  axes[jAxisMoveSide];
            float yPos = p.y + 3 * -axes[jAxisMoveUp];
            auto v = fwc.calculateVels(r, xPos, yPos, tPos, Movement::Type::Default);
            LB = v.LB;
            LF = v.LF;
            RB = v.RB;
            RF = v.RF;
        }
    }

    SDL_JoystickClose(0);
    SDL_Quit();
}

void init()
{
    //Check out `https://github.com/Grumbel/sdl-jstest`
    //   would have never figured out this hint otherwise
    //Worth noting this is a different thread than listen function
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_JOYSTICK | SDL_INIT_NOPARACHUTE);
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
}

void listen()
{
    int jAxisMoveUp, jAxisMoveSide, jAxisRotate;
    std::string name = SDL_JoystickNameForIndex(0);
    if(configure(name, jAxisMoveUp, jAxisMoveSide, jAxisRotate)) {
        std::cout
            << "********************************\n"
            << "Joystick Control Enabled\n"
            << name + '\n'
            << "********************************\n";
        joystickThread = std::thread(listener, jAxisMoveUp, jAxisMoveSide, jAxisRotate);
    } else {
        std::cout
            << "********************************\n"
            << "Joystick \"" + name + "\" is not supported.\n"
            << "********************************\n";
        hasSupportedJoystick = false;
    }
}

bool hasSupport()
{
    return hasSupportedJoystick && (SDL_NumJoysticks() != 0);
}

}
