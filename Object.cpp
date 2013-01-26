#include "Object.h"
#include <freeglut.h>
#include <iostream>

void drawGLStringLOCAL(GLfloat x, GLfloat y, char *textString)
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

Object::Object(int uniqueId, int type)
:sizeWidth(30),
sizeHeight(30),
facing(NORTH)
{
	_uniqueId = uniqueId;

	_mPosition = new vector2D();
	_mPosition->x = 0;
	_mPosition->y = 0;

	_mRecordedPosition = new vector2D();

	_mType = type;
	_mColor.red = 1;
	_mColor.green = 0;
	_mColor.blue = 0;
}

Object::~Object(void)
{
	if(_mPosition)
	{
		delete _mPosition;
		_mPosition = NULL;
	}
}

void Object::Render(void)
{
	glPushMatrix();

	//translate it using the position
	glTranslatef(_mPosition->x,_mPosition->y,0);
	glColor3f(_mColor.red,_mColor.green,_mColor.blue);
	glBegin (GL_LINES);
		glVertex2f (0.0, 0.0);
		glVertex2f (sizeWidth, 0.0);

		glVertex2f (0.0, 0.0);
		glVertex2f (0.0, sizeHeight);

		glVertex2f (sizeWidth, 0.0);
		glVertex2f (sizeWidth, sizeHeight);

		glVertex2f (0.0, sizeHeight);
		glVertex2f (sizeWidth, sizeHeight);
	glEnd();

	if(_mType==FLAME_THROWER)
	{
		glPushMatrix();

		switch(facing)
		{
		case NORTH:
			glTranslatef(sizeWidth/2,-sizeHeight/3,0);
			break;
		case EAST:
			glTranslatef(sizeWidth,sizeHeight/2,0);
			break;
		case SOUTH:
			glTranslatef(sizeWidth/2,sizeHeight,0);
			break;
		case WEST:
			glTranslatef(-sizeWidth/3,sizeHeight/2,0);
			break;
		}
		
		glBegin (GL_LINES);
			glVertex2f (0.0, 0.0);
			glVertex2f (sizeWidth/3, 0.0);

			glVertex2f (0.0, 0.0);
			glVertex2f (0.0, sizeHeight/3);

			glVertex2f (sizeWidth/3, 0.0);
			glVertex2f (sizeWidth/3, sizeHeight/3);

			glVertex2f (0.0, sizeHeight/3);
			glVertex2f (sizeWidth/3, sizeHeight/3);
		glEnd();

		glPopMatrix();
	}

	switch(_mType)
	{
	case OBSTACLE: 
			drawGLStringLOCAL(sizeWidth*0.4,sizeHeight*0.6,"O");
		break;
	case SPAWN_POINT:
			drawGLStringLOCAL(sizeWidth*0.4,sizeHeight*0.6,"S");
		break;
	case FLAME_THROWER:
			drawGLStringLOCAL(sizeWidth*0.4,sizeHeight*0.6,"F");
		break;
	}
	glPopMatrix();
}

void Object::Move(int x, int y, vector<Object*> objectList)
{
	//I want every object is within a square
	//so anything over height value should be force back
	
	if(x+sizeWidth > WINDOWS_HEIGHT - BORDER_SIZE)
		x = WINDOWS_HEIGHT - sizeWidth - BORDER_SIZE;

	if(x < BORDER_SIZE )
		x = BORDER_SIZE;

	if(y+sizeHeight > WINDOWS_HEIGHT - BORDER_SIZE)
		y = WINDOWS_HEIGHT - sizeHeight - BORDER_SIZE;

	if(y < BORDER_SIZE)
		y = BORDER_SIZE;

	//now convert the mouse screen position to the values i want to recorde in xml
	//the unit value for each map area in xml is from 0 to 100
	_mRecordedPosition->x = x/SCALE_VALUE;
	_mRecordedPosition->y = y/SCALE_VALUE;

	//now recorde the actual x and y
	_mPosition->x = x;
	_mPosition->y = y;
}

bool Object::Collision(int x, int y)
{
	//check if the mouse is colliding with this object
	if(x >= _mPosition->x && x <= _mPosition->x+sizeWidth)
		if(y >= _mPosition->y && y <= _mPosition->y+sizeHeight)
			return true;

	return false;
}

ObjectList::ObjectList()
:objectID(0)
{
}

ObjectList::~ObjectList()
{
	if(dummy) 
	{
		delete dummy;
		dummy = NULL;
	}

	_mObjectList.clear();
}

void ObjectList::Render(void)
{
	vector<Object*>::iterator objectIterator;
	for(objectIterator = _mObjectList.begin(); objectIterator != _mObjectList.end(); objectIterator++)
	{
		(*objectIterator)->Render();
	}
}

void ObjectList::Create(int x, int y, int type)
{
	dummy = new Object(objectID, type);
	dummy->Move(x,y,_mObjectList);

	objectID++;

	_mObjectList.push_back(dummy);
}

Object* ObjectList::selectObject(int x, int y)
{
	Object* tempObject = NULL;

	bool found = false;
	vector<Object*>::iterator objectIterator;
	for(objectIterator = _mObjectList.begin(); objectIterator != _mObjectList.end(); objectIterator++)
	{
		if((*objectIterator)->Collision(x,y))
		{
			tempObject = (*objectIterator);
			found = true;
		}
	}

	if(!found)
		return NULL;
	else
		return tempObject;
}