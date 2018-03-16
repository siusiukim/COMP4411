//////////////////////////////////////////////////////////////////////////////////////////
//	Main.cpp
//	Metaballs
//	Downloaded from: www.paulsprojects.net
//	Created:	26th August 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include <GL\glext.h>
#include <GL\wglext.h>
#include "LOG.h"
#include "WINDOW.h"
#include "FPS_COUNTER.h"
#include "TIMER.h"
#include "Maths/Maths.h"
#include "main.h"
#include "CUBE_GRID.h"
#include "METABALL.h"

//link to libraries
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "winmm.lib")

//errorLog MUST be kept - it is used by other files
LOG errorLog;
WINDOW window;
FPS_COUNTER fpsCounter;
TIMER timer;

const int minGridSize=10;
int gridSize=40;
float threshold=1.0f;

const int numDiffuseColors=3;
COLOR diffuseColors[numDiffuseColors];
int currentDiffuseColor=0;

COLOR backgroundColor(0.773f, 0.773f, 0.773f, 1.0f);

CUBE_GRID cubeGrid;

const int numMetaballs=3;
METABALL metaballs[numMetaballs];

//Set up variables
bool DemoInit()
{
	if(!window.Init("Metaballs", 512, 512, 32, 24, 8, WINDOWED_SCREEN))
		return 0;											//quit if not created
	
	//set up grid
	if(!cubeGrid.CreateMemory())
		return false;
	if(!cubeGrid.Init(gridSize))
		return false;

	//set up metaballs
	for(int i=0; i<numMetaballs; i++)
		metaballs[i].Init(VECTOR3D(0.0f, 0.0f, 0.0f), 5.0f+float(i));

	//Set Up Colors
	diffuseColors[0].Set(0.345f, 0.843f, 0.902f, 1.0f);
	diffuseColors[1].Set(0.047f, 0.839f, 0.271f, 1.0f);
	diffuseColors[2].Set(0.976f, 0.213f, 0.847f, 1.0f);	

	//reset timer for start
	timer.Reset();
	
	return true;
}

//Set up openGL
bool GLInit()
{
	//set viewport
	int height;
	if (window.height==0)
		height=1;
	else
		height=window.height;
	
	glViewport(0, 0, window.width, height);					//reset viewport

	//set up projection matrix
	glMatrixMode(GL_PROJECTION);							//select projection matrix
	glLoadIdentity();										//reset
	gluPerspective(45.0f, (GLfloat)window.width/(GLfloat)height, 1.0f, 100.0f);
	
	//load identity modelview
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//other states
	//shading
	glShadeModel(GL_SMOOTH);
	glClearColor(	backgroundColor.r,
					backgroundColor.g,
					backgroundColor.b,
					backgroundColor.a);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//depth
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	//hints
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	//need to normalize!
	glEnable(GL_NORMALIZE);

	glEnable(GL_CULL_FACE);

	//set up lighting
	float ambient[]={0.0f, 0.0f, 0.2f, 1.0f};
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseColors[currentDiffuseColor]);
	
	float position[]={-1.0f, 1.0f, 1.0f, 0.0f};
	glLightfv(GL_LIGHT1, GL_POSITION, position);

	float specular[]={1.0f, 1.0f, 1.0f, 1.0f};
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);

	glEnable(GL_LIGHT1);

	//set full reflectance
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);

	float shininess=32.0f;
	glMaterialfv(GL_FRONT, GL_SHININESS, &shininess);

	return true;
}

