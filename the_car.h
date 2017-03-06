#ifndef THE_CAR_
#define THE_CAR_

#include "the_port.h"

class the_car{
	ComPort the_port;
	char state;//s,f,b,r,l...x,y,1(oppo.l),2(oppo.r)
public:
	the_car():the_port(ComPort("COM6")){
		state = 'P';
		move_s();
	}
	void restart()
	{
		the_port.Send('F');
		//the_port = ComPort("Com5");
	}
	void move_f()
	{
		cout << "forward...";
//		if(state=='A')
//			return;
		the_port.Send('A');
		state = 'A';
		//cout << "...forward";
	}
	void move_b()
	{
		cout << "back...";
//		if(state=='B')
//			return;
		the_port.Send('B');
		state = 'B';
		//cout << "...back";
	}
	void move_r()
	{
		cout << "right...   ";
//		if(state=='R')
//			return;
		//? l or x
		the_port.Send('R');
		state = 'R';
		//cout << "...right   ";
	}
	void move_l()
	{
		cout << "left...    ";
//		if(state=='L')
//			return;
		the_port.Send('L');
		state = 'L';
		//cout << "...left    ";
	}
	void move_s()
	{
		cout << "stop...    ";
//		if(state=='P')
//			return;
		the_port.Send('P');
		state = 'P';
		//cout << "...stop    ";
	}
};

#endif
