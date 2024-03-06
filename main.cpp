// main.cpp
//
// Written by John Sowell
//
// This is the main program for the multithreaded BJC_MATCH_GAMES project.
//
// This project displays in DirectX, a match-game record score sheet for 12 fantasy Bowling Journal Competitors. 
// This application is also a multi-threaded application, progressively showing the competitors' match play records
// as the data is being loaded. Another feature of this application is that it not only is a DirectX application,
// but also a console application, printing out some information.
//
#include "Canvas.h"

Canvas Game; // An instance of the Canvas class that includes all the DirectX functionality

// ****************************************************************************

void DrawThread() // This is the draw thread, which performs the draw functionality while the data is being loaded.
{
	while (Game.Mode == LOAD_MODE)
	{
		Game.GameLoop(); // the draw functionality occurs in the Canvas class in the GameLoop
		cout << "\tIn Draw Thread" << endl;
	}
}

// ****************************************************************************

void GetVars(char* Rec,int& pid1, int& pid2, int& sc1, int& sc2, int& mp1, int& mp2)
{
	// A record of data is read in at a time in a pipe-delimited format. The values for the corresponding variables are parsed here
	char buf[10], ch;
	int Dum, i, ii;

	i = 0;
	ii = 0;
	ch = Rec[i];
	while (ch != '|')
	{
		buf[ii++] = ch;
		i++;
		ch = Rec[i];
	}
	buf[ii] = 0;
	Dum = atoi(buf); // we don't use the first variable

	i++;
	ii = 0;
	ch = Rec[i];
	while (ch != '|')
	{
		buf[ii++] = ch;
		i++;
		ch = Rec[i];
	}
	buf[ii] = 0;
	pid1 = atoi(buf);
	pid1--;

	i++;
	ii = 0;
	ch = Rec[i];
	while (ch != '|')
	{
		buf[ii++] = ch;
		i++;
		ch = Rec[i];
	}
	buf[ii] = 0;
	pid2 = atoi(buf);
	pid2--;

	i++;
	ii = 0;
	ch = Rec[i];
	while (ch != '|')
	{
		buf[ii++] = ch;
		i++;
		ch = Rec[i];
	}
	buf[ii] = 0;
	sc1 = atoi(buf);

	i++;
	ii = 0;
	ch = Rec[i];
	while (ch != '|')
	{
		buf[ii++] = ch;
		i++;
		ch = Rec[i];
	}
	buf[ii] = 0;
	sc2 = atoi(buf);

	i++;
	ii = 0;
	ch = Rec[i];
	while (ch != '|')
	{
		buf[ii++] = ch;
		i++;
		ch = Rec[i];
	}
	buf[ii] = 0;
	mp1 = atoi(buf);

	i++;
	ii = 0;
	ch = Rec[i];
	while (ch != '|')
	{
		buf[ii++] = ch;
		i++;
		ch = Rec[i];
	}
	buf[ii] = 0;
	mp2 = atoi(buf);
}

// ****************************************************************************

