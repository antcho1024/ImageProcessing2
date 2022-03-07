#include "main.h"

#define CLIPPIC_HOR(x) (x < 0) ? 0 : x >= WIDTH  ? WIDTH  - 1 : x
#define CLIPPIC_VER(x) (x < 0) ? 0 : x >= HEIGHT ? HEIGHT - 1 : x

#define PI			  3.141592653589793238462643383279

typedef struct _Scale_Buffer
{
	UChar* Near;
	UChar* Bi;
	UChar* BS;
	UChar* Cu;

}SCALE;

typedef struct _Rotation_Buffer
{
	UChar* Near;
	UChar* Bi;
	UChar* BS;
	UChar* Cu;

}ROTATION;

typedef struct _Image_Buffer
{

	UChar* padding; // 패딩 영상 저장 버퍼
	UChar* Result_Blurring;         // 축소 영상을 위한 블러링 결과 저장 

}Img_Buf;

void Geometric_Transformation(UChar* Data, Img_Buf* img);


void ImageOutput(UChar* Data, Int wid, Int hei, Char String[]);

UChar NearesetNeighbor(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar Bilinear(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar B_Spline(UChar* Data, Double srcX, Double srcY, Int Stride);
UChar Cubic(UChar* Data, Double srcX, Double srcY, Int Stride);