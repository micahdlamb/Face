#include <GL/freeglut.h>
#include <iostream>
using namespace std;
#include "clock.h"

int Clock::timeMilli;
float Clock::time;
int Clock::deltaMilli;
float Clock::delta;
float Clock::maxDelta = 1.f/30;

float Clock::start=0;
int Clock::counter=0;
