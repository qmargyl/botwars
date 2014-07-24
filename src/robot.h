#ifndef _ROBOT_H
#define _ROBOT_H

class MGFWrapper;

class IRobot
{
	private:

	protected:
		// World Sensors - Allowing a robot to get information about the world
		virtual int getWorldXMin() = 0;
		virtual int getWorldXMax() = 0;
		virtual int getWorldYMin() = 0;
		virtual int getWorldYMax() = 0;
		
		// Self Sensors - Allowing a robot to get information about itself
		virtual int getLocationX() = 0;
		virtual int getLocationY() = 0;
		virtual bool isMoving() = 0;
		virtual int getRemainingEnergy() = 0;
		
		// Actions
		virtual void moveTo(int x, int y) = 0;
		virtual void stop() = 0;

	public:
		int getIRobotMajorVersion();
		int getIRobotMinorVersion();
};



class Robot : public IRobot
{
	private:
		static MGFWrapper &m_Mgfw;
	protected:
		// World Sensors - Allowing a robot to get information about the world
		int getWorldXMin();
		int getWorldXMax();
		int getWorldYMin();
		int getWorldYMax();
		
		// Self Sensors - Allowing a robot to get information about itself
		int getLocationX();
		int getLocationY();
		bool isMoving();
		int getRemainingEnergy();
		
		// Actions
		void moveTo(int x, int y);
		void stop();
		
	public:
		// Used by RobotWars framework
		void init(MGFWrapper &mgfw);
		virtual void performMove() = 0;		// Implement derived robot's AI in this function

};




#endif