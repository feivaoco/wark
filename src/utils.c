#ifndef WARK_UTILS
#define WARK_UTILS

#include "stdio.h"
#include <sys/stat.h>
#include "stdlib.h"
#include "time.h"	
#include <raylib.h>
#include <raymath.h>



int WIDTH  = 840;
int HEIGHT = 720;
int MID_WIDTH = 840/2;
int MID_HEIGHT = 720/2;


float float_rand (float min, float max){ return min + (rand() / (float) RAND_MAX) * ( max - min );}
float float_sign(float x){if (x>0) return +1.0f; else if (x < 0) return -1.0f; else return 0.0f;}
float float_move_toward(float in, float to, float delta){ if (fabsf(to - in) <= delta) return to; else return in + float_sign(to - in) * delta;}

int int_sign(int x){if (x>0) return +1; else if (x < 0) return -1; else return 0;}
int int_move_toward(int in, int to, float delta){ if (abs(to - in) <= delta) return to; else return in + int_sign(to - in) * delta;}



#endif