//
// Canvas.cpp
//
// Written by John Sowell
//
// Written 2/19/2024
//
// Implements the Canvas class
//
#include "Canvas.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Canvas::Canvas()
{
}

// ****************************************************************************

void Canvas::ComposeFrame()
{
	// Each DirectX frame is drawn here
	char buf[10]; // black
	int N, x, y;

	FillScreen(10);

	PrintFontCenterAt(995, 25, "BOWLING  JOURNAL  COMPETITION", Clarendon, 12);
	PrintFontCenterAt(995, 85, "MATCH  GAME  STATISTICS", Clarendon, 16);

	PrintFontAt(40, 206, Name[0], Clarendon, 14);
	PrintFontAt(40, 256, Name[1], Clarendon, 1);
	PrintFontAt(40, 306, Name[2], Clarendon, 6);
	PrintFontAt(40, 356, Name[3], Clarendon, 1);
	PrintFontAt(40, 406, Name[4], Clarendon, 0);
	PrintFontAt(40, 453, Name[5], Clarendon, 1);
	PrintFontAt(40, 506, Name[6], Clarendon, 12);
	PrintFontAt(40, 553, Name[7], Clarendon, 1);
	PrintFontAt(40, 603, Name[8], Clarendon, 8);
	PrintFontAt(40, 653, Name[9], Clarendon, 1);
	PrintFontAt(40, 706, Name[10], Clarendon, 4);
	PrintFontAt(40, 753, Name[11], Clarendon, 1);

	Line(410, 150, 410, 850, 9);
	Line(500, 150, 500, 850, 9);
	Line(590, 150, 590, 850, 9);
	Line(680, 150, 680, 850, 9);
	Line(770, 150, 770, 850, 9);
	Line(860, 150, 860, 850, 9);
	Line(950, 150, 950, 850, 9);
	Line(1040, 150, 1040, 850, 9);
	Line(1130, 150, 1130, 850, 9);
	Line(1220, 150, 1220, 850, 9);
	Line(1310, 150, 1310, 850, 9);
	Line(1400, 150, 1400, 850, 9);
	Line(1490, 150, 1490, 850, 9);
	Line(1580, 200, 1580, 800, 9);
	Line(410, 150, 1490, 150, 9);
	Line(410, 850, 1490, 850, 9);
	Line(25, 200, 25, 800, 9);

	Line(25, 200, 1580, 200, 9);
	Line(25, 250, 1580, 250, 9);
	Line(25, 300, 1580, 300, 9);
	Line(25, 350, 1580, 350, 9);
	Line(25, 400, 1580, 400, 9);
	Line(25, 450, 1580, 450, 9);
	Line(25, 500, 1580, 500, 9);
	Line(25, 550, 1580, 550, 9);
	Line(25, 600, 1580, 600, 9);
	Line(25, 650, 1580, 650, 9);
	Line(25, 700, 1580, 700, 9);
	Line(25, 750, 1580, 750, 9);
	Line(25, 800, 1580, 800, 9);

	PrintFontCenterAt(455, 156, "1", Clarendon, 14);
	PrintFontCenterAt(545, 156, "2", Clarendon, 1);
	PrintFontCenterAt(635, 156, "3", Clarendon, 6);
	PrintFontCenterAt(725, 156, "4", Clarendon, 1);
	PrintFontCenterAt(815, 156, "5", Clarendon, 0);
	PrintFontCenterAt(905, 156, "6", Clarendon, 1);
	PrintFontCenterAt(995, 156, "7", Clarendon, 12);
	PrintFontCenterAt(1085, 156, "8", Clarendon, 1);
	PrintFontCenterAt(1175, 156, "9", Clarendon, 8);
	PrintFontCenterAt(1265, 156, "10", Clarendon, 1);
	PrintFontCenterAt(1355, 156, "11", Clarendon, 4);
	PrintFontCenterAt(1445, 156, "12", Clarendon, 1);

	for (x = 0; x < 12; x++)
	{
		FilledRectangle(410 + (90 * x), 200 + (50 * x), 500 + (90 * x), 250 + (50 * x), 9);
	}
	for (x = 0; x < 12; x++)
	{
		for (y = 0; y < 12; y++)
		{
			if (VS[x][y].Wins > 0)
			{
				StrCpy(buf, ToString(VS[x][y].Wins / 10));
				if (VS[x][y].Wins % 10 == 5)
					StrCat(buf, "+");
				PrintFontCenterAt((y * 90) + 455, (x * 50) + 206, buf, Clarendon, 9);
			}
		}

		StrCpy(buf, ToString(TotalWins[x] / 10));
		if (TotalWins[x] % 10 == 5)
			StrCat(buf, "+");
		PrintFontCenterAt(1535, (x * 50) + 206, buf, Clarendon, 27);

		StrCpy(buf, ToString(TotalLosses[x] / 10));
		if (TotalLosses[x] % 10 == 5)
			StrCat(buf, "+");
		PrintFontCenterAt((x*90)+455, 806, buf, Clarendon, 21);
	}

	DX3::ComposeFrame();
}

// ****************************************************************************

void Canvas::GameLoop()
{
	Update();
	StartFrame();
	ComposeFrame();
	EndFrame();
}

// ****************************************************************************

void Canvas::Initialize(HWND hWnd, int Colors)
{
	int x, y;

	DX3::Initialize(hWnd, Colors);	

	SetMyPalette(); // I have a special palette of 28 colors that I use for some of my games
	SetPalette(8, 255, 0, 255); // decided to make the purple color in the palette, a brighter shade of purple

	LoadFont(&Clarendon, "Clarendon36.dat");

	Records = 0;
	Mode = LOAD_MODE;	

	for (x = 0; x < 12; x++)
	{
		TotalWins[x] = 0;
		TotalLosses[x] = 0;

		for (y = 0; y < 12; y++)
		{
			VS[x][y].Wins = 0;
			VS[x][y].Losses = 0;
			VS[x][y].HighWinningMatch = 0;
			VS[x][y].HighLosingMatch = 0;
			VS[x][y].HighTieMatch = 0;
			VS[x][y].HG = 0;
			VS[x][y].L1 = 0;
			VS[x][y].L2 = 0;
			VS[x][y].W1 = 0;
			VS[x][y].W2 = 0;
		}
	}

	StrCpy(Name[0], "Booker  J.  Sowell");
	StrCpy(Name[1], "Bert  Calfee");
	StrCpy(Name[2], "Darla  Jackson");
	StrCpy(Name[3], "Rhonda  Gilbert");
	StrCpy(Name[4], "Sonia  Krout");
	StrCpy(Name[5], "Dorothy  Gilbert");
	StrCpy(Name[6], "Micki  Rhodes");
	StrCpy(Name[7], "Joey  Slaggard");
	StrCpy(Name[8], "Cindy  Seagram");
	StrCpy(Name[9], "Vonda  Sargeant");
	StrCpy(Name[10], "Zelda  LaTrice  Allen");
	StrCpy(Name[11], "Crystal  Thomas");
}

// ****************************************************************************

void Canvas::Update()
{
	// usually, behind-the-scenes variables in the game are updated here
}

// ----------------------------------------------------------------------------

Canvas::~Canvas()
{
}