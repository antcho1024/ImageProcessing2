#include "GEO.h"

void Scaling(UChar* Data, Int dstWid, Int dstHei, Double scaleVal) // 스케일링 함수 ( 확대/축소 )
{//원본 데이터와 계산된 output 영상의 크기와 몇 배로 스케일링할 지에대한 변수를 매개변수로 받음 
	SCALE scale;
	double srcX, srcY; //역 추적한 원본 화소 위치 (정수 위치가 아닐 수 있음)

	Char String[4][10] = { "Near_L", "Bi_L" , "BS_L" ,"Cu_L" }; //파일 명으로 출력될 문자열들. 축소시엔 ("Near") 확대시엔 ("Near_L")

	scale.Near = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));// 4가지 보간 법으로 각각 축소/확대를 진행 함
	scale.Bi = (UChar*)calloc(dstWid * dstHei, sizeof(UChar)); // 각각 output 영상을 담을 공간 할당
	scale.BS = (UChar*)calloc(dstWid * dstHei, sizeof(UChar));
	scale.Cu = (UChar*)calloc(dstWid * dstHei, sizeof(UChar)); 

	for (int i = 0; i < dstHei; i++)
	{
		for (int j = 0; j < dstWid; j++)
		{
			srcX = (double)j / scaleVal;
			srcY = (double)i / scaleVal;

			scale.Near[i * dstWid + j] = NearesetNeighbor(Data, srcX, srcY, WIDTH);
			scale.Bi[i * dstWid + j] = Bilinear(Data, srcX, srcY, WIDTH);
			scale.BS[i * dstWid + j] = B_Spline(Data, srcX, srcY, WIDTH);
			scale.Cu[i * dstWid + j] = Cubic(Data, srcX, srcY, WIDTH); // 각 보간법에 대한 함수를 호출
		}
	}

	ImageOutput(scale.Near, dstWid, dstHei, String[0]);
	ImageOutput(scale.Bi, dstWid, dstHei, String[1]);
	ImageOutput(scale.BS, dstWid, dstHei, String[2]);
	ImageOutput(scale.Cu, dstWid, dstHei, String[3]); // 이미지 출력

	free(scale.Near);
	free(scale.Bi);
	free(scale.BS);
	free(scale.Cu); // 공간 해제
}

