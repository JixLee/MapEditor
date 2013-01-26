#include <math.h>
#include <iostream>
#include <freeglut.h>
#include "Object.h"
#include "MACRO.h"
#include "XML\tinyxml.h"
#include <string>
#include <vector>

using namespace std;

typedef struct {
	bool mLButtonUp;
	bool mRButtonUp;
	int  mX, mY;
} SMouse;

SMouse mouse;

int mapCount = 0;		//trace how many maps we have exported

ObjectList* mObjectList;
Object* selectedObject = NULL;
Object* previousObject = NULL;

void drawGLString(GLfloat x, GLfloat y, char *textString)
{
	//drawing text on the screen by using glutBitmapCharacter.
    int le;
    int qs;
    
    glRasterPos2f(x, y);
    le = (int) strlen(textString);
    for (qs = 0; qs < le; qs++) 
    {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, textString[qs]);
    }
}

void exportXML()
{
	TiXmlDocument doc;  
	TiXmlElement* msg;
 	TiXmlDeclaration* decl = new TiXmlDeclaration( "1.0", "", "" );  
	doc.LinkEndChild( decl );  

	vector<Object*>::iterator objectIterator;
	for(objectIterator = mObjectList->_mObjectList.begin(); objectIterator != mObjectList->_mObjectList.end(); objectIterator++)
	{
		//write every object into a element
		TiXmlElement * root;

		//check what object is this
		int type = (*objectIterator)->GetType();
		
		switch(type)
		{
		case OBSTACLE:
			root = new TiXmlElement( "Obstacle" ); 
			break;
		case SPAWN_POINT:
			root = new TiXmlElement( "SpawnPoint" ); 
			break;
		case FLAME_THROWER:
			root = new TiXmlElement( "FlameThrower" );
			break;
		default:
			root = new TiXmlElement( "UNKNOWN" ); 
			break;
		}
		
		doc.LinkEndChild( root );

		//its position
		msg = new TiXmlElement( "Position" );
		root->LinkEndChild(  msg );
		msg->SetAttribute("x", (*objectIterator)->_mRecordedPosition->x);
		msg->SetAttribute("z", (*objectIterator)->_mRecordedPosition->y);

		//its size
		msg = new TiXmlElement( "Size" );
		root->LinkEndChild(  msg );
		//need to conver the size into percentage
		int sizeWidth = (*objectIterator)->sizeWidth / SCALE_VALUE;
		int sizeHeight = (*objectIterator)->sizeHeight / SCALE_VALUE;
		msg->SetAttribute("scaleWidth", sizeWidth);
		msg->SetAttribute("scaleHeight", sizeHeight);

		if(type==FLAME_THROWER)
		{
			//its facing
			msg = new TiXmlElement( "Direction" );
			root->LinkEndChild(  msg );
			msg->SetAttribute("facing", (*objectIterator)->facing);
		}
	}

	char fileNameString[20];
	sprintf(fileNameString, "area_%d.xml", mapCount);
	doc.SaveFile(fileNameString);
	mapCount++;

}
void init (void) {
	mObjectList = new ObjectList();
}

void selectObject(int x, int y)
{
	if(mObjectList->getSize()<=0)
		return;

	//did we click on something?
	if(mObjectList->selectObject(x,y)==NULL)
	{
		//no
		//reset
		selectedObject = NULL;
		if(previousObject)
			previousObject->SetRed();
		previousObject = NULL;
	}
	//yes
	//do we have a object selected now?
	else if(selectedObject==NULL)
	{
		//no
		selectedObject=mObjectList->selectObject(x,y);
		selectedObject->SetGreen();
		previousObject = selectedObject;
	}
	else
	{
		//yes
		selectedObject=mObjectList->selectObject(x,y);

		//is the current select object same as previous object?
		if(previousObject == selectedObject)
		{
			//yes
			//do nothing
		}
		else
		{
			//no
			previousObject->SetRed();
			selectedObject->SetGreen();
			previousObject = selectedObject;
		}
	}
}
void onMouseClick (int button, int state, int x, int y) {
	switch (button) {
		case GLUT_LEFT_BUTTON:
			mouse.mLButtonUp = state;
			if(state==0)
			{
				selectObject(x,y);
			}
			break;

		case GLUT_RIGHT_BUTTON:
			mouse.mRButtonUp = state;
			break;

		case GLUT_MIDDLE_BUTTON:
			break;
	}
}


void onMouseMove (int x, int y) {
	if(selectedObject!=NULL)
	{
		selectedObject->Move(x,y,mObjectList->_mObjectList);
	}
}

