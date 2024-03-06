//
// Strings3.cpp
// 
// Written by John Sowell
// 
// Written 1/13/2024
// 
// Implements the new string functions that eliminate memory leaks.
// 
#include "Strings3.h"

// ############################################################################

char* FixedStr(char* Str, int Len, int StartChar)
{
	static char NewStr[20000];
	int i;

	if ((StartChar < 0) || (StartChar >= StrLen(Str)) || (Len < 1))
		NewStr[0] = 0; // return a null string
	else
	{
		if (Len > StrLen(Str))
			Len = StrLen(Str);
		if (Len + StartChar > StrLen(Str))
			Len = StrLen(Str) - StartChar;
		for (i = 0; i < Len; i++)
			NewStr[i] = Str[StartChar + i];
		NewStr[Len] = 0;
	}
	return NewStr;
}

// ****************************************************************************

void StrCat(char* Str, const  char* V)
{
	char ch;
	int i,ii;

	i = StrLen(Str);
	ii = 0;
	ch = V[ii];
	while (ch != 0)
	{
		Str[i++] = ch;
		ch = V[++ii];
	}
	Str[i] = 0;
}

// ############################################################################

bool StrCmp(const char* S1, const char* S2)
{
	int x;

	if (StrLen(S1) != StrLen(S2))
		return false;

	for (x = 0; x < StrLen(S1); x++)
	{
		if (S1[x] != S2[x])
			return false;
	}

	return true;
}

// ****************************************************************************

void StrCpy(char* Str, const  char* V)
{
	char ch;
	int i;

	i = 0;
	ch = V[i];
	while (ch != 0)
	{
		Str[i++] = ch;
		ch = V[i];
	}
	Str[i] = 0;
}

// ############################################################################

int StrLen(const char* Str)
{
	int i, L;
	char ch;

	i = 0;
	L = 0;
	ch = Str[i];
	while (ch != 0)
	{
		L++;
		ch = Str[++i];
	}
	return L;
}

// ****************************************************************************

void StrnCat(char* Str, const char* Add, int N)
{
	char ch;
	int L, x, y;

	L = StrLen(Add);
	if (N > L)
		N = L;
	if (N < 1)
		return;

	x = StrLen(Str);
	for (y = 0; y < N; y++)
	{
		ch = Add[y];
		Str[x++] = ch;
		ch = Add[y];
	}
	Str[x] = 0;
}

// ****************************************************************************

void StrnCpy(char* Str, const char* Val, int N)
{
	int x;

	if (N > StrLen(Str))
		N = StrLen(Str);

	for (x = 0; x < N; x++)
	{
		Str[x] = *Val;
		Val++;
	}
	Str[x] = 0;
}

// ############################################################################

char* ToLower(char* Str)
{
	char ch;
	int i;

	i = 0;
	ch = Str[0];
	while (ch != 0)
	{
		if ((ch > 64) && (ch < 91))
			ch += 32;
		Str[i++] = ch;
		ch = Str[i];
	}

	return Str;
}

// ############################################################################

char* ToString(double N, int Places)
{
	char RevStr[30];
	double Fraction;
	static char Str[30];
	int d, Decimal, i, ii, WholeNumber;

	if (Places > 20)
		Places = 20;
	if (Places < 0)
		Places = 2;

	i = 0;
	if (N < 0)
	{
		Str[i++] = '-';
		N *= (double)-1;
	}

	WholeNumber = (int)N;
	Fraction = N - (double)WholeNumber;
	for (ii = 0; ii < Places; ii++)
		Fraction *= (double)10;
	Decimal = int(Fraction);

	ii = 0;
	while (Decimal > 0)
	{
		d = Decimal % 10;
		RevStr[ii++] = 48 + d;
		Decimal /= 10;
	}
	RevStr[ii++] = 46;
	while (WholeNumber > 0)
	{
		d = WholeNumber % 10;
		RevStr[ii++] = 48 + d;
		WholeNumber /= 10;
	}
	RevStr[ii] = 0;
	ii--;
	while (ii >= 0)
	{
		Str[i++] = RevStr[ii--];
	}
	Str[i] = 0;
	return Str;
}

// ############################################################################

char* ToString(int N)
{
	char RevStr[11];
	static char Str[15];
	int d,i,ii;

	i = 0;
	if (N < 0)
	{
		Str[i++] = '-';
		N *= -1;
	}

	ii = 0;
	while (N > 0)
	{
		d = N % 10;
		RevStr[ii++] = 48 + d;
		N /= 10;
	}
	RevStr[ii] = 0;

	ii--; // pointing to the last char in RevStr
	while (ii >= 0)
	{
		Str[i++] = RevStr[ii--];
	}
	Str[i] = 0;
	return Str;
}

// ############################################################################

char* ToUpper(char* Str)
{
	char ch;
	int i;

	i = 0;
	ch = Str[0];
	while (ch != 0)
	{
		if ((ch > 96) && (ch < 123))
			ch -= 32;
		Str[i++] = ch;
		ch = Str[i];
	}

	return Str;
}