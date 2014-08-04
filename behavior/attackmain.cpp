#include "attackmain.h"
#include "skill/driveball.h"
#include "skill/kicktopoint.h"
#include "utilities/measurments.h"
#include "model/gamemodel.h"
#include "model/robot.h"

AttackMain::AttackMain(const ParameterList& list)
{
    state = initial;
}

void AttackMain::perform(Robot * robot)
{
    #if TRACE
        cout << endl << "Performing Behavior::AttackMain" << endl;
    #endif

    gm = GameModel::getModel();

    //Set either robID(1) or robID(2) as the support_attacker
    Robot * support_attacker;

    if (robot->getID()==0)
        support_attacker = gm->getMyTeam().at(1);
    else
        support_attacker = gm->getMyTeam().at(0);


    //Get info from gamemodel
    sp = support_attacker->getRobotPosition();
    rp = robot->getRobotPosition();
    gp = gm->getOpponentGoal();
    bp = gm->getBallPoint();
    goal_direction = Measurments::angleBetween(rp, gp);  //Sets drive direction towards goal


    //Initialize skills that are used in switch statement
    Skill::KickToPoint pass_skill = Skill::KickToPoint(sp,7*M_PI/180);
    Skill::KickToPoint score_skill = Skill::KickToPoint(gp,7*M_PI/180);


    //When the robot reaches the ball for the first time, drive_start_point is set to current position
    if(touched_ball == false)
    {
        drive_start_point = rp;
        if(Measurments::isClose(rp,bp,150))
        {
            //touched_ball = true;
        }
    }

    //Create switch logic
    switch (state)
    {
        case initial:
            state = score;
            drive_skill = new Skill::DriveBall(gp, goal_direction);
            break;
        case score:
            if(Measurments::isClose(bp,gp,shot_distance))
            {
                score_skill.perform(robot);
                //std::cout << "AttackMain: Score" << std::endl;
            }
            else
            {
                state = drive;
            }

            break;

        case drive:
            if(!Measurments::isClose(drive_start_point,rp,drive_distance))
            {
                state = pass;
                break;
            }
            else
            {
                drive_skill->perform(robot);
                //std::cout << "AttackMain: Drive" << std::endl;
                state = score;
            }
            break;

        case pass:
            if(Measurments::angleDiff(Measurments::angleBetween(sp,bp),support_attacker->getOrientation())<5*M_PI/180)
            {
                pass_skill.perform(robot);
                //std::cout << "AttackMain: Pass" << std::endl;
                state = score;
            }
    }

}