void keyboard (unsigned char key, int x, int y) {
	switch(key){
		case 'q':
		case 'Q':
			mObjectList->Create(x,y,OBSTACLE);
			break;
		case 'x':
		case 'X':
			exportXML();
			break;
		case 'z':
		case 'Z':
			mObjectList->Clear();
			break;
		case 'w':
		case 'W':
			mObjectList->Create(x,y,SPAWN_POINT);
			break;
		case 'e':
		case 'E':
			mObjectList->Create(x,y,FLAME_THROWER);
			break;
		case '+':
			if(selectedObject!=NULL)
				selectedObject->upSizeAll();
			break;
		case '-':
			if(selectedObject!=NULL)
				selectedObject->downSizeAll();
			break;
		case 'u':
		case 'U':
			if(selectedObject!=NULL)
				selectedObject->upSizeWidth();
			break;
		case 'i':
		case 'I':
			if(selectedObject!=NULL)
				selectedObject->downSizeWidth();
			break;
		case 'j':
		case 'J':
			if(selectedObject!=NULL)
				selectedObject->upSizeHeight();
			break;
		case 'k':
		case 'K':
			if(selectedObject!=NULL)
				selectedObject->downSizeHeight();
			break;
		case 'p':
		case 'P':
			if(selectedObject!=NULL)
				selectedObject->rotate();
			break;
		default:
			break;
	}
}

void RenderUI(void)
{
	glPushMatrix();
	glColor3f(1,1,1);
	drawGLString(WINDOWS_WIDTH*0.76, WINDOWS_HEIGHT*0.05, "Legends");
	drawGLString(WINDOWS_WIDTH*0.76, WINDOWS_HEIGHT*0.10, "O = Obstacle");
	drawGLString(WINDOWS_WIDTH*0.76, WINDOWS_HEIGHT*0.15, "S = Enemy Spawn Point");
	drawGLString(WINDOWS_WIDTH*0.76, WINDOWS_HEIGHT*0.20, "F = Flame Thrower");
	drawGLString(WINDOWS_WIDTH*0.76, WINDOWS_HEIGHT*0.30, "Please read menu.txt");
	drawGLString(WINDOWS_WIDTH*0.76, WINDOWS_HEIGHT*0.32, "for instructions");
	glPopMatrix();
}
void Redner (void) {
	glClear (GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
		glLoadIdentity();
		gluOrtho2D(0, WINDOWS_WIDTH, 0, WINDOWS_HEIGHT);
		glScalef(1, -1, 1);
		glTranslatef(0, -WINDOWS_HEIGHT, 0);
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
			glLoadIdentity();

			mObjectList->Render();

			//because I want a sqaure map, some of the area in width is not been used
			//draw a line at that position
			glColor3f(1,1,1);
			glBegin (GL_LINES);
				glVertex2f (601,0);
				glVertex2f (601,WINDOWS_HEIGHT);
			glEnd();

			//draw the potentional borders
			//top
			glBegin (GL_LINES);
				glVertex2f (BORDER_SIZE , BORDER_SIZE);
				glVertex2f (600 - BORDER_SIZE, BORDER_SIZE);
			glEnd();

			//bottom
			glBegin (GL_LINES);
				glVertex2f (BORDER_SIZE, WINDOWS_HEIGHT - BORDER_SIZE);
				glVertex2f (600 - BORDER_SIZE,WINDOWS_HEIGHT - BORDER_SIZE);
			glEnd();

			//left
			glBegin (GL_LINES);
				glVertex2f (BORDER_SIZE,BORDER_SIZE);
				glVertex2f (BORDER_SIZE,WINDOWS_HEIGHT - BORDER_SIZE);
			glEnd();

			//right
			glBegin (GL_LINES);
				glVertex2f (600 - BORDER_SIZE, BORDER_SIZE);
				glVertex2f (600 - BORDER_SIZE, WINDOWS_HEIGHT - BORDER_SIZE);
			glEnd();
			RenderUI();

		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

	glutSwapBuffers ();
	glutPostRedisplay ();
}

void main (int argc, char * argv[]) {
	glutInit (&argc, argv);
	glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize (WINDOWS_WIDTH, WINDOWS_HEIGHT);
	glutInitWindowPosition (0, 0);
	glutCreateWindow ("Map Editor");
	glutDisplayFunc (Redner);
	glutMouseFunc (onMouseClick);
	glutMotionFunc (onMouseMove);
	glutKeyboardFunc (keyboard);

	init();
	glutMainLoop ();
}
