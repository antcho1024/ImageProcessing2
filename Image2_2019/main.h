#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>
#define _CRT_SECURE_NO_WARNINGS


#define WIDTH      512              // 영상의 가로 크기
#define HEIGHT     512              // 영상의 세로 크기

//#define scaleConstant  0.47         // 스케일링 상수 (축소)
#define scaleConstant  2.32         // 스케일링 상수 (확대)

#define maxVal    255
#define minVal    0

typedef unsigned char UChar;
typedef char          Char;
typedef double		  Double;
typedef int           Int;