void Rotation(UChar* Data) // 회전 함수
{// 원본영상을 매개변수로 받음
	ROTATION rot;
	FILE* up1, * up2, * up3, * up4;

	double Angle;
	double srcX, srcY;	// Source 위치

	int New_X, New_Y;
	int Center_X = WIDTH / 2, Center_Y = HEIGHT / 2;


	//회전 중심 : 원점일 경우

	fopen_s(&up1, "Ro_Near.raw", "wb");
	fopen_s(&up2, "Ro_Bi.raw", "wb");
	fopen_s(&up3, "Ro_BS.raw", "wb");
	fopen_s(&up4, "Ro_Cu.raw", "wb");

	rot.Near = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar)); //output 영상을 위한 공간 할당
	rot.Bi = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.BS = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.Cu = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));

	for (Angle = 0; Angle <= 360; Angle += 4) //4도씩 돌려 한바퀴(360도) 돌림
	{
		double Seta = PI / 180.0 * Angle; // 세타 구해줌

		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				srcX = cos(Seta) * j + sin(Seta) * i; //inverse 추적.
				srcY = -sin(Seta) * j + cos(Seta) * i;

				New_X = (int)srcX;
				New_Y = (int)srcY;//원시 화소가 영상 경계 밖에 있는지를 확인하기 위한 변수

				if (!(New_X < 0 || New_X >= WIDTH - 1 || New_Y < 0 || New_Y >= HEIGHT - 1)) // 원시 화소가 영상 경계 밖에 위치
				{
					rot.Near[i * WIDTH + j] = NearesetNeighbor(Data, srcX, srcY, WIDTH);
					rot.Bi[i * WIDTH + j] = Bilinear(Data, srcX, srcY, WIDTH);
					rot.BS[i * WIDTH + j] = B_Spline(Data, srcX, srcY, WIDTH);
					rot.Cu[i * WIDTH + j] = Cubic(Data, srcX, srcY, WIDTH);//원시 화소가 영상 경계 밖에 위치하지 않을 경우 보간 함수를 호출
				}
				else
				{
					rot.Near[i * WIDTH + j] = 0;
					rot.Bi[i * WIDTH + j] = 0;
					rot.BS[i * WIDTH + j] = 0;
					rot.Cu[i * WIDTH + j] = 0;//원시 화소가 영상 경계 밖에 위치할 경우 화소값 0
				}
			}
		}

		fwrite(rot.Near, sizeof(UChar), (WIDTH * HEIGHT), up1);
		fwrite(rot.Bi, sizeof(UChar), (WIDTH * HEIGHT), up2);
		fwrite(rot.BS, sizeof(UChar), (WIDTH * HEIGHT), up3);
		fwrite(rot.Cu, sizeof(UChar), (WIDTH * HEIGHT), up4);
	}

	free(rot.Near);
	free(rot.Bi);
	free(rot.BS);
	free(rot.Cu);

	fclose(up1);
	fclose(up2);
	fclose(up3);
	fclose(up4);


	//회전 중심 :  C ( WIDTH / 2,  HEIGHT / 2 ) 일 경우;

	fopen_s(&up1, "Ro_Near_center.raw", "wb");
	fopen_s(&up2, "Ro_Bi_center.raw", "wb");
	fopen_s(&up3, "Ro_BS_center.raw", "wb");
	fopen_s(&up4, "Ro_Cu_center.raw", "wb"); // 파일이름을 바궈서 다시 오픈

	rot.Near = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.Bi = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.BS = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));
	rot.Cu = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar)); // 다시 메모리 할당

	for (Angle = 0; Angle <= 360; Angle += 4)
	{// 원점일 때와 거의 동일 하나 srcX, srcY 을 구하는 식만 달라짐
		double Seta = PI / 180.0 * Angle;

		for (int i = 0; i < HEIGHT; i++)
		{
			for (int j = 0; j < WIDTH; j++)
			{
				srcX = Center_X - Center_X * cos(Seta) + cos(Seta) * j - Center_Y * sin(Seta) + i * sin(Seta);
				srcY = Center_Y - Center_Y * cos(Seta) + i * cos(Seta) + Center_X * sin(Seta) - j * sin(Seta);

				New_X = (int)srcX;
				New_Y = (int)srcY;

				if (!(New_X < 0 || New_X >= WIDTH - 1 || New_Y < 0 || New_Y >= HEIGHT - 1)) // 원시 화소가 영상 경계 밖에 위치
				{
					rot.Near[i * WIDTH + j] = NearesetNeighbor(Data, srcX, srcY, WIDTH);
					rot.Bi[i * WIDTH + j] = Bilinear(Data, srcX, srcY, WIDTH);
					rot.BS[i * WIDTH + j] = B_Spline(Data, srcX, srcY, WIDTH);
					rot.Cu[i * WIDTH + j] = Cubic(Data, srcX, srcY, WIDTH);
				}
				else
				{
					rot.Near[i * WIDTH + j] = 0;
					rot.Bi[i * WIDTH + j] = 0;
					rot.BS[i * WIDTH + j] = 0;
					rot.Cu[i * WIDTH + j] = 0;
				}
			}
		}

		fwrite(rot.Near, sizeof(UChar), (WIDTH * HEIGHT), up1);
		fwrite(rot.Bi, sizeof(UChar), (WIDTH * HEIGHT), up2);
		fwrite(rot.BS, sizeof(UChar), (WIDTH * HEIGHT), up3);
		fwrite(rot.Cu, sizeof(UChar), (WIDTH * HEIGHT), up4);
	}

	free(rot.Near);
	free(rot.Bi);
	free(rot.BS);
	free(rot.Cu);


	fclose(up1);
	fclose(up2);
	fclose(up3);
	fclose(up4);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Image_Padding(Img_Buf* img, UChar* Buf, int width, int height, int Mask_size) //output 영상의 과도 축소를 막기 위한 패딩 함수
{
	int line, i, j;

	img->padding = (UChar*)calloc((width + Mask_size - 1) * (height + Mask_size - 1), sizeof(UChar));
	for (line = 0; line < (Mask_size / 2); line++)
	{
		//상하단 패딩
		for (i = 0; i < width; i++)
		{
			img->padding[(width + Mask_size - 1) * line + Mask_size / 2 + i] = Buf[i];
			img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - line) + Mask_size / 2 + i] = Buf[i + (width * (height - 1))];
		}

		//좌우측 패딩
		for (i = 0; i < height; i++)
		{
			img->padding[(width + Mask_size - 1) * (Mask_size / 2 + i) + line] = Buf[i * width];
			img->padding[(width + Mask_size - 1) * (Mask_size / 2 + 1 + i) - 1 - line] = Buf[i * width + (width - 1)];
		}
	}

	for (line = 0; line < 4; line++)
	{
		for (i = 0; i < (Mask_size / 2); i++)
		{
			for (j = 0; j < (Mask_size / 2); j++)
			{
				/*** 좌상단 패딩 ***/
				if (line == 0)
				{
					img->padding[(width + Mask_size - 1) * i + j] = Buf[0];
				}
				/*** 우상단 패딩 ***/
				else if (line == 1)
				{
					img->padding[(width + Mask_size - 1) * i + Mask_size / 2 + width + j] = Buf[width - 1];
				}
				/*** 좌하단 패딩 ***/
				else if (line == 2)
				{
					img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - i) + j] = Buf[width * (height - 1)];
				}
				/*** 우하단 패딩 ***/
				else
				{
					img->padding[(width + Mask_size - 1) * (height + Mask_size - 2 - i) + Mask_size / 2 + width + j] = Buf[width * height - 1];
				}
			}
		}
	}

	/*** 원본 버퍼 불러오기 ***/
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			img->padding[(width + Mask_size - 1) * (Mask_size / 2 + i) + Mask_size / 2 + j] = Buf[i * width + j];
		}
	}
}

