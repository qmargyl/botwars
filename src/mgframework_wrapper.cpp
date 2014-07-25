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
		if (!m_Window.createWindow()) 
		{
			return false;
		}

		// All graphics should be loaded here.
		m_Floor = m_Window.loadBMPImage("tileset.bmp");

		m_MOSprite[0] = m_Window.loadBMPImage("movingobject_00.bmp");
		m_MOSprite[1] = m_Window.loadBMPImage("movingobject_01.bmp");
		m_MOSprite[2] = m_Window.loadBMPImage("movingobject_02.bmp");
		m_MOSprite[3] = m_Window.loadBMPImage("movingobject_03.bmp");
		m_MOSprite[4] = m_Window.loadBMPImage("movingobject_04.bmp");
		m_MOSprite[5] = m_Window.loadBMPImage("movingobject_05.bmp");
		m_MOSprite[6] = m_Window.loadBMPImage("movingobject_06.bmp");
		m_MOSprite[7] = m_Window.loadBMPImage("movingobject_07.bmp");
		m_MOSprite[8] = m_Window.loadBMPImage("movingobject_08.bmp");

		m_StationaryObject = m_Window.loadBMPImage("stationaryobject.bmp");
		m_Mark = m_Window.loadBMPImage("mark.bmp");

		SDL_SetColorKey(m_MOSprite[0], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[1], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[2], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[3], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[4], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[5], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[6], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[7], SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_MOSprite[8], SDL_SRCCOLORKEY, 0);

		SDL_SetColorKey(m_StationaryObject, SDL_SRCCOLORKEY, 0);
		SDL_SetColorKey(m_Mark, SDL_SRCCOLORKEY, 0);

		// Objcts such as the map are initialized here.
		m_Map.init(w, h, tw, th, m_Window.getWidth(), m_Window.getHeight()); // width (in number of tiles), height, tile width (in pixels), tile height, resolution x and y.

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
	// Application specific game logics are updated here..
