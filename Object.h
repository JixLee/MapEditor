#pragma once
#include <vector>
#include "MACRO.h"

using namespace std;

#define OBSTACLE	0
#define SPAWN_POINT 1
#define FLAME_THROWER 2

typedef struct {
	int x;
	int y;
} vector2D;

typedef struct {
	int red;
	int green;
	int blue;
} color;

class Object
{
protected:
	vector2D* _mPosition;				//this is the position to display on the editor
	int _mType;							//trace what type of object is this
	color _mColor;

	void Adjust(Object* target){};		//adjust this object according to target
public:
	int _uniqueId;
	float sizeHeight;
	float sizeWidth;
	int facing;

	vector2D* _mRecordedPosition;		//this is the value i want to write to xml

	Object(int uniqueId, int type);
	~Object(void);

	void Move(int x, int y, vector<Object*> objectList);
	void Render(void);
	void SetGreen()	{ _mColor.red = 0; _mColor.green = 1; _mColor.blue = 0; }
	void SetRed()	{ _mColor.red = 1; _mColor.green = 0; _mColor.blue = 0; }
	bool Collision(int x, int y);
	int GetType(void){return _mType;};
	void downSizeAll()		{ downSizeWidth(); downSizeHeight(); }				//scale down both width and height
	void upSizeAll()		{ upSizeWidth(); upSizeHeight(); }					//scale up both width and height
	void downSizeWidth()	{ sizeWidth-=SCALE_SIZE; if(sizeWidth<=SCALE_SIZE)sizeWidth=SCALE_SIZE; }						//scale down width			
	void downSizeHeight()	{ sizeHeight-=SCALE_SIZE; if(sizeHeight<=SCALE_SIZE)sizeHeight=SCALE_SIZE; }					//scale down height
	void upSizeWidth()		{ sizeWidth+=SCALE_SIZE; }
	void upSizeHeight()		{ sizeHeight+=SCALE_SIZE; }

	void rotate()		{ facing++; if(facing>WEST)facing = NORTH; }
};

class ObjectList
{
	int objectID;
	Object* dummy;						//dummy object used when creating a new object
public:
	vector<Object*> _mObjectList;		//the list of object exist in the editor

	ObjectList();
	~ObjectList();

	void Render(void);
	void Create(int x, int y, int type);			//create a new object at this position.
	Object* selectObject(int x, int y);				//trace with the mouse has collided with a object, if yes, return it out
	void Clear(void){_mObjectList.clear();}		//clear all the objects
	int getSize(void) { return _mObjectList.size(); }
};