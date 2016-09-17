#include "stdafx.h"
#include "esUtil.h"

extern int esMain(ESContext *esContext);

ESContext *gesContext;
/// 
// kdMain() 
//
//    Main function for OpenKODE application
//
KDint kdMain(KDint argc, const KDchar *const *argv)
{
	gesContext = new ESContext();
	ESContext *esContext = gesContext;

	memset(esContext, 0, sizeof(ESContext));

	if (esMain(esContext) != GL_TRUE)
	{
		return 1;
	}

	// Main Loop
	while (1)
	{
		// Wait for an event
		const KDEvent *evt = kdWaitEvent(0);
		if (evt)
		{
			// Exit app
			if (evt->type == KD_EVENT_WINDOW_CLOSE)
				break;
		}

		// Draw frame
		Draw(&userData);
	}

	

	

	if (esContext->shutdownFunc != NULL)
	{
		esContext->shutdownFunc(esContext);
	}

	if (esContext->userData != NULL)
	{
		free(esContext->userData);
	}

	free(esContext);
	// Destroy the window
	kdDestroyWindow(window);

	return 0;
}
