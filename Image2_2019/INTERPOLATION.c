
#include "GEO.h"

int Round(double x) //�ݿø� ���ִ� �Լ�.
{
	x += 0.5;
	x = (int)x;
	return x;
}

int Min(int a, int b) { // �� ������ �� ���� ���� ã���ִ� �Լ�
	if (a > b) return b;
	else return a;
}

int Max(int a, int b) { // �� ������ �� ū ���� ã���ִ� �Լ�
	if (a > b) return a;
	else return b;
}

UChar NearesetNeighbor(UChar* Data, Double srcX, Double srcY, Int Stride) // Near Neighbor Interpolation �Լ�
{//srcX�� srcY�� �� �����ؼ� �ֺ� �� 4���� ���� ����� ȭ�� ���� ã���� ( �ݿø��� ���� ã�� �� ���� )
	return Data[((int)(srcY + 0.5) * Stride + (int)(srcX + 0.5))]; 
}

UChar Bilinear(UChar* Data, Double srcX, Double srcY, Int Stride) //Bilinear Interpolation �Լ�
{ //3���� linear Interpolation�� ���� ȭ�� ���� ã����
	int SrcX_Plus1, SrcY_Plus1;
	double Hor_Wei, Ver_Wei; //Horizontal Weight, Vertical Weight
	int TL, TR, BL, BR; //�� ȭ�� ��ġ

	SrcX_Plus1 = CLIPPIC_HOR((int)srcX +1);// src ȭ���� ���� �� ȭ�Ҵ� src(�Ǽ�)���� ������ ���� ���� �� ����
	SrcY_Plus1 = CLIPPIC_VER((int)srcY + 1);// ���� src�� �̿��ؼ� X��, Y������ +1 �� �� ��ġ�� ã����


	Hor_Wei = srcX - (int)srcX; 
	Ver_Wei = srcY - (int)srcY;	//����ġ �� ���ϱ� (�Ÿ� ���ϱ�)

	TL = (int)srcY * Stride + (int)srcX; //������ ã���� Src_Plus1 ���� ���� �ֺ� 4�� ȭ�� ��ġ���� ã����
	TR = (int)srcY * Stride + SrcX_Plus1;
	BL = SrcY_Plus1 * Stride + (int)srcX;
	BR = SrcY_Plus1 * Stride + SrcX_Plus1;

	UChar TMP =  // ���� ����(x�� ����)���� ���� 2�� linear Interpolation ���ְ�, ��� ����� ���� ���� (y�� ����)���� linear Interpolation
		(1 - Ver_Wei) * (((1 - Hor_Wei) * Data[TL]) + (Hor_Wei * Data[TR])) +

		Ver_Wei * (((1 - Hor_Wei) * Data[BL]) + (Hor_Wei * Data[BR]));
	
	TMP = Min(Max(Round(TMP), 0), 255); // �ݿø�, Ŭ���� ����
	return TMP;
}

double BSpline_function(Double x) { //ȭ�ҿ� weight�� �ִ� Ŀ�� �Լ� ���� (BSpline �������� �ش��ϴ�)
	// �Ű����� x�� ������ �ֺ�ȭ�ҿ� src ȭ�ҿ��� �Ÿ�

	double result;

	if (x < 0) x *= -1; // X�� �������� ��� �ϱ� ���� ����

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

UChar B_Spline(UChar* Data, Double srcX, Double srcY, Int Stride)// B_Spline Interpolation �Լ�
//�ֺ� 16�� ���� 5���� 1���� Interpolation�� ���� ȭ�� ���� ã���� ( 4�� ����������� ����, �� ����� ���� ������������ 1�� ����)
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
	Src_Y_Minus_1 = CLIPPIC_VER((int)srcY - 1); //���� ���� ������� �ֺ� ȭ�� ��ġ ã�� ���� ����

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

	TMP = Min(Max(Round(TMP), 0), 255); // Ŭ���ΰ� �ݿø�

	return (UChar)TMP;
}



double Cubic_function(Double x){ //ȭ�ҿ� weight�� �ִ� Ŀ�� �Լ� ���� (Cubic �������� �ش��ϴ�)
// �Ű����� x�� ������ �ֺ�ȭ�ҿ� src ȭ�ҿ��� �Ÿ�

	double result;
	double a;
	a = 0.5; // Ŀ�� �Ķ���� b,c�� b=0,c=1/2 �� �Ǵ� ���� a=1/2 �Ǵ� ���� 

	if (x < 0) x *= -1;  // X�� �������� ��� �ϱ� ���� ����

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

UChar Cubic(UChar* Data, Double srcX, Double srcY, Int Stride)// Cubic Interpolation �Լ�
//�ֺ� 16�� ���� 5���� 1���� Interpolation�� ���� ȭ�� ���� ã���� ( 4�� ����������� ����, �� ����� ���� ������������ 1�� ����)
//B-spline ���� �Լ��� ����
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

	TMP = Min(Max(Round(TMP), 0), 255); // Ŭ���ΰ� �ݿø� �ʿ�

	return (UChar)TMP;
}