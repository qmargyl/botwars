#include "mgframework_wrapper.h"
#include "mgframework/mgframework.h"

using std::string;

int main(int argc, char **argv)
{
	MGFWrapper* mgfw = 0;
	MGWindow window;

	bool loggingOn = false;
	bool scriptFile = false;
	char scriptFileName[256] = "";

	if(argc==1)
	{
		// No parameters were given
	}
	else
	{
		for(int i=1; i<argc; i++)
		{
			if(strcmp(argv[i], "-logging")==0)
			{
				loggingOn = true;
			}
			else if(strcmp(argv[i], "-script")==0)
			{
				if(i + 1 == argc)
				{
					//No parameter after -script
					goto EXIT_MAIN_RIGHT_AWAY;
				}
				else
				{
					//Store argv[++i] as script file name..
					strcpy(scriptFileName, argv[++i]);
					scriptFile = true;
				}
			}
			else
			{
				// Unknown parameter.
			}
		}
	}


	// Create and initialize the framework...


	mgfw = new MGFWrapper();
	mgfw->setWindowProperties(MGWindow_RES_1024_768, 32, false, 
							  string("BotWars v0.1 based on MGF ") + 
							  string(mgfw->getMGFrameworkVersion()),
							  &window);

	if(loggingOn) mgfw->enableLogging();

	if(mgfw->windowPropertiesSet())
	{
		// If initialization is ok, run the framework...
		if(mgfw->init(64, 48, 32, 32))
		{
			if(scriptFile)
			{
				mgfw->run(scriptFileName);
			}
			else
			{
				mgfw->run(NULL);
			}
		}
	}

EXIT_MAIN_RIGHT_AWAY:

	delete mgfw;
	return 0;
}
