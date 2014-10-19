#include "mgframework_wrapper.h"
#include <iostream>
#include <algorithm>
#include "mgframework/mgframework.h"
#include "mgframework/mgmovingobject.h"

MGFWrapper::MGFWrapper()
{
	unsetWindowProperties(); // Force setWindowProperties to be called before init.
	disableTyping();
	setInstanceType(MGFSINGLEPLAYERINSTANCE);
	disableLogging();
}

bool MGFWrapper::init(int w, int h, int tw, int th)
{
	// The window is created.
	if (windowPropertiesSet())
	{
		if (!getWindow()->createWindow())
		{
			return false;
		}

		// All graphics should be loaded here.
		m_Floor = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("tileset.bmp", false));

		m_MOSprite[0] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_00.bmp", true));
		m_MOSprite[1] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_01.bmp", true));
		m_MOSprite[2] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_02.bmp", true));
		m_MOSprite[3] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_03.bmp", true));
		m_MOSprite[4] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_04.bmp", true));
		m_MOSprite[5] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_05.bmp", true));
		m_MOSprite[6] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_06.bmp", true));
		m_MOSprite[7] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_07.bmp", true));
		m_MOSprite[8] = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("movingobject_08.bmp", true));

		m_StationaryObject = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("stationaryobject.bmp", true));
		m_Mark = static_cast<SDL_Texture*>(getWindow()->loadBMPImage("mark.bmp", true));

		// Objcts such as the map are initialized here.
		m_Map.init(w, h, tw, th, getWindow()->getWidth(), getWindow()->getHeight()); // width (in number of tiles), height, tile width (in pixels), tile height, resolution x and y.

		// Setup the edge around the screen to allow scrolling to see the entire map.
		m_Map.setTopEdge(0);
		m_Map.setBottomEdge(0);
		m_Map.setLeftEdge(0);
		m_Map.setRightEdge(0);

		// Setup application specific game logics..
		disableFeatureSelectiveTileRendering();
		disableFeatureOnlySelectOwnedMO();
		enableFeatureMouseScrolling();
		disableFeatureCenterOnMO();

		runConsoleCommand("setfps 60", this, NULL);			// Set initial FPS to 60 to avoid slow start
		runConsoleCommand("logging off", this, NULL);		// Turn on logging for the MGFramework class
		runConsoleCommand("map logging off", this, NULL);	// Turn on logging for the MGMap class
		runConsoleCommand("minimap off", this, NULL);
		
		runConsoleCommand("add mo 20 -owner 1 -area_square 5 5 20 20", this, NULL);
		runConsoleCommand("add mo 20 -owner 2 -area_square 30 30 45 45", this, NULL);

		runConsoleCommand("add so 50", this, NULL);

		return true;
	}
	else
	{
		return false;
	}
}

void MGFWrapper::handleGameLogics()
{

}

