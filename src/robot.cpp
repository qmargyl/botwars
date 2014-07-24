
#include "robot.h"
#include "mgframework_wrapper.h"

int IRobot::getIRobotMajorVersion()
{
	return 0;
}

int IRobot::getIRobotMinorVersion()
{
	return 1;
}


void Robot::init(MGFWrapper &mgfw)
{
	m_Mgfw = mgfw;
}

