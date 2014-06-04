#ifndef GOTOPOSITIONWITHORIENTATION_H
#define GOTOPOSITIONWITHORIENTATION_H

#include "skill.h"
#include "model/robot.h"
#include "skill/rotate.h"

namespace Skill {

    class GoToPositionWithOrientation : public Skill
    {
    public:
        GoToPositionWithOrientation(Point target, double goalOrientation);
        GoToPositionWithOrientation(float tx, float ty, double goalOrientation);

        void perform(Robot * robot);
    private:
        Point   targetPosition;
        double goalOrientation;
    };

}

#endif // GOTOPOSITIONWITHORIENTATION_H
