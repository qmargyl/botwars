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
		enum
		{
			TEX_GRASS = 0,
			TEX_MO_0,
			TEX_MO_1,
			TEX_TREE,
			TEX_TREE2,
			TEX_TREE3,
			TEX_REDFRAME,
			NO_OF_TEXTURES
		} TextureIndex;

		std::vector<MGTexHandle> textures;

	public:
		MGFWrapper();
		virtual bool init(int w, int h, int tw, int th); // Has to be implemented in a class derived from MGFramework
};

#endif




