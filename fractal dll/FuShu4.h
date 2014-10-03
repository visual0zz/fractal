#include <windows.h>
#include "math.h"

#define PI 3.14159265358979//Բ����
#define E 2.71828182845904//��Ȼ����

#define LN(a) (log(a)/log(E))

//������
class FuShu4  
{
public:
	long double w;//w+xi+yj+zk
	long double x;
	long double y;
	long double z;
public:
	friend FuShu4 operator+(long double m,FuShu4 f);
	friend FuShu4 operator-(long double m,FuShu4 f);
	friend FuShu4 operator*(long double m,FuShu4 f);
	friend FuShu4 operator/(long double m,FuShu4 f);

	FuShu4 operator++();//ǰ׺����
	FuShu4 operator++(int);//��׺����

	FuShu4 operator=(long double m);
	FuShu4 operator*(long double m);
	FuShu4 operator-(long double m);
	FuShu4 operator+(long double m);
	FuShu4 operator/(long double m);

	//FuShu4 operator=(FuShu4 f);
	FuShu4 operator*(FuShu4 f);
	FuShu4 operator-(FuShu4 f);
	FuShu4 operator+(FuShu4 f);
	FuShu4 operator/(FuShu4 f);
	
public:
	FuShu4();
	FuShu4(long double m);
	FuShu4(long double m,long double a,long double b,long double c);
	virtual ~FuShu4();

public:
	long double m();//����ģ
	long double m2();//����ģ��ƽ��
	void Get(long double m,long double a,long double b,long double c);
	void Get(long double m);
	void Get(FuShu4 f);

};

FuShu4 cos(FuShu4 f);
FuShu4 sin(FuShu4 f);
FuShu4 tan(FuShu4 f);
FuShu4 cot(FuShu4 f);
FuShu4 cosh(FuShu4 f);
FuShu4 sinh(FuShu4 f);
FuShu4 tanh(FuShu4 f);
FuShu4 coth(FuShu4 f);
FuShu4 exp(FuShu4 f);
FuShu4 ln(FuShu4 f);
FuShu4 log(FuShu4 f);//��ʮΪ��

FuShu4 csc(FuShu4 f);
FuShu4 sec(FuShu4 f);
FuShu4 csch(FuShu4 f);
FuShu4 sech(FuShu4 f);

FuShu4 sqrt(FuShu4 f);
FuShu4 asin(FuShu4 f);
FuShu4 acos(FuShu4 f);
FuShu4 atan(FuShu4 f);

FuShu4 pow(FuShu4 f,long double n);//����n�η���
FuShu4 pow(FuShu4 f,FuShu4 n);//����n�η���