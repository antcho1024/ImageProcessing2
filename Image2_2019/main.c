#include "GEO.h"

void main()
{
	FILE *fp;
	UChar *ori; //���� ���� ȭ�Ұ����� �����ϱ� ���� ����
	Img_Buf image; //���� �뵵


	Int wid = WIDTH; Int hei = HEIGHT;
	Int min = minVal; Int max = maxVal;

	fopen_s(&fp, "lena_512x512.raw", "rb"); //���� ���� ����

	ori = (UChar*)malloc(sizeof(UChar) * (wid * hei)); //���� ���� ũ�⸸ŭ ���� ����
	memset(ori, 0, sizeof(UChar) * (wid * hei)); //0���� �ʱ�ȭ
	fread(ori, sizeof(UChar), (wid * hei), fp); // ���� ���� �б�(���� ������ �ȼ� ���� �迭 ������ ����)

	Geometric_Transformation(ori, &image); //�Լ� ȣ��

	free(ori);
	fclose(fp);
}