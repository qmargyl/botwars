#ifndef _MGFRAMEWORK_WRAPPER_H
#define _MGFRAMEWORK_WRAPPER_H

#include "mgframework/mgframework.h"

class MGFWrapper : public MGFramework
{
	private:

		// These methods have to be implemented in a class derived from MGFramework.
		virtual void handleGameLogics();
		virtual void draw();

		// Graphics (which is not part of the framework) should be added here.
		SDL_Texture *m_Floor;

		SDL_Texture *m_MOSprite[9];

		SDL_Texture *m_StationaryObject;
		SDL_Texture *m_Mark;

	public:
		MGFWrapper();
		virtual bool init(int w, int h, int tw, int th); // Has to be implemented in a class derived from MGFramework
};

#endif




