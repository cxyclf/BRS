#ifndef TEST_H7
#define TEST_H7
#include <iostream>
#include <ctime>
#include <bits/stdc++.h>
#include <string>
#include <cstdio>
#include <ctime>
#include <iomanip> 
using namespace std;

void printDatetime(time_t rawtime);

std::string getCurrentTimestamp(time_t x);

std::string gethms(time_t x);

void convertTime(time_t total_seconds);
#endif