void MGFWrapper::draw()
{
	bool noRenderingNeeded = isSelectiveTileRenderingActive() && !MGMovingObject::anyMovingMO() && !isFramingOngoing() && !renderAllTiles();

	if(!noRenderingNeeded)
	{
		// Draw all tiles visible in the window...
		for (int x=0; x < m_Map.getWidth(); x++)
		{
			for ( int y=0; y < m_Map.getHeight(); y++)
			{
				// Only draw the tiles actually visible (+1 to draw partly visible tiles) in the window...
				if(  ((x * m_Map.getTileWidth() + m_Map.getScrollX()) <= getWindow()->getWidth() + m_Map.getTileWidth() - m_Map.getRightEdge()) &&
					 ((x * m_Map.getTileWidth() + m_Map.getScrollX()) >= 0 - m_Map.getTileWidth()) &&
					 ((y * m_Map.getTileHeight() + m_Map.getScrollY()) <= getWindow()->getHeight() + m_Map.getTileHeight() - m_Map.getBottomEdge()) &&
					 ((y * m_Map.getTileHeight() + m_Map.getScrollY()) >= 0 - m_Map.getTileHeight()) &&
					 (!isSelectiveTileRenderingActive() || renderAllTiles() || m_Map.isMarkedForRendering(x, y)) )
				{
					if(m_Map.getTileProperty(x, y) & MGMAP_TP_PROPERTY_1)
					{
						drawTile(m_Floor, 0, 0, x * m_Map.getTileWidth() + m_Map.getScrollX(), y * m_Map.getTileHeight() + m_Map.getScrollY(), m_Map.getTileWidth(), m_Map.getTileHeight());
					}
					m_Map.unmarkForRendering(x, y);
				}
			}
		}

		// Draw all moving objects...
		int oX,oY;
		for(std::list<MGMovingObject>::iterator it = m_MO.begin(); it != m_MO.end(); it++)
		{
			oX = (*it).getTileX() * m_Map.getTileWidth() + m_Map.getScrollX() + (*it).getXOffset();
			oY = (*it).getTileY() * m_Map.getTileHeight() + m_Map.getScrollY() + (*it).getYOffset();
			// Only draw visible moving objects...
			if(detectCollisionRectangle(oX, oY, oX + m_Map.getTileWidth(), oY + m_Map.getTileHeight(), 0, 0, getWindow()->getWidth(), getWindow()->getHeight()))
			{
				drawTile(m_MOSprite[it->getOwner()], 0, 0, oX, oY);
				if(isSelectiveTileRenderingActive())
				{
					m_Map.markForRendering((*it).getTileX(), (*it).getTileY());
					m_Map.markForRendering((*it).getTileX() + 1, (*it).getTileY() + 1);
					m_Map.markForRendering((*it).getTileX() - 1, (*it).getTileY() - 1);
					m_Map.markForRendering((*it).getTileX() + 1, (*it).getTileY() - 1);
					m_Map.markForRendering((*it).getTileX() - 1, (*it).getTileY() + 1);
					m_Map.markForRendering((*it).getTileX() + 1, (*it).getTileY());
					m_Map.markForRendering((*it).getTileX() - 1, (*it).getTileY());
					m_Map.markForRendering((*it).getTileX(), (*it).getTileY() + 1);
					m_Map.markForRendering((*it).getTileX(), (*it).getTileY() - 1);
				}

				if((*it).isMarked())
				{
					drawTile(m_Mark, 0, 0, oX, oY);
				}
			}
		}

		// Draw all stationary objects...
		int sX, sY;
		for(int i = 0; i < getNumberOfSO(); i++)
		{
			if(m_SO != NULL)
			{
				sX=m_SO[i].getTileX() * m_Map.getTileWidth() + m_Map.getScrollX();
				sY=m_SO[i].getTileY() * m_Map.getTileHeight() + m_Map.getScrollY()-16;
				// Only draw visible stationary objects...
				if(detectCollisionRectangle(sX, sY, sX+m_Map.getTileWidth(), sY+m_Map.getTileHeight(), 0, 0, getWindow()->getWidth() - m_Map.getRightEdge(), getWindow()->getHeight() - m_Map.getBottomEdge()))
				{
					drawTile(m_StationaryObject, 0, 0, sX, sY, m_Map.getTileWidth(), m_Map.getTileHeight()+16);
				}
			}
		}
	}

	// Example of how text can be printed on the surface.. Here FPS and time left between frames.
	getWindow()->drawText((std::string("MOs: ") + MGFramework::toString((int)getNumberOfMO()) + 
		std::string("(") + MGFramework::toString((int)MGMovingObject::nMovingMO()) + std::string(")") + std::string("          ")).c_str(), 
			 16, getWindow()->getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 30, 0, 0, 0, 0, 255, 0);
	getWindow()->drawText((std::string("FD : ") + MGFramework::toString((int)getLastFrameDelayTime()) + std::string("          ")).c_str(), 
			 16, getWindow()->getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 50, 0, 0, 0, 0, 255, 0);
	getWindow()->drawText((std::string("FPS: ") + MGFramework::toString((int)getFPS()) + std::string("          ")).c_str(), 
			 16, getWindow()->getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 70, 0, 0, 0, 0, 255, 0);
	getWindow()->drawText((std::string("DT: ") + MGFramework::toString(getDrawnTilesCounter()) + std::string("          ")).c_str(), 
			 16, getWindow()->getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 90, 0, 0, 0, 0, 255, 0);


	// Draw marking frame if marking is ongoing
	if(!noRenderingNeeded)
	{
		if(isFramingOngoing())
		{
			getWindow()->drawRectangleRGB(getFrameStartX(), getFrameStartY(), getFrameEndX(), getFrameEndY(),  0xFF, 0x00, 0x00);
		}
	}


}
