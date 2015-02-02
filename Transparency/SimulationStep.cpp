#include "global.h"



float const stepSize = 0.0001f;



void SimulationStep(float deltaSeconds)
{



	const float MIN_TIMESTEP = 0.001f;
	if (deltaSeconds > MIN_TIMESTEP)
	{
		deltaSeconds = MIN_TIMESTEP;
	}
	g_TheTimer.Reset();
	g_TheTimer.Start();

	//Update cube
	//g_vecObjects[3].UpdateBoltTexture(deltaSeconds, 0.09f);

	//Update floor texture 1
	//Huzzah! Update the sweet vader bomber to have this animation-ish effect!
	g_vecObjects[3].textOffsetAndZoom[0].Offset.x += 0.001f;
	g_vecObjects[3].textOffsetAndZoom[0].Offset.y += 0.001f;
	g_vecObjects[3].textOffsetAndZoom[0].Zoom.x = 1.0f;
	g_vecObjects[3].textOffsetAndZoom[0].Zoom.y = 1.0f;


	
	//
	std::vector<CObject>::iterator itObject = g_vecObjects.begin();
	for (itObject; itObject != g_vecObjects.end();itObject++)
	{
		if (itObject->rotate)
		{
			itObject->rotation.y += stepSize;
		}

		itObject->position.x += getRand<float>(-0.05f, 0.05f);
		itObject->position.y += getRand<float>(-0.05f, 0.05f);
		itObject->position.z += getRand<float>(-0.05f, 0.05f);
	}

}