void LoadThread() // the match game data for the Bowling Journal Competition is loaded here in this thread.
{
	char buf[100];
	int MGT, MP1, MP2, P1, P2, SC1, SC2, x, y;
	FILE* infile;

	if (Game.Mode != LOAD_MODE)
		return;

	fopen_s(&infile, "bjc_match_games.dat", "r");
	if (infile == NULL)
	{
		cout << "INPUT FILE NOT FOUND!" << endl;
		return;
	}
	while (!feof(infile))
	{
		fgets(buf, 100, infile);
		Game.Records++;
		if (Game.Records > 2) // read past headers
		{
			// data obtained and calculations made in this loop.
			GetVars(buf, P1, P2, SC1, SC2, MP1, MP2);
			Game.VS[P1][P2].Wins += MP1;
			Game.TotalWins[P1] += MP1;
			Game.VS[P1][P2].Losses += MP2;
			Game.TotalLosses[P1] += MP2;

			Game.VS[P2][P1].Wins += MP2;
			Game.TotalWins[P2] += MP2;
			Game.VS[P2][P1].Losses += MP1;
			Game.TotalLosses[P2] += MP1;

			MGT = SC1 + SC2;
			if (SC1 > SC2)
			{
				if (MGT > Game.VS[P1][P2].HighWinningMatch)
				{
					Game.VS[P1][P2].HighWinningMatch = MGT;
					Game.VS[P1][P2].W1 = SC1;
					Game.VS[P1][P2].W2 = SC2;
				}
				if (MGT > Game.VS[P2][P1].HighLosingMatch)
				{
					Game.VS[P2][P1].HighLosingMatch = MGT;
					Game.VS[P2][P1].L1 = SC1;
					Game.VS[P2][P1].L2 = SC2;
				}
			}

			if (SC1 < SC2)
			{
				if (MGT > Game.VS[P1][P2].HighLosingMatch)
				{
					Game.VS[P1][P2].HighLosingMatch = MGT;
					Game.VS[P1][P2].L1 = SC1;
					Game.VS[P1][P2].L2 = SC2;
				}
				if (MGT > Game.VS[P2][P1].HighWinningMatch)
				{
					Game.VS[P2][P1].HighWinningMatch = MGT;
					Game.VS[P2][P1].W1 = SC1;
					Game.VS[P2][P1].W2 = SC2;
				}
			}

			if (SC1 == SC2)
			{
				if (MGT > Game.VS[P1][P2].HighTieMatch)
					Game.VS[P1][P2].HighTieMatch = MGT;
				if (MGT > Game.VS[P2][P1].HighTieMatch)
					Game.VS[P2][P1].HighTieMatch = MGT;
			}

			if (SC1 > Game.VS[P1][P2].HG)
				Game.VS[P1][P2].HG = SC1;
			if (SC2 > Game.VS[P2][P1].HG)
				Game.VS[P2][P1].HG = SC2;
		}

		Sleep(1); // just so you can see the output!
		cout << "In Load Thread" << endl;
	}

	fclose(infile);
	Game.Mode = GAME_MODE;

	// after all the data is loaded and calculated, each competitor's record against the other competitors is printed out
	cout << "\n\nBOWLING JOURNAL COMPETITION MATCH GAME STATISTICS\n" << endl;
	for (x = 0; x < 12; x++)
	{
		cout << "\t" << Game.Name[x] << ":" << endl;
		StrCpy(buf, ToString(Game.TotalWins[x] / 10));
		if (Game.TotalWins[x] % 10 == 5)
			StrCat(buf, "½");
		StrCat(buf, " - ");
		StrCat(buf, ToString(Game.TotalLosses[x] / 10));
		if (Game.TotalLosses[x] % 10 == 5)
			StrCat(buf, "½");
		cout << "\t\tTotal Match Game Record: " << buf << endl;

		for (y = 0; y < 12; y++)
		{
			if (y != x)
			{
				if (Game.VS[x][y].Wins > 0)
					StrCpy(buf, ToString(Game.VS[x][y].Wins / 10));
				else
					StrCpy(buf, "0");
				if (Game.VS[x][y].Wins % 10 == 5)
					StrCat(buf, "½");
				StrCat(buf, " - ");

				if (Game.VS[x][y].Losses > 0)
					StrCat(buf, ToString(Game.VS[x][y].Losses / 10));
				else
					StrCat(buf, "0");
				if (Game.VS[x][y].Losses % 10 == 5)
					StrCat(buf, "½");

				cout << "\t\t\tVS. " << Game.Name[y] << ": " << buf << endl;
				if (Game.VS[x][y].HighWinningMatch > 0)
					cout << "\t\t\tHighest Winning Match Game: " << Game.VS[x][y].HighWinningMatch << " (" << Game.VS[x][y].W1 << " - " << Game.VS[x][y].W2 << ")" << endl;
				if (Game.VS[x][y].HighLosingMatch > 0)
					cout << "\t\t\tHighest Losing Match Game: " << Game.VS[x][y].HighLosingMatch << " (" << Game.VS[x][y].L1 << " - " << Game.VS[x][y].L2 << ")" << endl;
				if (Game.VS[x][y].HighTieMatch > 0)
					cout << "\t\t\tHighest Tie Match Game: " << Game.VS[x][y].HighTieMatch << " (" << Game.VS[x][y].HighTieMatch / 2 << " - " << Game.VS[x][y].HighTieMatch / 2 << ")" << endl;
				cout << "\t\t\tHigh Score vs. This Opponent: " << Game.VS[x][y].HG << endl;

				cout << "\n" << endl;				
			}
		}
	}
}

// ############################################################################

LRESULT CALLBACK MainWinProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	switch (Msg)
	{
	case WM_CREATE:
		cout << "Main Window Created!" << endl;
		break;

	case WM_DESTROY:
		cout << "Main Window Destroyed!" << endl;
		cout << "Application Over!" << endl;
		Game.Mode = DONE_MODE;
		PostQuitMessage(0);
		break;

	default:
		break;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

// &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

int main() // main entry point for the project
{
	HINSTANCE hInst = GetModuleHandle(NULL);
	HWND hWnd;
	MSG msg = { 0 };
	WNDCLASS wc;

	wc = { 0 };
	wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 0));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIconW(hInst, (LPCWSTR)MB_ICONQUESTION);
	wc.hInstance = hInst;
	wc.lpfnWndProc = MainWinProc;
	wc.lpszClassName = L"MainWindowClass";
	RegisterClassW(&wc);
	hWnd = CreateWindowW(L"MainWindowClass", L"Main Window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 1616, 900, NULL, NULL, NULL, NULL);

	cout << "Starting App" << endl;
	Game.Initialize(hWnd, 28);

	thread ThreadDraw(DrawThread); // defines the draw thread
	thread ThreadLoad(LoadThread); // defines the load thread

	// These statements are needed, else the program will crash at the end. These statements join the threads to the program.
	ThreadDraw.join(); 
	ThreadLoad.join();

	while (TRUE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
				break;
		}
		if (Game.Mode == GAME_MODE)
			Game.GameLoop();
	}

	cout << "Goodbye Cruel World!" << endl;
	return msg.wParam;
}