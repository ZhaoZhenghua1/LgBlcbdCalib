#pragma once

struct Rectparm
{
	int x;
	int y;
	int width;
	int height;
};

struct blackboardparam 
{
	Rectparm blackboardLT;
	Rectparm blackboardRT;
	Rectparm blackboardLB;
	Rectparm blackboardRB;
};

struct tchparam
{
	int calibwidth;
	int calibheight;

	int procwidth;
	int procheight;

	int roix;
	int roiy;
	int roiwidth;
	int roiheight;

	float showrate;

	int isHchange;

	Rectparm shield[3];

	/*int shieldx[3];
	int shieldy[3];
	int shieldwidth[3];
	int shieldheight[3];*/

	int blackboardx;
	int blackboardy;
	int blackboardwidth;
	int blackboardheight;

	int teacherdeskx;
	int teacherdesky;
	int teacherdeskwidth;
	int teacherdeskheight;

	int zoomareax;
	int zoomareay;
	int zoomareawidth;
	int zoomareaheight;

	int speed;
};

struct stuparam
{
	int calibwidth;
	int calibheight;

	int procwidth;
	int procheight;

	int roix;
	int roiy;
	int roiwidth;
	int roiheight;

	Rectparm shield[3];

	/*int shieldx[3];
	int shieldy[3];
	int shieldwidth[3];
	int shieldheight[3];*/

	Rectparm tlheight;
	Rectparm trheight;
	Rectparm blheight;
	Rectparm brheight;
};

struct luboparam
{
	int savewidth;
	int saveheight;

	int screanchangeport;
	int startport;

	tchparam tparam;
	stuparam sparam;
	blackboardparam bparam;
	blackboardparam bparam2;
};
