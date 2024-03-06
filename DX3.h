//
// DX3.h
//
// Written by John Sowell
//
// Written 3/2/2018 - 3/5/2018
//
// Defines a faster version of the two-dimensional DirectX Engine that'll be 
//the basis for my game programs
//
// 1/29/2022
//		Added function to get number in string of numbers (for LoadFont function)
//
// 7/5/2022
//		Added Log function, added condition in FillScreen function for if the
//	
// 1/17/2024
//		changed char parameters to const char parameters. Also more completely deleted pointers
//
#include <conio.h>
#include <d3d9.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "Strings3.h"

#pragma once
#pragma comment(lib, "d3d9")

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class JSS_SPRITE
{
public:
	BOOL Alive;
	int *Image,Phase,Phases,xdim,xdir,xloc,ydim,ydir,yloc;
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class JSS_BMP
{
public:
	D3DCOLOR *RawData;
	int *Buffer,Height,Width;
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class JSS_IMAGE : public JSS_BMP
{
public:
	int NoColor;
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class JSS_FONT
{
public:
	int ByteSize,FontAttr,FontSize,Height,OffSet[95],Width[95];
	char *FontName,*FontStr;
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class PaletteType
{
public:
	int Red,Green,Blue;
};

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

class DX3
{
private:
	D3DCOLOR *Buffer;
	HWND hWnd;
	IDirect3D9*	pDirect3D;
	IDirect3DDevice9* pDevice;
	int Pitch;
	PaletteType *Palette;

public:
	int Colors,Height,Images,UsedColors,Width;
	JSS_IMAGE *Image;

	DX3();
	~DX3();

	void Initialize(HWND W,int C = 256);
	void SetPalette(int i, int R, int G, int B);
	void SetMyPalette();
	void SetDefaultPalette();
	virtual void GameLoop();
	void StartFrame();
	void EndFrame();
	virtual void ComposeFrame();
	void PutPixel(int x, int y, int R, int G, int B);
	void CLS();
	virtual void Update();
	void FillScreen(int R, int G, int B);
	void PutPixel(int x, int y, int c);
	void FillScreen(int c);
	void Line(int x1, int y1, int x2, int y2, int R, int G, int B);
	void Line(int x1, int y1, int x2, int y2, int c);
	void Rectangle(int x1, int y1, int x2, int y2, int R, int G, int B);
	void Rectangle(int X1, int y1, int x2, int y2, int c);
	void FilledRectangle(int x1, int y1, int x2, int y2, int R, int G, int B);
	void FilledRectangle(int X1, int y1, int x2, int y2, int c);
	void Ellipse(int cx, int cy, int rx, int ry, int R, int G, int B);
	void Ellipse(int cx, int cy, int rx, int ry, int c);
	void FilledEllipse(int cx, int cy, int rx, int ry, int R, int G, int B);
	void FilledEllipse(int cx, int cy, int rx, int ry, int c);
	void Circle(int cx, int cy, int r, int R, int G, int B);
	void Circle(int cx, int cy, int r, int c);
	void FilledCircle(int cx, int cy, int r, int R, int G, int B);
	void FilledCircle(int cx, int cy, int r, int c);
	D3DCOLOR GetPixel(int x, int y, int &R, int &G, int &B);
	D3DCOLOR GetPixel(int x, int y, int &c);
	void GetPalette(int c, int &R, int &G, int &B);
	void LoadFont(JSS_FONT *Font, const char *FileName);
	void PrintFontAt(int x, int y, const char str[], JSS_FONT Font, int R, int G, int B);
	void PrintBkFontAt(int x, int y, const char str[], JSS_FONT Font, int R1, int G1, int B1, int R2, int G2, int B2);
	void PrintFontCenterAt(int x, int y, const char str[], JSS_FONT Font, int R, int G, int B);
	void PrintFontRightAt(int x, int y, const char str[], JSS_FONT Font, int R, int G, int B);
	void PrintFontAt(int x, int y, const char str[], JSS_FONT Font, int color);
	void PrintBkFontAt(int x, int y, const char str[], JSS_FONT Font, int TextColor, int BkColor);
	void PrintFontCenterAt(int x, int y, const char str[], JSS_FONT Font, int color);
	void PrintFontRightAt(int x, int y, const char str[], JSS_FONT Font, int color);
	int GetBMPColors(const char *FN);
	void LoadBMP(JSS_BMP *BMP, const char *FN);
	void DrawBMP(JSS_BMP *BMP, int x, int y, int DNC = -1, BOOL ByPalette = TRUE); // palette color NOT to draw, default is to draw everything
	void ProfilePalette();
	int GetPaletteIndex(int R, int G, int B);
	void AllocateImages(int I);
	void GetImage(JSS_IMAGE *Image, int x, int y, int Width, int Height, int NC);
	void GetImage(JSS_IMAGE *Image, JSS_BMP *BMP, int x, int y, int Width, int Height, int NC);
	void DrawSprite(JSS_SPRITE *Sprite, BOOL FromPalette = FALSE, BOOL DrawBackground = FALSE);
	BOOL Collision(JSS_SPRITE *S1, JSS_SPRITE *S2);
	void SaveScreen();
	int GetNumber(const char* Str, int j, int &k);
	void Log(const char* S);
};