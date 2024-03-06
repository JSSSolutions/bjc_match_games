//
// Canvas.h
//
// Written by John Sowell
//
// Written 2/19/2024
//
// Defines the Canvas class, which is a subclass of the DX3 class, which houses all the DirectX functionality.
// The Canvas class defines how the match play score sheet is displayed in DirectX.
//
#pragma once

#include <iostream>
#include <thread>
#include <windows.h>

#include "DX3.h"
#include "MatchGame.h"

using namespace std;

// ============================================================================

#define LOAD_MODE	1
#define GAME_MODE	2
#define DONE_MODE	3

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class Canvas : public DX3
{
public:
	char Name[12][25]; // names of the 12 Bowling Journal Competitors
	int Mode, Records, TotalLosses[12], TotalWins[12];

	JSS_FONT Clarendon; // a data file containing the data for a 36-point Clarendon font
	MatchGameType VS[12][12];

	Canvas();
	~Canvas();

	void GameLoop();
	void ComposeFrame();
	void Update();
	void Initialize(HWND hWnd, int Colors = 256);
};