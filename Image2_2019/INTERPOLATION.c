
#include "GEO.h"

int Round(double x) //반올림 해주는 함수.
{
	x += 0.5;
	x = (int)x;
	return x;
}

int Min(int a, int b) { // 두 인자중 더 작은 값을 찾아주는 함수
	if (a > b) return b;
	else return a;
}

int Max(int a, int b) { // 두 인자중 더 큰 값을 찾아주는 함수
	if (a > b) return a;
	else return b;
}

UChar NearesetNeighbor(UChar* Data, Double srcX, Double srcY, Int Stride) // Near Neighbor Interpolation 함수
{//srcX와 srcY을 역 추적해서 주변 점 4개중 가장 가까운 화소 값을 찾아줌 ( 반올림을 통해 찾을 수 있음 )
	return Data[((int)(srcY + 0.5) * Stride + (int)(srcX + 0.5))]; 
}

UChar Bilinear(UChar* Data, Double srcX, Double srcY, Int Stride) //Bilinear Interpolation 함수
{ //3번의 linear Interpolation을 통해 화소 값을 찾아줌
	int SrcX_Plus1, SrcY_Plus1;
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	int TL, TR, BL, BR; //각 화소 위치

	SrcX_Plus1 = CLIPPIC_HOR((int)srcX +1);// src 화소의 왼쪽 위 화소는 src(실수)에서 버림을 통해 얻을 수 있음
	SrcY_Plus1 = CLIPPIC_VER((int)srcY + 1);// 따라서 src를 이용해서 X축, Y축으로 +1 된 점 위치를 찾아줌


	Hor_Wei = srcX - (int)srcX; 
	Ver_Wei = srcY - (int)srcY;	//가중치 값 구하기 (거리 구하기)

	TL = (int)srcY * Stride + (int)srcX; //위에서 찾아준 Src_Plus1 값을 통해 주변 4개 화소 위치들을 찾아줌
	TR = (int)srcY * Stride + SrcX_Plus1;
	BL = SrcY_Plus1 * Stride + (int)srcX;
	BR = SrcY_Plus1 * Stride + SrcX_Plus1;

	UChar TMP =  // 수평 방향(x축 방향)으로 먼저 2번 linear Interpolation 해주고, 결과 값들로 수직 방향 (y축 방향)으로 linear Interpolation
		(1 - Ver_Wei) * (((1 - Hor_Wei) * Data[TL]) + (Hor_Wei * Data[TR])) +

		Ver_Wei * (((1 - Hor_Wei) * Data[BL]) + (Hor_Wei * Data[BR]));
	
	TMP = Min(Max(Round(TMP), 0), 255); // 반올림, 클리핑 과정
	return TMP;
}

double BSpline_function(Double x) { //화소에 weight를 주는 커널 함수 정의 (BSpline 보간법에 해당하는)
	// 매개변수 x는 각각의 주변화소와 src 화소와의 거리

	double result;

	if (x < 0) x *= -1; // X의 절댓값으로 계산 하기 위한 과정

	if (fabs(x) >= 0 && fabs(x) < 1) {
		result = (0.5) * x * x * x - x * x + (0.66666);
	}
	else if (fabs(x) >= 1 && fabs(x) < 2) {
		result = (-0.166666) * x * x * x + x * x - 2 * x + (1.3333);
	}
	else
		result = 0;

	return result;
}

