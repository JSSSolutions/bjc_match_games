//
// DX3.cpp
//
// Written by John Sowell
//
// Written 3/2/2018 - 3/5/2018
//
// Implements the new DX3 DirectX engine
//
// 4/16/2020
//		Changed certain functions deemed no longer safe in Visual Studio 2017
//
// 1/29/2022
//		Added GetNumber function and re-wrote GetFont function to better read in
//		font files without fscanf_s functions.
//
// 7/5/2022
//		Added Log function, added condition in FillScreen function for if the
//		palette is not null
//
// 1/17/2024
//		Changed char parameters to const char parameters plus deleted pointers
//		more completely
//
#include "DX3.h"

// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

DX3::DX3()
{
	Buffer = NULL;

	Colors = 0;
	Height = 0;
	Image = NULL;
	Images = 0;
	Palette = NULL;
	Width = 0;
	hWnd = NULL;
	pDevice = NULL;
	pDirect3D = NULL;

}

// ****************************************************************************

void DX3::AllocateImages(int I)
{
	Image = new JSS_IMAGE[I];
	Images = I;
}

// ****************************************************************************

void DX3::Circle(int cx, int cy, int r, int R, int G, int B)
{
	Ellipse(cx,cy,r,r,R,G,B);
}

// ****************************************************************************

void DX3::Circle(int cx, int cy, int r, int c)
{
	Ellipse(cx,cy,r,r,c);
}

// ****************************************************************************

void DX3::CLS()
{
	memset(Buffer,0,4*Pitch*Height);
}

// ###########################################################################

BOOL DX3::Collision(JSS_SPRITE *S1, JSS_SPRITE *S2)
{
	if ((S1->Alive == FALSE) || (S2->Alive == FALSE))
		return FALSE;
	if ((S1->xloc >= S2->xloc) && (S1->xloc <= S2->xloc + S2->xdim) && (S1->yloc >= S2->yloc) && (S1->yloc <= S2->yloc + S2->ydim))
		return TRUE;
	if ((S1->xloc + S1->xdim >= S2->xloc) && (S1->xloc + S1->xdim <= S2->xloc + S2->xdim) && (S1->yloc >= S2->yloc) && (S1->yloc <= S2->yloc + S2->ydim))
		return TRUE;
	if ((S1->xloc >= S2->xloc) && (S1->xloc <= S2->xloc + S2->xdim) && (S1->yloc + S1->ydim >= S2->yloc) && (S1->yloc + S1->ydim <= S2->yloc + S2->ydim))
		return TRUE;
	if ((S1->xloc + S1->xdim >= S2->xloc) && (S1->xloc + S1->xdim <= S2->xloc + S2->xdim) && (S1->yloc + S1->ydim >= S2->yloc) && (S1->yloc + S1->ydim <= S2->yloc + S2->ydim))
		return TRUE;
	return FALSE;
}

// ****************************************************************************

void DX3::ComposeFrame()
{
	IDirect3DSurface9* pBackBuffer = NULL;
	D3DLOCKED_RECT rect;

	pDevice->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer);
	pBackBuffer->LockRect(&rect,NULL,NULL);
	memcpy(((D3DCOLOR*)rect.pBits),Buffer,4*Pitch*Height);
	pBackBuffer->UnlockRect();
	pBackBuffer->Release();
}

// ****************************************************************************

void DX3::DrawBMP(JSS_BMP *BMP,int x, int y, int DNC, BOOL ByPalette)
{
	BOOL Found;
	int B,G,I,II,P,R,XX,X1,X2,X3,YY;
	D3DCOLOR NoPixel;

	NoPixel = 4278190080;
	if ((DNC > -1) && (DNC < UsedColors))
	{
		R = Palette[DNC].Red;
		G = Palette[DNC].Green;
		B = Palette[DNC].Blue;
		NoPixel = D3DCOLOR_XRGB(R,G,B);
	}

	for (YY=0; YY<BMP->Height; YY++)
	{
		if (ByPalette == TRUE)
		{
			for (XX=0; XX<BMP->Width; XX++)
			{
				I = (YY*BMP->Width) + XX;
				P = BMP->Buffer[I];
				if (P != DNC)
					PutPixel(x+XX,y+YY,P);
			}
		}
		else
		{
			if ((DNC < 0) || (DNC >= UsedColors))
			{
				I = ((y+YY)*Pitch) + x;
				II = YY*BMP->Width;
				memcpy(&Buffer[I],&BMP->RawData[II],4*BMP->Width);
			}
			else
			{
				X3 = 0;
				while (X3 < BMP->Width)
				{
					X1 = -1;
					X2 = -1;
					Found = (BMP->RawData[(YY*BMP->Width)+X3] == NoPixel);
					while ((Found == TRUE) && (X3 < BMP->Width))
					{
						X3++;
						Found = (BMP->RawData[(YY*BMP->Width)+X3] == NoPixel);
					}
					if (X3 < BMP->Width)
					{
						X1 = X3;
						Found = (BMP->RawData[(YY*BMP->Width)+X3] != NoPixel);
						while ((Found == TRUE) && (X3 < BMP->Width))
						{
							X3++;
							Found = (BMP->RawData[(YY*BMP->Width)+X3] != NoPixel);
						}
						X2 = X3-1;
						I = (Pitch*(y+YY))+x+X1;
						II = (YY*BMP->Width)+X1;
						memcpy(&Buffer[I],&BMP->RawData[II],4*(X2-X1+1));
					}
				}
			}
		}
	}
}

// ****************************************************************************

void DX3::DrawSprite(JSS_SPRITE *Sprite, BOOL FromPalette, BOOL DrawBackground)
{
	int DNC,i,p = Sprite->Phase;

	i = Sprite->Image[p];
	if (DrawBackground == TRUE)
		DNC = -1;
	else
		DNC = Image[i].NoColor;
	DrawBMP(&Image[i],Sprite->xloc,Sprite->yloc,DNC,FromPalette);
}

// ****************************************************************************

