#ifndef RUN_H
#define RUN_H

#include<iostream>
#include<vector>
#include "cv.h";
#include "the_car.h"
#include "coordinate.h"
#include "pictureDeal.h"

#define sin_turn 0.25

int adj_cam()
{
	CvCapture *cam;
	IplImage *tmp;

	cam = cvCreateCameraCapture(0);
	if (!cam)return 0;

	cvNamedWindow("adj_cam");
	while (cvWaitKey(1) != ' ')
	{
		tmp = cvQueryFrame(cam);
		cvShowImage("adj_cam", tmp);
	}
	cvDestroyWindow("adj_cam");
	cvReleaseCapture(&cam);

	return 0;
}

void test(the_car car)
{
	char tmp;

	tmp = cvWaitKey();
	while (tmp != ' ')
	{
		switch (tmp)
		{
		case 'w':
			car.move_f();
			break;
		case 'a':
			car.move_l();
			break;
		case 's':
			car.move_b();
			break;
		case 'd':
			car.move_r();
			break;
		case 'p':
			car.move_s();
			break;
		default:
			break;
		}
	}
}

double get_sine(coordinate a, coordinate b)
{
	double sine;

	sine = (a.x*b.y - a.y*b.x)*1.0;		//叉乘
	sine /= (a.length() * b.length());

	return sine;
}

double get_sine(coordinate car_head, coordinate car_tail, coordinate destination)
{
	coordinate car_dtn, tail_head;
	double sine;		//两向量夹角正弦值

	tail_head.x = car_head.x - car_tail.x;
	tail_head.y = car_head.y - car_tail.y;		//小车方向向量

	car_dtn.x = destination.x - car_head.x;
	car_dtn.y = destination.y - car_head.y;		//小车至本段终点的向量

	sine = (tail_head.x*car_dtn.y - tail_head.y*car_dtn.x)*1.0;		//叉乘
	sine /= (car_dtn.length() * tail_head.length());

	return sine;
}

double get_cosine(coordinate a, coordinate b)
{
	double cosine;

	cosine = (a.x*b.x + a.y*b.y)*1.0;
	cosine /= (a.length()*b.length());

	return cosine;
}

double get_cosine(coordinate car_head, coordinate car_tail, coordinate destination)
{
	coordinate car_dtn, tail_head;
	double cosine;		//两向量夹角正弦值

	tail_head.x = car_head.x - car_tail.x;
	tail_head.y = car_head.y - car_tail.y;		//小车方向向量

	car_dtn.x = destination.x - car_head.x;
	car_dtn.y = destination.y - car_head.y;		//小车至本段终点的向量

	cosine = tail_head.x*car_dtn.x + tail_head.y*car_dtn.y;
	cosine /= (car_dtn.length() * tail_head.length());

	return cosine;
}

bool out_bound(coordinate head, coordinate tail)
{
	if (head.x < 10 || head.x>390 || head.y < 10 || head.y>390)
		return 1;
	if (tail.x < 10 || tail.x>390 || tail.y < 10 || tail.y>390)
		return 1;
	return 0;
}

char get_dct(coordinate car_head, coordinate car_tail, int dtn)
{
	coordinate t2h, h2d, path;
	int vst;
	char dct;
	double sine1, sine2;
	extern vector<coordinate> point;

	h2d.x = point[dtn].x - car_head.x;
	h2d.y = point[dtn].y - car_head.y;

	t2h.x = car_head.x - car_tail.x;
	t2h.y = car_head.y - car_tail.y;

	vst = (dtn - 1 + point.size()) % point.size();

	path.x = point[dtn].x - point[vst].x;
	path.y = point[dtn].y - point[vst].y;

	if (get_cosine(t2h, h2d) < 0.8)
		dct = 's';
	else
	{
		sine1 = get_sine(path, t2h);
		sine2 = get_sine(h2d, t2h);
		if (sine2<sin_turn && sine2>-sin_turn)
			//if (sine1<sin_turn && sine1>-sin_turn)
				dct = 'w';
			/*else
				if (sine1 >= sin_turn)
					dct = 'a';
				else
					dct = 'd';*/
		else
			if (sine2 >= sin_turn)
				dct = 'a';
			else
				dct = 'd';
	}
	if (out_bound(car_head,car_tail))
    {
        if (dct=='w')
            dct='s';
        if (dct=='s')
            dct='w';
    }

	return dct;
}

bool get_destination(coordinate car_head, coordinate car_tail, int dtn)
{
	coordinate t2h, h2d, dct;
	int vst;
	extern vector<coordinate> point;

	/*head.x = car_head.x - point[dtn].x;
	head.y = car_head.y - point[dtn].y;

	tail.x = car_tail.x - point[dtn].x;
	tail.y = car_tail.y - point[dtn].y;*/

	h2d.x = point[dtn].x - car_head.x;
	h2d.y = point[dtn].y - car_head.y;

	t2h.x = car_head.x - car_tail.x;
	t2h.y = car_head.y - car_tail.y;

	vst = (dtn - 1 + point.size()) % point.size();

	dct.x = point[dtn].x - point[vst].x;
	dct.y = point[dtn].y - point[vst].y;

	if (h2d.length() <= 30 && get_cosine(h2d, t2h) > 0.8)
		return 1;

	return 0;
}

#endif
