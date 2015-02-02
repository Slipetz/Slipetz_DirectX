#include "global.h"
#include <WindowsX.h>


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
float cameraStepSize = 0.5f; 


LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch( message )
	{
	case WM_PAINT:
		hdc = BeginPaint( hWnd, &ps );
		EndPaint( hWnd, &ps );
		break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

		// Handle keyboard input...
	case WM_KEYDOWN: 
		switch (wParam) 
		{
		case VK_TAB:
			break;
		case VK_LEFT:
			g_Camera.Strafe(cameraStepSize); // Walk "left"
			break; 
		case VK_RIGHT:
			g_Camera.Strafe(-cameraStepSize); // Walk "right"
			break;
		case VK_UP:
			g_Camera.Walk(cameraStepSize); // Walk "front"
			break;
		case VK_DOWN:
			g_Camera.Walk(-cameraStepSize); // Walk "back"
			break;
		case VK_NEXT:	// PgUp
			break;
		case VK_PRIOR:	// PgDown
			break;
		case 'W': case 'w':
			{
				g_vecObjects[2].material.ambient.w += 0.01f;
				//writeLineToConsole(L"D w: ",g_vecObjects[2].material.ambient.w  );
			}
			
			break;
		case 'S': case 's':
			{
				g_vecObjects[2].material.ambient.w -= 0.01f;
				//writeLineToConsole(L"D w: ",g_vecObjects[2].material.ambient.w  );
			}
			
			break;
		case 'A': case 'a':
			break;
		case 'I': case 'i':
			break;
		case 'D': case 'd':
			break;
		case 'X': case 'x':
			break;
		case 'Y': case 'y':
			break;
		case 'Z': case 'z':
			break;

		}
		break;
		//Left mouse button down.
	case WM_LBUTTONDOWN:
		{
			g_Camera.mLastMousePos.x = static_cast<float>(GET_X_LPARAM(lParam)); 
			g_Camera.mLastMousePos.y = static_cast<float>(GET_Y_LPARAM(lParam)); 
		}
		break;
		//Mouse move
	case WM_MOUSEMOVE:
		{
			//Left button pressed.
			if( LOWORD(wParam) == MK_LBUTTON )
			{
				POINT pt;
				pt.x = GET_X_LPARAM(lParam); 
				pt.y = GET_Y_LPARAM(lParam);

				// Make each pixel correspond to a quarter of a degree.
				float dx = XMConvertToRadians(0.25f*static_cast<float>(pt.x - g_Camera.mLastMousePos.x));
				float dy = XMConvertToRadians(0.25f*static_cast<float>(pt.y - g_Camera.mLastMousePos.y));

				//Multiply by a xx value to increase/decrease camera movement speed.
				g_Camera.Pitch(dy*0.01f);
				g_Camera.RotateY(dx*0.01f);

			}

		}
		break;
	default:
		return DefWindowProc( hWnd, message, wParam, lParam );
	}

	return 0;
}