void DX3::Ellipse(int cx, int cy, int rx, int ry, int R, int G, int B)
{
	double i,rxx,ryy,Theta,xx,xxx,yy,yyy;
	int x,y;

	rxx = (double)rx;
	ryy = (double)ry;
	for (i=0.0; i<360.0; i+=0.1)
	{
		Theta = (3.1415926536 / 180.0) * i;
		xx = rxx*cos(Theta)+0.5;
		yy = ryy*sin(Theta)+0.5;
		xxx = (double)cx + xx;
		yyy = (double)cy - yy;
		x = (int)xxx;
		y = (int)yyy;
		PutPixel(x,y,R,G,B);
	}
}

// ****************************************************************************

void DX3::Ellipse(int cx, int cy, int rx, int ry, int c)
{
	Ellipse(cx,cy,rx,ry,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::EndFrame()
{
	if (pDevice != NULL)
		pDevice->Present(NULL,NULL,NULL,NULL);
}

// ****************************************************************************

void DX3::FilledCircle(int cx, int cy, int r, int R, int G, int B)
{
	FilledEllipse(cx,cy,r,r,R,G,B);
}

// ****************************************************************************

void DX3::FilledCircle(int cx, int cy, int r, int c)
{
	FilledEllipse(cx,cy,r,r,c);
}

// ****************************************************************************

void DX3::FilledEllipse(int cx, int cy, int rx, int ry, int R, int G, int B)
{
	double i,rxx,ryy,Theta,xx,xxx,yy,yyy;
	int Adjy,*MaxX,*MinX,x,y;

	MaxX = new int[(2*ry)+1];
	MinX = new int[(2*ry)+1];
	rxx = (double)rx;
	ryy = (double)ry;
	Adjy = cy - ry;
	for (y=0; y<(2*ry)+1; y++)
	{
		MinX[y] = 3000;
		MaxX[y] = -1;
	}
	for (i=0.0; i<360.0; i+=0.1)
	{
		Theta = (3.1415926536 / 180.0) * i;
		xx = rxx*cos(Theta);
		if ((i<90) || (i>270))
			xx+=0.5;
		else
			xx-=0.5;
		yy = ryy*sin(Theta);
		if (i<180)
			yy+=0.5;
		else
			yy-=0.5;
		xxx = (double)cx + xx;
		yyy = (double)cy - yy;
		x = (int)xxx;
		y = (int)yyy;
		if (x < MinX[y-Adjy])
			MinX[y-Adjy] = x;
		if (x > MaxX[y-Adjy])
			MaxX[y-Adjy] = x;
	}

	for (y=0; y<(2*ry)+1; y++)
	{
		if ((MinX[y] < 3000) && (MaxX[y] > -1))
			Line(MinX[y],y+Adjy,MaxX[y],y+Adjy,R,G,B);
	}
}

// ****************************************************************************

void DX3::FilledEllipse(int cx, int cy, int rx, int ry, int c)
{
	FilledEllipse(cx,cy,rx,ry,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::FilledRectangle(int x1, int y1, int x2, int y2, int R, int G, int B)
{
	int y;

	for (y=y1; y<=y2; y++)
	{
		Line(x1,y,x2,y,R,G,B);
	}
}

// ****************************************************************************

void DX3::FilledRectangle(int x1, int y1, int x2, int y2, int c)
{
	FilledRectangle(x1,y1,x2,y2,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::FillScreen(int R, int G, int B)
{// Faster way to do this is to get an image of this and PutImage using memcpy
	int x,y;

	for (x=0; x<Width; x++)
	{
		for (y=0; y<Height; y++)
		{
			PutPixel(x,y,R,G,B);
		}
	}
}

// ****************************************************************************

void DX3::FillScreen(int c)
{
	if (Palette != NULL)
		FillScreen(Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::GameLoop()
{
}

// ############################################################################

int DX3::GetBMPColors(const char *FN)
{
	BOOL Found;
	FILE *infile; 
	FILE *outfile; 
	int B,bits,colors,G,height,i,ix,pitch,R,width,x;
	PaletteType *BMPPalette;
	UCHAR ch,ch1,ch2,Header[1078];
	
	fopen_s(&infile, FN, "rb");
	fopen_s(&outfile, "BMPs.txt", "a");

	if (infile == NULL)
		return 0;

	colors = 0;
	for (x=0; x<54; x++)
	{
		Header[x] = fgetc(infile);
	}

	width = (int)Header[18];
	width += (256*(int)Header[19]);
	width += (65536*(int)Header[20]);
	height = (int)Header[22];
	height += (256*(int)Header[23]);
	height += (65536*(int)Header[24]);
	
	bits = (int)Header[28];
	BMPPalette = NULL;
	if (bits == 4)
		BMPPalette = new PaletteType[16];
	if (bits == 8)
		BMPPalette = new PaletteType[256];
	if (bits == 24)
		BMPPalette = new PaletteType[16777216]; // the screen can only hold but so many colors at a time
	if ((bits != 4) && (bits != 8) && (bits != 24)) return 0;
	if (bits == 4)
	{
		for (x=54; x<118; x++)
		{
			Header[x] = fgetc(infile);
		}
		pitch = ((width+7) / 8) * 4;
		ix = 0;
		for (x=0; x<height*pitch; x++)
		{
			ch = fgetc(infile);
			ch1 = ch / 16;
			ch2 = ch % 16;
			if (ix < width)
			{
				R = (int)Header[(4*ch1)+56];
				G = (int)Header[(4*ch1)+55];
				B = (int)Header[(4*ch1)+54];
				Found = FALSE;
				for (i=0; i<colors; i++)
				{
					if ((BMPPalette[i].Red == R) && (BMPPalette[i].Green == G) && (BMPPalette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				if (Found == FALSE)
				{
					BMPPalette[colors].Red = R;
					BMPPalette[colors].Green = G;
					BMPPalette[colors++].Blue = B;
				}
				ix++;
			}

			if (ix < width)
			{
				R = (int)Header[(4*ch2)+56];
				G = (int)Header[(4*ch2)+55];
				B = (int)Header[(4*ch2)+54];
				Found = FALSE;
				for (i=0; i<colors; i++)
				{
					if ((BMPPalette[i].Red == R) && (BMPPalette[i].Green == G) && (BMPPalette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				if (Found == FALSE)
				{
					BMPPalette[colors].Red = R;
					BMPPalette[colors].Green = G;
					BMPPalette[colors++].Blue = B;
				}
				ix++;
			}
			if (x % pitch == pitch-1)
				ix = 0;
		}
	}
	
	if (bits == 8)
	{
		for (x=54; x<1078; x++)
		{
			Header[x] = fgetc(infile);
		}
		
		pitch = ((width+3) / 4) * 4;
		ix = 0;
		for (x=0; x<pitch*height; x++)
		{
			ch = fgetc(infile);
			if (ix < width)
			{
				R = (int)Header[(4*ch)+56];
				G = (int)Header[(4*ch)+55];
				B = (int)Header[(4*ch)+54];
				Found = FALSE;
				for (i=0; i<colors; i++)
				{
					if ((BMPPalette[i].Red == R) && (BMPPalette[i].Green == G) && (BMPPalette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				if (Found == FALSE)
				{
					BMPPalette[colors].Red = R;
					BMPPalette[colors].Green = G;
					BMPPalette[colors++].Blue = B;
				}
			}
			ix++;
			if (x % pitch == pitch-1)
				ix = 0;
		}
	}

	if (bits == 24)
	{
		pitch = (((width*3) + 3) / 4) * 4;
		R = -1;
		G = -1;
		B = -1;
		x = 0;
		while (x<pitch*height)
		{
			if (x%pitch < width*3)
			{
				ch = fgetc(infile);
				B = (int)ch;
				ch = fgetc(infile);
				G = (int)ch;
				ch = fgetc(infile);
				R = (int)ch;
				Found = FALSE;
				for (i=0; i<colors; i++)
				{
					if ((BMPPalette[i].Red == R) && (BMPPalette[i].Green == G) && (BMPPalette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				if (Found == FALSE)
				{
					BMPPalette[colors].Red = R;
					BMPPalette[colors].Green = G;
					BMPPalette[colors++].Blue = B;
				}

				x+=3;
			}
			else
			{
				ch = fgetc(infile);
				x++;
			}
		}
	}
	
	fclose(infile);
	fprintf(outfile,"================================================================\n\n");
	fprintf(outfile,"DATA FOR %s:\n\n",FN);
	fprintf(outfile,"Dimensions: %d X %d\n",width,height);
	fprintf(outfile,"Bits: %d\n",bits);
	fprintf(outfile,"Colors: %d\n\n",colors);
	fprintf(outfile,"PALETTE:\n\n");
	for (x=0; x<colors; x++)
	{
		fprintf(outfile,"%d: RGB(%d,%d,%d)\n",x,BMPPalette[x].Red,BMPPalette[x].Green,BMPPalette[x].Blue);
	}
	fprintf(outfile,"\n");
	fclose(outfile);
	delete[] BMPPalette;
	return colors;
}

// ****************************************************************************

void DX3::GetImage(JSS_IMAGE *Image,int x, int y, int Width, int Height, int NC)
{
	D3DCOLOR C;
	int B,G,I,P,R,X,Y;

	Image->Width = Width;
	Image->Height = Height;
	Image->NoColor = NC;
	Image->Buffer = new int[Width*Height];
	Image->RawData = new D3DCOLOR[Width*Height];

	for (X=x; X<x+Width; X++)
	{
		for (Y=y; Y<y+Height; Y++)
		{
			C = GetPixel(X,Y,P);
			I = ((Y-y)*Width) + X - x;
			Image->RawData[I] = C;
			if (P < 0)
			{
				P = UsedColors;
				GetPixel(X,Y,R,G,B);
				SetPalette(UsedColors++,R,G,B);
			}
			Image->Buffer[I] = P;
		}
	}
}

// ****************************************************************************

void DX3::GetImage(JSS_IMAGE *Image, JSS_BMP *BMP, int x, int y, int Width, int Height, int NC)
{
	int I,II,Y;

	Image->Width = Width;
	Image->Height = Height;
	Image->NoColor = NC;
	Image->Buffer = new int[Width*Height];
	Image->RawData = new D3DCOLOR[Width*Height];

	for (Y=0; Y<Height; Y++)
	{
		I = ((y+Y) * BMP->Width) + x;
		II = (Y*Image->Width);
		memcpy(&Image->Buffer[II],&BMP->Buffer[I],4*Image->Width);
		memcpy(&Image->RawData[II],&BMP->RawData[I],4*Image->Width);
	}
}

// ############################################################################

int DX3::GetNumber(const char* Str, int j, int& k)
{
	char buff[80], ch;
	int l, ll, N;

	l = StrLen(Str);
	if (j >= l)
		return 0;

	ll = 0;
	k = j;
	while (k < l)
	{
		ch = Str[k];
		if ((ch < 48) || (ch > 57))
			break;
		else
			buff[ll++] = ch;
		k++;
	}

	buff[ll] = 0;
	N = atoi(buff);
	return N;
}

// ****************************************************************************

void DX3::GetPalette(int c, int &R, int &G, int &B)
{
	R = Palette[c].Red;
	G = Palette[c].Green;
	B = Palette[c].Blue;
}

// ############################################################################

int DX3::GetPaletteIndex(int R, int G, int B)
{
	int i = -1;

	if (UsedColors == 0)
		return i;

	for (i=0; i<UsedColors; i++)
	{
		if ((R == Palette[i].Red) && (G == Palette[i].Green) && (B == Palette[i].Blue))
			return i;
	}
	
	return -1;
}

// ############################################################################

D3DCOLOR DX3::GetPixel(int x, int y, int &R, int &G, int &B)
{
	D3DCOLOR C = Buffer[(y*Pitch)+x];
	int c;

	c = C - 4278190080;
	R = c / 65536;
	G = (c % 65536) / 256;
	B = c % 256;

	return C;;
}

// ############################################################################

D3DCOLOR DX3::GetPixel(int x, int y, int &c)
{
	D3DCOLOR C;
	int B,G,i,R;

	C = GetPixel(x,y,R,G,B);
	c = -1;
	for (i=0; i<256; i++)
	{
		if ((Palette[i].Red == R) && (Palette[i].Green == G) && (Palette[i].Blue == B))
		{
			c = i;
			break;
		}
	}

	return C;
}

// ****************************************************************************

void DX3::Initialize(HWND W, int C)
{
	RECT rc;
	pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
    D3DPRESENT_PARAMETERS d3dpp;
	
	hWnd = W;
	GetClientRect(hWnd,&rc);
	Width = rc.right - rc.left;
	Pitch = ((Width + 31) / 32) * 32;
	Height = rc.bottom - rc.top;
	Colors = C;
	Buffer = new D3DCOLOR[Pitch*Height];

	Palette = new PaletteType[Colors];
	UsedColors = 0;
	if (Colors == 256)
	{
		SetDefaultPalette();
		UsedColors = Colors;
	}
	if (Colors == 28)
	{
		SetMyPalette();
		UsedColors = Colors;
	}
	if (Colors == 0) // not using a palette, in other words
		Palette = NULL;

    ZeroMemory(&d3dpp,sizeof(d3dpp));
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;
	d3dpp.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;
    pDirect3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE,&d3dpp,&pDevice);
}

// ****************************************************************************

void DX3::Line(int x1, int y1, int x2, int y2, int R, int G, int B)
{
	int x,xx1,xx2,y,yy1,yy2;
	float xxx1,xxx2,xxx,yyy1,yyy2,yyy,Slope;

	if ((x1 == x2) && (y1 == y2)) // single pixel!
	{
		PutPixel(x1,y1,R,G,B);
		return;
	}

	if (x1 == x2) // vertical line
	{
		x = x1;
		if (y1 < y2)
		{
			yy1 = y1;
			yy2 = y2;
		}
		else
		{
			yy1 = y2;
			yy2 = y1;
		}
		for (y=yy1; y<=yy2; y++)
		{
			PutPixel(x,y,R,G,B);
		}
		return;
	}

	if (y1 == y2) // horizontal line
	{
		y = y1;
		if (x1 < x2)
		{
			xx1 = x1;
			xx2 = x2;
		}
		else
		{
			xx1 = x2;
			xx2 = x1;
		}
		for (x=xx1; x<=xx2; x++)
		{
			PutPixel(x,y,R,G,B);
		}
		return;
	}

	xxx1 = (float)x1;
	xxx2 = (float)x2;
	yyy1 = (float)y1;
	yyy2 = (float)y2;
	Slope = (yyy2-yyy1) / (xxx2-xxx1);
	if ((Slope < 1) && (Slope > -1))
	{
		xxx = xxx1;
		yyy = yyy1;
		PutPixel((int)xxx,(int)yyy,R,G,B);
		if (xxx1 < xxx2)
		{
			while (xxx < xxx2)
			{
				xxx += 1.0;
				yyy += Slope;
				PutPixel((int)xxx,(int)yyy,R,G,B);
			}
		}

		if (xxx1 > xxx2)
		{
			while (xxx > xxx2)
			{
				xxx -= 1.0;
				yyy -= Slope;
				PutPixel((int)xxx,(int)yyy,R,G,B);
			}
		}
	}
	else
	{
		Slope = (float)1.0 / Slope;
		xxx = xxx1;
		yyy = yyy1;
		PutPixel((int)xxx,(int)yyy,R,G,B);
		if (yyy1 < yyy2)
		{
			while (yyy < yyy2)
			{
				yyy += 1.0;
				xxx += Slope;
				PutPixel((int)xxx,(int)yyy,R,G,B);
			}
		}

		if (yyy1 > yyy2)
		{
			while (yyy > yyy2)
			{
				yyy -= 1.0;
				xxx -= Slope;
				PutPixel((int)xxx,(int)yyy,R,G,B);
			}
		}
	}
}

// ****************************************************************************

void DX3::Line(int x1, int y1, int x2, int y2, int c)
{
	Line(x1,y1,x2,y2,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::LoadBMP(JSS_BMP *BMP, const char *FN)
{
	BOOL Found;
	FILE *infile; 
	int B,bits,G,height,i,I,pitch,R,width,x,X,Y;
	UCHAR ch,ch1,ch2,Header[1078];

	fopen_s(&infile, FN, "rb");
	if (infile == NULL)
		return;

	for (x=0; x<54; x++)
	{
		Header[x] = fgetc(infile);
	}

	width = (int)Header[18];
	width += (256*(int)Header[19]);
	width += (65536*(int)Header[20]);
	BMP->Width = width;
	height = (int)Header[22];
	height += (256*(int)Header[23]);
	height += (65536*(int)Header[24]);
	BMP->Height = height;
	BMP->Buffer = new int[width*height];
	BMP->RawData = new D3DCOLOR[width*height];
	
	bits = (int)Header[28];
	if (bits == 4)
	{
		for (x=54; x<118; x++)
		{
			Header[x] = fgetc(infile);
		}
		pitch = ((width+7) / 8) * 4;
		X = 0;
		Y = height-1;
		for (x=0; x<height*pitch; x++)
		{
			ch = fgetc(infile);
			ch1 = ch / 16;
			ch2 = ch % 16;
			if (X < width)
			{
				R = (int)Header[(4*ch1)+56];
				G = (int)Header[(4*ch1)+55];
				B = (int)Header[(4*ch1)+54];
				Found = FALSE;
				for (i=0; i<UsedColors; i++)
				{
					if ((Palette[i].Red == R) && (Palette[i].Green == G) && (Palette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				I = (Y*width)+X;
				BMP->Buffer[I] = i;
				BMP->RawData[I] = D3DCOLOR_XRGB(R,G,B);
				if (Found == FALSE)
				{
					Palette[UsedColors].Red = R;
					Palette[UsedColors].Green = G;
					Palette[UsedColors++].Blue = B;
				}
				X++;
			}

			if (X < width)
			{
				R = (int)Header[(4*ch2)+56];
				G = (int)Header[(4*ch2)+55];
				B = (int)Header[(4*ch2)+54];
				Found = FALSE;
				for (i=0; i<UsedColors; i++)
				{
					if ((Palette[i].Red == R) && (Palette[i].Green == G) && (Palette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				I = (Y*width)+X;
				BMP->Buffer[I] = i;
				BMP->RawData[I] = D3DCOLOR_XRGB(R,G,B);
				if (Found == FALSE)
				{
					Palette[UsedColors].Red = R;
					Palette[UsedColors].Green = G;
					Palette[UsedColors++].Blue = B;
				}
				X++;
			}
			if (x % pitch == pitch-1)
			{
				X = 0;
				Y--;
			}
		}
	}
	
	if (bits == 8)
	{
		for (x=54; x<1078; x++)
		{
			Header[x] = fgetc(infile);
		}
		
		pitch = ((width+3) / 4) * 4;
		X = 0;
		Y = height-1;
		for (x=0; x<pitch*height; x++)
		{
			ch = fgetc(infile);
			if (X < width)
			{
				R = (int)Header[(4*ch)+56];
				G = (int)Header[(4*ch)+55];
				B = (int)Header[(4*ch)+54];
				Found = FALSE;
				for (i=0; i<UsedColors; i++)
				{
					if ((Palette[i].Red == R) && (Palette[i].Green == G) && (Palette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				I = (width*Y)+X;
				BMP->Buffer[I] = i;
				BMP->RawData[I] = D3DCOLOR_XRGB(R,G,B);
				if (Found == FALSE)
				{
					Palette[UsedColors].Red = R;
					Palette[UsedColors].Green = G;
					Palette[UsedColors++].Blue = B;
				}
			}
			X++;
			if (x % pitch == pitch-1)
			{
				X = 0;
				Y--;
			}
		}
	}

	if (bits == 24)
	{
		pitch = (((width*3) + 3) / 4) * 4;
		R = -1;
		G = -1;
		B = -1;
		x = 0;
		X = 0;
		Y = height-1;
		while (x<pitch*height)
		{
			if (x%pitch < width*3)
			{
				ch = fgetc(infile);
				B = (int)ch;
				ch = fgetc(infile);
				G = (int)ch;
				ch = fgetc(infile);
				R = (int)ch;
				Found = FALSE;
				for (i=0; i<UsedColors; i++)
				{
					if ((Palette[i].Red == R) && (Palette[i].Green == G) && (Palette[i].Blue == B))
					{
						Found = TRUE;
						break;
					}
				}
				I = (Y*width) + X;
				BMP->Buffer[I] = i;
				BMP->RawData[I] = D3DCOLOR_XRGB(R,G,B);
				if (Found == FALSE)
				{
					Palette[UsedColors].Red = R;
					Palette[UsedColors].Green = G;
					Palette[UsedColors++].Blue = B;
				}
				X++;
				if (X == width)
				{
					X = 0;
					Y--;
				}
				x+=3;
			}
			else
			{
				ch = fgetc(infile);
				x++;
			}
		}
	}
	fclose(infile);
}

// ****************************************************************************

void DX3::LoadFont(JSS_FONT *Font, const char *FileName)
{
	FILE *infile;
	int A,B,H,i,l,ll,O,S,W,x,xx,y;
	char buf[80];

	fopen_s(&infile,FileName,"r");
	if (!infile)
		return;
	else
	{
		x = 0;
		fgets(buf,81,infile);
		l = StrLen(buf);
		buf[l-1] = 0;
		Font->FontName = (char *)malloc((l-1)*sizeof(char));
		for (i = 0; i < l - 1; i++) 
		{
			Font->FontName[i] = buf[i];
		} 
		Font->FontName[l-1] = 0;
		fgets(buf, 81, infile);
		l = StrLen(buf);
		buf[l - 1] = 0;
		
		S = GetNumber(buf, 0, i);
		H = GetNumber(buf, i + 1, i);
		A = GetNumber(buf, i + 1, i);
		B = GetNumber(buf, i + 1, i);
		Font->FontSize = S;
		Font->Height = H;
		Font->FontAttr = A;
		Font->ByteSize = B;
		Font->FontStr = (char *)malloc(Font->ByteSize * sizeof(char));
		x = 0;
		ll = 0;
		while (x!=126)
		{
			fgets(buf, 81, infile);
			l = StrLen(buf);
			buf[l - 1] = 0;
			
			x = GetNumber(buf, 0, i);
			W = GetNumber(buf, i + 1, i);
			O = GetNumber(buf, i + 1, i);
			Font->Width[x - 32] = W;
			Font->OffSet[x - 32] = O;
			for (y=0; y<Font->Height; y++)
			{
				fgets(buf, 81, infile);
				l = StrLen(buf);
				buf[l-1] = 0;
				for (xx=0; xx<W; xx++)
				{
					Font->FontStr[ll++] = buf[xx];
				}
			}
		}
		Font->FontStr[ll] = 0;  // null terminator
		fclose(infile);
	}
}

// ****************************************************************************

void DX3::Log(const char* S)
{
	FILE* outfile;
	time_t Time;
	struct tm TT;

	Time = time(0);
	localtime_s(&TT, &Time);
	fopen_s(&outfile, "LogFile.txt", "a");
	fprintf(outfile, "%d/%d/%d %d:%02d:%02d ---> %s\n", TT.tm_mon + 1, TT.tm_mday, TT.tm_year + 1900, TT.tm_hour, TT.tm_min, TT.tm_sec, S);
	fclose(outfile);
}

// ****************************************************************************

void DX3::PrintBkFontAt(int x, int y, const char str[], JSS_FONT Font, int R1, int G1, int B1, int R2, int G2, int B2)
{
	int o,l,xx,xxx,yyy;
	unsigned char ch;
	
	l = StrLen(str);
	for (xx=0; xx<l; xx++)
	{
		ch = str[xx];
		o = Font.OffSet[ch-32];
		for (yyy=y; yyy<(y+Font.Height); yyy++)
		{
			for (xxx=x; xxx<(x+Font.Width[ch-32]); xxx++)
			{
				if (Font.FontStr[o++] == 49) 
					PutPixel(xxx,yyy,R1,G1,B1);
				else 
					PutPixel(xxx,yyy,R2,G2,B2);
			}
		}
		x += Font.Width[ch-32];
	}
}

// ****************************************************************************

void DX3::PrintBkFontAt(int x, int y, const char str[], JSS_FONT Font, int TC, int BKC)
{
	PrintBkFontAt(x,y,str,Font,Palette[TC].Red,Palette[TC].Green,Palette[TC].Blue,Palette[BKC].Red,Palette[BKC].Green,Palette[BKC].Blue);
}

// ****************************************************************************

void DX3::PrintFontAt(int x, int y, const char str[], JSS_FONT Font, int R, int G, int B)
{
	int o,l,xx,xxx,yyy;
	unsigned char ch;
	
	l = StrLen(str);
	for (xx=0; xx<l; xx++)
	{
		ch = str[xx];
		o = Font.OffSet[ch-32];
		for (yyy=y; yyy<(y+Font.Height); yyy++)
		{
			for (xxx=x; xxx<(x+Font.Width[ch-32]); xxx++)
			{
				if (Font.FontStr[o++] == 49) 
				{
					PutPixel(xxx,yyy,R,G,B);
				}
			}
		}
		x += Font.Width[ch-32];
	}
}

// ****************************************************************************

void DX3::PrintFontAt(int x, int y, const char str[], JSS_FONT Font, int c)
{
	PrintFontAt(x,y,str,Font,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::PrintFontCenterAt(int x, int y, const char str[], JSS_FONT Font, int R, int G, int B)
{
	int xl,fl,l,i,index;
	
	l = StrLen(str);
	fl = 0;
	for (index=0; index<l; index++)
	{
		i = str[index] - 32;
		fl += Font.Width[i];
	}
	xl = x-(fl/2);
	PrintFontAt(xl,y,str,Font,R,G,B);
}

// ****************************************************************************

void DX3::PrintFontCenterAt(int x, int y, const char str[], JSS_FONT Font, int c)
{
	PrintFontCenterAt(x,y,str,Font,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::PrintFontRightAt(int x, int y, const char str[], JSS_FONT Font, int R, int G, int B)
{
	int xl,fl,l,i,index;
	
	l = StrLen(str);
	fl = 0;
	for (index=0; index<l; index++)
	{
		i = str[index] - 32;
		fl += Font.Width[i];
	}
	xl = x-fl;
	PrintFontAt(xl,y,str,Font,R,G,B);
}

// ****************************************************************************

void DX3::PrintFontRightAt(int x, int y, const char str[], JSS_FONT Font, int c)
{
	PrintFontRightAt(x,y,str,Font,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::ProfilePalette()
{
	FILE *outfile; 
	fopen_s(&outfile, "Palette.txt", "w");
	int x;

	for (x=0; x<UsedColors; x++)
	{
		fprintf(outfile,"%d: RGB(%d,%d,%d)\n",x,Palette[x].Red,Palette[x].Green,Palette[x].Blue);
	}
	fclose(outfile);
}

// ****************************************************************************

void DX3::PutPixel(int x, int y, int R, int G, int B)
{
	int I = (y*Pitch) + x;

	if ((x<0) || (x>=Width) || (y<0) || (y>=Height))
		return;
	Buffer[I] = D3DCOLOR_XRGB(R,G,B);
}

// ****************************************************************************

void DX3::PutPixel(int x, int y, int c)
{
	PutPixel(x,y,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::Rectangle(int x1, int y1, int x2, int y2, int R, int G, int B)
{
	Line(x1,y1,x2,y1,R,G,B);
	Line(x1,y1,x1,y2,R,G,B);
	Line(x2,y2,x1,y2,R,G,B);
	Line(x2,y2,x2,y1,R,G,B);
}

// ****************************************************************************

void DX3::Rectangle(int x1, int y1, int x2, int y2, int c)
{
	Rectangle(x1,y1,x2,y2,Palette[c].Red,Palette[c].Green,Palette[c].Blue);
}

// ****************************************************************************

void DX3::SaveScreen()
{
	char FN[80];
	FILE *infile,*outfile;
	int adjx,B,G,MemSize,pitch,R,x,y;
	UCHAR Header[54];

	for (x=0; x<54; x++)
	{
		Header[x] = 0;
	}
	Header[0] = 66;
	Header[1] = 77;
	Header[10] = 54;
	Header[14] = 40;
	Header[18] = Width % 256;
	Header[19] = Width / 256;
	Header[22] = Height % 256;
	Header[23] = Height / 256;
	Header[26] = 1;

	pitch = (((Width*3)+3)/4)*4;
	adjx = pitch - (3*Width);
	MemSize = (pitch*Height) + 54;
	Header[2] = MemSize % 256;
	Header[3] = (MemSize / 256) % 256;
	Header[4] = MemSize / 65536;
	Header[28] = 24;
	Header[34] = pitch % 256;
	Header[35] = pitch / 256;

	x = 1;
	sprintf_s(FN,"Screen %d.bmp",x);
	fopen_s(&infile,FN,"r");
	while (infile != NULL)
	{
		fclose(infile);
		x++;
		sprintf_s(FN,"Screen %d.bmp",x);
		fopen_s(&infile,FN,"r");
	}

	fopen_s(&outfile,FN,"wb");
	for (x=0; x<54; x++)
	{
		fprintf(outfile,"%c",Header[x]);
	}
	for(y=Height-1; y>-1; y--)
	{
		for (x=0; x<Width; x++)
		{
			GetPixel(x,y,R,G,B);
			fprintf(outfile,"%c%c%c",B,G,R);
		}
		for (x=0; x<adjx; x++)
		{
			fprintf(outfile,"%c",0);
		}
	}
	fclose(outfile);
}

// ****************************************************************************

void DX3::SetDefaultPalette()
{
	SetPalette(0,0,0,0);		// black			10
	SetPalette(1,0,0,128);	
	SetPalette(2,0,128,0);		// dk green			0
	SetPalette(3,0,128,128);	// dk cyan			20
	SetPalette(4,128,0,0);		// dk red/maroon	13
	SetPalette(5,128,0,128);	// dk purple		8
	SetPalette(6,128,128,0);	// brown			5
	SetPalette(7,192,192,192);	// lt gray			7
	SetPalette(8,128,128,128);  
	SetPalette(9,0,0,255);		// blue				3
	SetPalette(10,0,255,0);		// lt green			4
	SetPalette(11,0,255,255);	// lt cyan			12
	SetPalette(12,255,0,0);		// red				1
	SetPalette(13,255,0,255);
	SetPalette(14,255,255,0);	// yellow			2
	SetPalette(15,255,255,255);	// white			9
	SetPalette(16,255,255,192); // beige			15
	SetPalette(17,255,255,128);
	SetPalette(18,255,255,64);
	SetPalette(19,255,224,255);
	SetPalette(20,255,224,192);
	SetPalette(21,255,224,128);
	SetPalette(22,255,224,64);
	SetPalette(23,255,224,0);
	SetPalette(24,255,192,255);
	SetPalette(25,255,192,192);
	SetPalette(26,255,192,128);
	SetPalette(27,255,192,64);
	SetPalette(28,255,192,0);
	SetPalette(29,255,160,255);
	SetPalette(30,255,160,192);
	SetPalette(31,255,160,128);
	SetPalette(32,255,160,64);
	SetPalette(33,255,160,0);
	SetPalette(34,255,128,255);
	SetPalette(35,255,128,192);
	SetPalette(36,255,128,128);	// pink				11
	SetPalette(37,255,128,64);
	SetPalette(38,255,128,0);	// orange			6
	SetPalette(39,255,96,255);
	SetPalette(40,255,96,192);
	SetPalette(41,255,96,128);
	SetPalette(42,255,96,64);
	SetPalette(43,255,96,0);
	SetPalette(44,255,64,255);
	SetPalette(45,255,64,192);
	SetPalette(46,255,64,128);
	SetPalette(47,255,64,64);
	SetPalette(48,255,64,0);
	SetPalette(49,255,32,255);
	SetPalette(50,255,32,192);
	SetPalette(51,255,32,128);
	SetPalette(52,255,32,64);
	SetPalette(53,255,32,0);
	SetPalette(54,255,0,192);
	SetPalette(55,255,0,128);	// shocking pink	16
	SetPalette(56,255,0,64);
	SetPalette(57,192,255,255);
	SetPalette(58,192,255,192);
	SetPalette(59,192,255,128);
	SetPalette(60,192,255,64);
	SetPalette(61,192,255,0);
	SetPalette(62,192,224,255);
	SetPalette(63,192,224,192);
	SetPalette(64,192,224,128);
	SetPalette(65,192,224,64);
	SetPalette(66,192,224,0);
	SetPalette(67,192,192,255);
	SetPalette(68,192,192,128);
	SetPalette(69,192,192,64);
	SetPalette(70,192,192,0);
	SetPalette(71,192,160,255);
	SetPalette(72,192,160,192);
	SetPalette(73,192,160,128);
	SetPalette(74,192,160,64);
	SetPalette(75,192,160,0);
	SetPalette(76,192,128,255);
	SetPalette(77,192,128,192);
	SetPalette(78,192,128,128);
	SetPalette(79,192,128,64);
	SetPalette(80,192,128,0);
	SetPalette(81,192,96,255);
	SetPalette(82,192,96,192);
	SetPalette(83,192,96,128);
	SetPalette(84,192,96,64);
	SetPalette(85,192,96,0);
	SetPalette(86,192,64,255);
	SetPalette(87,192,64,192);
	SetPalette(88,192,64,128);
	SetPalette(89,192,64,64);
	SetPalette(90,192,64,0);
	SetPalette(91,192,32,255);
	SetPalette(92,192,32,192);
	SetPalette(93,192,32,128);
	SetPalette(94,192,32,64);
	SetPalette(95,192,32,0);
	SetPalette(96,192,0,255);
	SetPalette(97,192,0,192);
	SetPalette(98,192,0,128);
	SetPalette(99,192,0,64);
	SetPalette(100,192,0,0);
	SetPalette(101,128,255,255);
	SetPalette(102,128,255,192);
	SetPalette(103,128,255,128);
	SetPalette(104,128,255,64);
	SetPalette(105,128,255,0);
	SetPalette(106,128,224,255);
	SetPalette(107,128,224,192);
	SetPalette(108,128,224,128);
	SetPalette(109,128,224,64);
	SetPalette(110,128,224,0);
	SetPalette(111,128,192,255);
	SetPalette(112,128,192,128);
	SetPalette(113,128,192,64);
	SetPalette(114,128,192,0);
	SetPalette(115,128,160,255);
	SetPalette(116,128,160,192);
	SetPalette(117,128,160,128);
	SetPalette(118,128,160,64);
	SetPalette(119,128,160,0);
	SetPalette(120,128,128,255);	// lt blue		14
	SetPalette(121,128,128,192);
	SetPalette(122,128,128,64);
	SetPalette(123,128,96,255);
	SetPalette(124,128,96,192);
	SetPalette(125,128,96,128);
	SetPalette(126,128,96,64);
	SetPalette(127,128,96,0);
	SetPalette(128,128,64,255);
	SetPalette(129,128,64,192);
	SetPalette(130,128,64,128);
	SetPalette(131,128,64,64);
	SetPalette(132,128,64,0);
	SetPalette(133,128,0,255);
	SetPalette(134,128,0,192);
	SetPalette(135,128,0,64);
	SetPalette(136,64,255,255);
	SetPalette(137,64,255,192);
	SetPalette(138,64,255,128);
	SetPalette(139,64,255,64);
	SetPalette(140,64,255,0);
	SetPalette(141,64,224,255);
	SetPalette(142,64,224,192);
	SetPalette(143,64,224,128);
	SetPalette(144,64,224,64);
	SetPalette(145,64,224,0);
	SetPalette(146,64,192,255);
	SetPalette(147,64,192,192);
	SetPalette(148,64,192,128);
	SetPalette(149,64,192,64);
	SetPalette(150,64,192,0);
	SetPalette(151,64,160,255);
	SetPalette(152,64,160,192);
	SetPalette(153,64,160,128);
	SetPalette(154,64,160,64);
	SetPalette(155,64,160,0);
	SetPalette(156,64,128,255);
	SetPalette(157,64,128,192);
	SetPalette(158,64,128,128);		// teal			22
	SetPalette(159,64,128,64);
	SetPalette(160,64,128,0);
	SetPalette(161,64,96,255);
	SetPalette(162,64,96,192);
	SetPalette(163,64,96,128);
	SetPalette(164,64,96,64);
	SetPalette(165,64,96,0);
	SetPalette(166,64,64,255);
	SetPalette(167,64,64,192);
	SetPalette(168,64,64,128);
	SetPalette(169,64,64,64);
	SetPalette(170,64,64,0);
	SetPalette(171,64,32,255);
	SetPalette(172,64,32,192);
	SetPalette(173,64,32,128);
	SetPalette(174,64,32,64);
	SetPalette(175,64,32,0);
	SetPalette(176,64,0,255);
	SetPalette(177,64,0,192);
	SetPalette(178,64,0,128);		// indigo		17
	SetPalette(179,64,0,64);
	SetPalette(180,64,0,0);
	SetPalette(181,0,255,192);
	SetPalette(182,0,255,128);
	SetPalette(183,0,255,64);
	SetPalette(184,0,224,255);
	SetPalette(185,0,224,192);
	SetPalette(186,0,224,128);
	SetPalette(187,0,224,64);
	SetPalette(188,0,224,0);
	SetPalette(189,0,192,255);
	SetPalette(190,0,192,192);
	SetPalette(191,0,192,128);
	SetPalette(192,0,192,64);
	SetPalette(193,0,192,0);
	SetPalette(194,0,160,255);
	SetPalette(195,0,160,192);
	SetPalette(196,0,160,128);
	SetPalette(197,0,160,64);
	SetPalette(198,0,160,0);
	SetPalette(199,0,128,255);
	SetPalette(200,0,128,192);
	SetPalette(201,0,128,64);
	SetPalette(202,0,96,255);
	SetPalette(203,0,96,192);
	SetPalette(204,0,96,128);
	SetPalette(205,0,96,64);
	SetPalette(206,0,96,0);
	SetPalette(207,0,64,255);
	SetPalette(208,0,64,192);
	SetPalette(209,0,64,128);
	SetPalette(210,0,64,64);
	SetPalette(211,0,64,0);
	SetPalette(212,0,32,255);
	SetPalette(213,0,32,192);
	SetPalette(214,0,32,128);
	SetPalette(215,0,32,64);
	SetPalette(216,0,32,0);
	SetPalette(217,0,0,192);
	SetPalette(218,0,0,64);
	SetPalette(219,0,255,216);
	SetPalette(220,216,0,255);
	SetPalette(221,85,85,85);
	SetPalette(222,85,85,170);
	SetPalette(223,85,85,255);
	SetPalette(224,85,170,85);
	SetPalette(225,85,170,170);
	SetPalette(226,85,170,255);
	SetPalette(227,85,255,85);
	SetPalette(228,85,255,170);
	SetPalette(229,85,255,255);
	SetPalette(230,170,85,85);
	SetPalette(231,170,85,170);
	SetPalette(232,170,85,255);
	SetPalette(233,170,170,85);
	SetPalette(234,170,170,170);
	SetPalette(235,170,170,255);
	SetPalette(236,170,255,85);
	SetPalette(237,170,255,170);
	SetPalette(238,170,255,255);
	SetPalette(239,255,85,85);
	SetPalette(240,255,85,170);
	SetPalette(241,255,85,255);
	SetPalette(242,255,170,85);
	SetPalette(243,255,170,170);
	SetPalette(244,255,170,255);
	SetPalette(245,255,255,85);
	SetPalette(246,255,255,170);
	SetPalette(247,248,128,112);	// salmon		18
	SetPalette(248,240,160,96);		// sandy brown	19
	SetPalette(249,104,104,104);	// dk gray		21
	SetPalette(250,220,160,220);	// plum			23
	SetPalette(251,84,108,48);		// olive		24
	SetPalette(252,255,224,224);	// misty rose	25
	SetPalette(253,128,255,144);	// faded green	26
	SetPalette(254,255,216,0);		// gold			27
	SetPalette(255,204,204,204);
}

// ****************************************************************************

void DX3::SetMyPalette()
{
	SetPalette(0,0,128,0);
	SetPalette(1,255,0,0);
	SetPalette(2,255,255,0);
	SetPalette(3,0,0,255);
	SetPalette(4,0,255,0);
	SetPalette(5,128,128,0);
	SetPalette(6,255,128,0);
	SetPalette(7,192,192,192);
	SetPalette(8,128,0,128);
	SetPalette(9,255,255,255);
	SetPalette(10,0,0,0);
	SetPalette(11,255,128,255);
	SetPalette(12,0,255,255);
	SetPalette(13,128,0,0);
	SetPalette(14,128,128,255);
	SetPalette(15,255,255,192);
	SetPalette(16,255,0,128);
	SetPalette(17,64,0,128);
	SetPalette(18,248,128,112);
	SetPalette(19,240,160,96);
	SetPalette(20,0,128,128);
	SetPalette(21,104,104,104);
	SetPalette(22,64,128,128);
	SetPalette(23,220,160,220);
	SetPalette(24,84,108,48);
	SetPalette(25,255,224,224);
	SetPalette(26,128,255,144);
	SetPalette(27,255,216,0);
}

// ****************************************************************************

void DX3::SetPalette(int i, int R, int G, int B)
{
	Palette[i].Red = R;
	Palette[i].Green = G;
	Palette[i].Blue = B;
}

// ****************************************************************************

void DX3::StartFrame()
{
	if (pDevice != NULL)
		pDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 0.0f, 0);
}

// ****************************************************************************

void DX3::Update()
{
}

// ----------------------------------------------------------------------------

DX3::~DX3()
{
	int i;

	if (Buffer != NULL)
	{
		delete Buffer;
		Buffer = NULL;
	}
	if (Palette != NULL)
	{
		delete Palette;
		Palette = NULL;
	}

	for (i = 0; i < Images; i++)
	{
		delete Image[i].Buffer;
		delete Image[i].RawData;
	}
	delete[] Image;
	Image = NULL;
}