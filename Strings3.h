//
// Strings3.h
//
// Written by John Sowell
//
// Written 1/13/2024
//
// Revising the string functions eliminating memory leaks.
//
#pragma once

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

void StrCpy(char* Str, const char* V);
int StrLen(const char* Str);
char* ToUpper(char* Str);
char* ToLower(char* Str);
void StrCat(char* Str, const char* V);
char* ToString(int N);
char* ToString(double N, int Places);
char* FixedStr(char* Str, int Len, int StartChar);
bool StrCmp(const char* S1, const char* S2);
void StrnCat(char* Str, const char* Add, int N);
void StrnCpy(char* Str, const char* Val, int N);