//	if(getNumberOfMO()>0)
//	{
//		m_Map.setScrollOffset(	m_Window.getWidth()/2 - m_MO[0].getTileX() * m_Map.getTileWidth() - m_MO[0].getXOffset(),
//								m_Window.getHeight()/2 - m_MO[0].getTileY() * m_Map.getTileHeight() - m_MO[0].getYOffset() );
//	}
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
				if(  ((x * m_Map.getTileWidth() + m_Map.getScrollX()) <= m_Window.getWidth() + m_Map.getTileWidth() - m_Map.getRightEdge()) &&
					 ((x * m_Map.getTileWidth() + m_Map.getScrollX()) >= 0 - m_Map.getTileWidth()) &&
					 ((y * m_Map.getTileHeight() + m_Map.getScrollY()) <= m_Window.getHeight() + m_Map.getTileHeight() - m_Map.getBottomEdge()) &&
					 ((y * m_Map.getTileHeight() + m_Map.getScrollY()) >= 0 - m_Map.getTileHeight()) &&
					 (!isSelectiveTileRenderingActive() || renderAllTiles() || m_Map.isMarkedForRendering(x, y)) )
				{
					if(m_Map.getTileProperty(x, y) & MGMAP_TP_PROPERTY_1)
					{
						m_Window.drawSprite(m_Floor, 0, 0, x * m_Map.getTileWidth() + m_Map.getScrollX(), y * m_Map.getTileHeight() + m_Map.getScrollY(), m_Map.getTileWidth(), m_Map.getTileHeight());
					}
					m_Map.unmarkForRendering(x, y);
				}
			}
		}

		// Draw all moving objects...
		int oX,oY;
		for(int i=0;i<getNumberOfMO();i++)
		{
			if(m_MO != NULL)
			{
				oX=m_MO[i].getTileX() * m_Map.getTileWidth() + m_Map.getScrollX() + m_MO[i].getXOffset();
				oY=m_MO[i].getTileY() * m_Map.getTileHeight() + m_Map.getScrollY() + m_MO[i].getYOffset();
				// Only draw visible moving objects...
				if(detectCollisionRectangle(oX, oY, oX + m_Map.getTileWidth(), oY + m_Map.getTileHeight(), 0, 0, m_Window.getWidth() - m_Map.getRightEdge(), m_Window.getHeight() - m_Map.getBottomEdge()))
				{
					if(m_MO[i].getOwner() >= 0 && m_MO[i].getOwner() < 9)
					{
						m_Window.drawSprite(m_MOSprite[m_MO[i].getOwner()], 0, 0, oX, oY, m_Map.getTileWidth(), m_Map.getTileHeight());
					}
					if(isSelectiveTileRenderingActive())
					{
						m_Map.markForRendering(m_MO[i].getTileX(), m_MO[i].getTileY());
						m_Map.markForRendering(m_MO[i].getTileX()+1, m_MO[i].getTileY()+1);
						m_Map.markForRendering(m_MO[i].getTileX()-1, m_MO[i].getTileY()-1);
						m_Map.markForRendering(m_MO[i].getTileX()+1, m_MO[i].getTileY()-1);
						m_Map.markForRendering(m_MO[i].getTileX()-1, m_MO[i].getTileY()+1);
						m_Map.markForRendering(m_MO[i].getTileX()+1, m_MO[i].getTileY());
						m_Map.markForRendering(m_MO[i].getTileX()-1, m_MO[i].getTileY());
						m_Map.markForRendering(m_MO[i].getTileX(), m_MO[i].getTileY()+1);
						m_Map.markForRendering(m_MO[i].getTileX(), m_MO[i].getTileY()-1);
					}

					if(m_MO[i].isMarked())
					{
						m_Window.drawSprite(m_Mark, 0, 0, oX, oY, m_Map.getTileWidth(), m_Map.getTileHeight());
					}
				}
			}
		}

		// Draw all stationary objects...
		int sX, sY;
		for(int i=0;i<getNumberOfSO();i++)
		{
			if(m_SO != NULL)
			{
				sX=m_SO[i].getTileX() * m_Map.getTileWidth() + m_Map.getScrollX();
				sY=m_SO[i].getTileY() * m_Map.getTileHeight() + m_Map.getScrollY()-16;
				// Only draw visible stationary objects...
				if(detectCollisionRectangle(sX, sY, sX+m_Map.getTileWidth(), sY+m_Map.getTileHeight(), 0, 0, m_Window.getWidth() - m_Map.getRightEdge(), m_Window.getHeight() - m_Map.getBottomEdge()))
				{
					m_Window.drawSprite(m_StationaryObject, 0, 0, sX, sY, m_Map.getTileWidth(), m_Map.getTileHeight()+16);
				}
			}
		}
	}

	// Example of how text can be printed on the surface.. Here FPS and time left between frames.
#ifndef MGF_DEBUGGING_ENABLED
	m_Window.drawText((string("MOs: ") + MGFramework::toString((int)getNumberOfMO()) + 
			 string("(") + MGFramework::toString((int)MGMovingObject::nMovingMO()) + string(")") + string("          ")).c_str(), 
			 16, m_Window.getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 30, 0, 0, 0, 0, 255, 0);
	m_Window.drawText((string("FD : ") + MGFramework::toString((int)getLastFrameDelayTime()) + string("          ")).c_str(), 
			 16, m_Window.getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 50, 0, 0, 0, 0, 255, 0);
	m_Window.drawText((string("FPS: ") + MGFramework::toString((int)getFPS()) + string("          ")).c_str(), 
			 16, m_Window.getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 70, 0, 0, 0, 0, 255, 0);
	m_Window.drawText((string("DT: ") + MGFramework::toString(getDrawnTilesCounter()) + string("          ")).c_str(), 
			 16, m_Window.getWidth() - m_Map.getWidth() - 16, m_Map.getHeight() + 90, 0, 0, 0, 0, 255, 0);
#endif


	if(!noRenderingNeeded)
	{
		// Draw marking frame if marking is ongoing
		if(isFramingOngoing())
		{
			int uLX=std::min(getFrameStartX(), getFrameEndX());
			int uLY=std::min(getFrameStartY(), getFrameEndY());
			int xL=abs(getFrameStartX() - getFrameEndX());
			int yL=abs(getFrameStartY() - getFrameEndY());
			m_Window.hLine32(uLX, uLY, xL, 0x00FF0000);
			m_Window.hLine32(uLX, uLY+yL, xL, 0x00FF0000);
			m_Window.vLine32(uLX, uLY, yL, 0x00FF0000);
			m_Window.vLine32(uLX+xL, uLY, yL, 0x00FF0000);
		}
	}


}
