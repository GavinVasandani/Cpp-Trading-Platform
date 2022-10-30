//As NetClient directory includes NetCommon headers then we can use NetCommon headers in NetClient like:

#include <iostream>
#include <olc_net.h>

//As we've created message class, let's now create a message object and test the class

enum class CustomMsgTypes : uint32_t
{
	FireBullet,
	MovePlayer
};


int main()
{
	olc::net::message<CustomMsgTypes> msg; /*so we access olc header, we create an object of class messageand we send in custom
	type CustomMsgTypes as the type to use*/
	msg.header.id = CustomMsgTypes::FireBullet; //so ID is type CustomMsgType so it must be assigned to FireBullet or MovePlayer
	//which are variables of the same type. To access variables in class do classname::variablename

	//Things we want to add to message:
	int a = 1;
	bool b = true;
	float c = 3.141;

	//Adding these variables into the message:
	msg << a << b << c; 

	//We want to reassign these variables to test whether our msg remembers our a,b,c values we initially sent
	a = 10;
	b = false;
	c = 2.89;

	//Reading back out the message:
	msg >> c >> b >> a; 

	return 0;
}