UChar Blurring(UChar* buf, int Mask_size) // aliasing 을 피하기 위한 블러링 함수
{
	double Mask_Coeff[] = { 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0 };
	double Convolution_All_coeff = 0;

	for (int i = 0; i < Mask_size * Mask_size; i++)
		Convolution_All_coeff += (Mask_Coeff[i] * (double)buf[i]);

	return Convolution_All_coeff = Convolution_All_coeff > maxVal ? maxVal : Convolution_All_coeff < minVal ? minVal : Convolution_All_coeff;
}

void Image_Filtering(UChar* Data, Img_Buf* img)
{
	int Mask_size = 3;         //MxM size
	int Add_size = Mask_size / 2 + 1;
	UChar Padding_buf[9] = { 0 };

	Image_Padding(img, Data, WIDTH, HEIGHT, 3);

	img->Result_Blurring = (UChar*)calloc(WIDTH * HEIGHT, sizeof(UChar));

	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			for (int k = 0; k < Mask_size; k++)
				for (int l = 0; l < Mask_size; l++)
					Padding_buf[k * Mask_size + l] = img->padding[(i + k) * (WIDTH + Add_size) + (j + l)];

			img->Result_Blurring[i * WIDTH + j] = Blurring(&Padding_buf, Mask_size);
		}
	}
	free(img->padding);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void Geometric_Transformation(UChar* Data, Img_Buf* img) // main 함수에서 이미지 변형을 위해 호출한 함수
{
	
	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	Double scaleVal = scaleConstant; // 스케일링 크기 변수 ( 0.47 / 2.32 )

	Int dstWid; //스케일링 적용된 영상의 가로 길이
	Int dstHei; ////스케일링 적용된 영상의 세로 길이

	dstWid = wid * scaleVal + 0.5;
	dstHei = hei * scaleVal + 0.5; // 스케일링 될 영상의 크기 ( 반올림 포함 )

	Rotation(Data); //회전 함수 호출

	if (scaleVal < 1) // 축소시 원본 영상 블러링 적용
	{
		Image_Filtering(Data, img);
		memcpy(Data, img->Result_Blurring, sizeof(UChar) * wid * hei);
		free(img->Result_Blurring);
	}

	Scaling(Data, dstWid, dstHei, scaleVal); //스케일링 함수 호출

	

}