UChar B_Spline(UChar* Data, Double srcX, Double srcY, Int Stride)// B_Spline Interpolation 함수
//주변 16개 점을 5번의 1차원 Interpolation을 통해 화소 값을 찾아줌 ( 4번 수평방향으로 보간, 그 결과를 갖고 수직방향으로 1번 보간)
{
	int Src_X_Minus_1, Src_X_Plus_1, Src_X_Plus_2;
	int Src_Y_Minus_1, Src_Y_Plus_1, Src_Y_Plus_2;
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	double TMP_Hor[4] = { 0,0,0,0 };
	double TMP = 0;

	Src_X_Plus_1 = CLIPPIC_HOR((int)srcX + 1);
	Src_X_Plus_2 = CLIPPIC_HOR((int)srcX + 2);
	Src_Y_Plus_1 = CLIPPIC_VER((int)srcY + 1);
	Src_Y_Plus_2 = CLIPPIC_VER((int)srcY + 2);

	Src_X_Minus_1 = CLIPPIC_HOR((int)srcX - 1);
	Src_Y_Minus_1 = CLIPPIC_VER((int)srcY - 1); //위와 같은 방법으로 주변 화소 위치 찾기 위한 과정

	Hor_Wei = srcX - (int)srcX;
	Ver_Wei = srcY - (int)srcY;

	int X_Pix[] = { Src_X_Minus_1, (int)srcX, Src_X_Plus_1, Src_X_Plus_2 };
	int Y_Pix[] = { Src_Y_Minus_1, (int)srcY, Src_Y_Plus_1, Src_Y_Plus_2 };
	double Distance_Hor[] = { Hor_Wei + 1, Hor_Wei, 1 - Hor_Wei, (1 - Hor_Wei) + 1 };
	double Distance_Ver[] = { Ver_Wei + 1, Ver_Wei, 1 - Ver_Wei, (1 - Ver_Wei) + 1 };

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			TMP_Hor[i] += BSpline_function(Distance_Hor[j]) * Data[Y_Pix[i] * Stride + X_Pix[j]]; 
		}
	}

	for (int i = 0; i < 4; i++)
	{
		TMP += BSpline_function(Distance_Ver[i]) * TMP_Hor[i];
	}

	TMP = Min(Max(Round(TMP), 0), 255); // 클리핑과 반올림

	return (UChar)TMP;
}



double Cubic_function(Double x){ //화소에 weight를 주는 커널 함수 정의 (Cubic 보간법에 해당하는)
// 매개변수 x는 각각의 주변화소와 src 화소와의 거리

	double result;
	double a;
	a = 0.5; // 커널 파라미터 b,c가 b=0,c=1/2 가 되는 것이 a=1/2 되는 것임 

	if (x < 0) x *= -1;  // X의 절댓값으로 계산 하기 위한 과정

	if (fabs(x) >= 0 && fabs(x) < 1) {
		result = (a + 2) * x * x * x - (a + 3) * x * x + 1;
	}
	else if (fabs(x) >= 1 && fabs(x) < 2) {
		result = a * x * x * x - 5 * a * x * x + 8 * a * x - 4 * a;
	}
	else
		result = 0;

	return result;
}

UChar Cubic(UChar* Data, Double srcX, Double srcY, Int Stride)// Cubic Interpolation 함수
//주변 16개 점을 5번의 1차원 Interpolation을 통해 화소 값을 찾아줌 ( 4번 수평방향으로 보간, 그 결과를 갖고 수직방향으로 1번 보간)
//B-spline 보간 함수와 동일
{
	int Src_X_Minus_1, Src_X_Plus_1, Src_X_Plus_2;
	int Src_Y_Minus_1, Src_Y_Plus_1, Src_Y_Plus_2;
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	double TMP_Hor[4] = { 0,0,0,0 };
	double TMP = 0;

	Src_X_Plus_1 = CLIPPIC_HOR((int)srcX + 1);
	Src_X_Plus_2 = CLIPPIC_HOR((int)srcX + 2);
	Src_Y_Plus_1 = CLIPPIC_VER((int)srcY + 1);
	Src_Y_Plus_2 = CLIPPIC_VER((int)srcY + 2);

	Src_X_Minus_1 = CLIPPIC_HOR((int)srcX - 1);
	Src_Y_Minus_1 = CLIPPIC_VER((int)srcY - 1);

	Hor_Wei = srcX - (int)srcX;
	Ver_Wei = srcY - (int)srcY;

	int X_Pix[] = { Src_X_Minus_1, (int)srcX, Src_X_Plus_1, Src_X_Plus_2 };
	int Y_Pix[] = { Src_Y_Minus_1, (int)srcY, Src_Y_Plus_1, Src_Y_Plus_2 };
	double Distance_Hor[] = { Hor_Wei + 1, Hor_Wei, 1 - Hor_Wei, (1 - Hor_Wei) + 1 };
	double Distance_Ver[] = { Ver_Wei + 1, Ver_Wei, 1 - Ver_Wei, (1 - Ver_Wei) + 1 };

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			TMP_Hor[i] += Cubic_function(Distance_Hor[j]) * Data[Y_Pix[i] * Stride + X_Pix[j]];
		}
	}

	for (int i = 0; i < 4; i++)
	{
		TMP += Cubic_function(Distance_Ver[i]) * TMP_Hor[i];
	}

	TMP = Min(Max(Round(TMP), 0), 255); // 클리핑과 반올림 필요

	return (UChar)TMP;
}