//Perform per frame updates
void UpdateFrame()
{
	window.Update();

	//update balls' position
	float c = 2.0f*(float)cos(timer.GetTime()/600);
  
	metaballs[0].position.x=-4.0f*(float)cos(timer.GetTime()/700) - c;
	metaballs[0].position.y=4.0f*(float)sin(timer.GetTime()/600) - c;

	metaballs[1].position.x=5.0f*(float)sin(timer.GetTime()/400) + c;
	metaballs[1].position.y=5.0f*(float)cos(timer.GetTime()/400) - c;

	metaballs[2].position.x=-5.0f*(float)cos(timer.GetTime()/400) - 0.2f*(float)sin(timer.GetTime()/600);
	metaballs[2].position.y=5.0f*(float)sin(timer.GetTime()/500) - 0.2f*(float)sin(timer.GetTime()/400);
	
	//increase or decrease density
	if(window.isKeyPressed(VK_UP) && gridSize<maxGridSize)
	{
		gridSize++;
		cubeGrid.Init(gridSize);
	}

	if(window.isKeyPressed(VK_DOWN) && gridSize>minGridSize)
	{
		gridSize--;
		cubeGrid.Init(gridSize);
	}

	//pause & unpause
	if(window.isKeyPressed('P'))
	{
		timer.Pause();
	}

	if(window.isKeyPressed('U'))
	{
		timer.Unpause();
	}

	//clear the field
	for(int i=0; i<cubeGrid.numVertices; i++)
	{
		cubeGrid.vertices[i].value=0.0f;
		cubeGrid.vertices[i].normal.LoadZero();
	}
	
	//evaluate the scalar field at each point
	VECTOR3D ballToPoint;
	float squaredRadius;
	VECTOR3D ballPosition;
	float normalScale;
	for(int i=0; i<numMetaballs; i++)
	{
		squaredRadius=metaballs[i].squaredRadius;
		ballPosition=metaballs[i].position;

		//VC++6 standard does not inline functions
		//by inlining these maually, in this performance-critical area,
		//almost a 100% increase in speed is found
		for(int j=0; j<cubeGrid.numVertices; j++)
		{
			//ballToPoint=cubeGrid.vertices[j].position-ballPosition;
			ballToPoint.x=cubeGrid.vertices[j].position.x-ballPosition.x;
			ballToPoint.y=cubeGrid.vertices[j].position.y-ballPosition.y;
			ballToPoint.z=cubeGrid.vertices[j].position.z-ballPosition.z;
			
			//get squared distance from ball to point
			//float squaredDistance=ballToPoint.GetSquaredLength();
			float squaredDistance=	ballToPoint.x*ballToPoint.x +
									ballToPoint.y*ballToPoint.y +
									ballToPoint.z*ballToPoint.z;
			if(squaredDistance==0.0f)
				squaredDistance=0.0001f;

			//value = r^2/d^2
			cubeGrid.vertices[j].value+=squaredRadius/squaredDistance;

			//normal = (r^2 * v)/d^4
			normalScale=squaredRadius/(squaredDistance*squaredDistance);
			//cubeGrid.vertices[j].normal+=ballToPoint*normalScale;
			cubeGrid.vertices[j].normal.x+=ballToPoint.x*normalScale;
			cubeGrid.vertices[j].normal.y+=ballToPoint.y*normalScale;
			cubeGrid.vertices[j].normal.z+=ballToPoint.z*normalScale;
		}
	}

	//toggle wireframe
	if(window.isKeyPressed('W'))
	{
		glPolygonMode(GL_FRONT, GL_LINE);
	}

	if(window.isKeyPressed('F'))
	{
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	//Change color
	if(window.isKeyPressed(VK_SPACE))
	{
		currentDiffuseColor++;
		if(currentDiffuseColor==numDiffuseColors)
			currentDiffuseColor=0;
		glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseColors[currentDiffuseColor]);

		window.SetKeyReleased(VK_SPACE);
	}
}

//draw a frame
void RenderFrame()
{
	//Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();										//reset modelview matrix


	glEnable(GL_LIGHTING);
	glTranslatef(0.0f, 0.0f, -30.0f);
	glRotatef((float)timer.GetTime()/30, 1.0f, 0.0f, 1.0f);
	cubeGrid.DrawSurface(threshold);
	glDisable(GL_LIGHTING);

	
	fpsCounter.Update();											//update frames per second counter
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	window.StartTextMode();
	window.Print(0, 28, "FPS: %.2f", fpsCounter.GetFps());			//print the fps
	glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
	window.Print(0, 48, "Grid Size: %d", gridSize);
	window.Print(0, 68, "%d triangles drawn", cubeGrid.numFacesDrawn);
	window.EndTextMode();
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if(window.isKeyPressed(VK_F1))
	{
		window.SaveScreenshot();
		window.SetKeyReleased(VK_F1);
	}

	window.SwapBuffers();									//swap buffers

	//check for any opengl errors
	window.CheckGLError();

	//quit if necessary
	if(window.isKeyPressed(VK_ESCAPE))
		PostQuitMessage(0);
}

void DemoShutdown()
{
	window.Shutdown();										//Shutdown window
}

//ENTRY POINT FOR APPLICATION
//CALL WINDOW CREATION ROUTINE, DEAL WITH MESSAGES, WATCH FOR INTERACTION
int WINAPI WinMain(	HINSTANCE	hInstance,				//instance
					HINSTANCE	hPrevInstance,			//Previous Instance
					LPSTR		lpCmdLine,				//command line parameters
					int			nCmdShow)				//Window show state
{
	//Initiation
	errorLog.Init("Error Log.txt");

	//init variables etc, then GL
	if(!DemoInit())
	{
		errorLog.OutputError("Demo Initiation failed");
		return 0;
	}
	else
		errorLog.OutputSuccess("Demo Initiation Successful");

	if(!GLInit())
	{
		errorLog.OutputError("OpenGL Initiation failed");
		return 0;
	}
	else
		errorLog.OutputSuccess("OpenGL Initiation Successful");

	//Main Loop
	for(;;)
	{
		if(!(window.HandleMessages())) break;//handle windows messages, quit if returns false
		UpdateFrame();
		RenderFrame();
	}

	DemoShutdown();
	
	errorLog.OutputSuccess("Exiting...");
	return (window.msg.wParam);								//Exit The Program
}