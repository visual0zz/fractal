#include <windows.h>
#include "math.h"

#define PI 3.14159265358979//Բ����
#define E 2.71828182845904//��Ȼ����

#define LN(a) (log(a)/log(E))

//������
class FuShu  
{
public:
	long double b;//a+bi
	long double a;
public:
	friend FuShu operator+(long double m,FuShu f);
	friend FuShu operator-(long double m,FuShu f);
	friend FuShu operator*(long double m,FuShu f);
	friend FuShu operator/(long double m,FuShu f);

	FuShu operator++();//ǰ׺����
	FuShu operator++(int);//��׺����

	FuShu operator=(long double m);
	FuShu operator*(long double m);
	FuShu operator-(long double m);
	FuShu operator+(long double m);
	FuShu operator/(long double m);

	//FuShu operator=(FuShu f);
	FuShu operator*(FuShu f);
	FuShu operator-(FuShu f);
	FuShu operator+(FuShu f);
	FuShu operator/(FuShu f);
	
public:
	FuShu();
	FuShu(long double m);
	FuShu(long double m,long double n);
	virtual ~FuShu();

public:
	long double m();//����ģ
	long double m2();//����ģ��ƽ��
	void Get(long double m,long double n);
	void Get(long double m);
	void Get(FuShu f);

};

FuShu cos(FuShu f);
FuShu sin(FuShu f);
FuShu tan(FuShu f);
FuShu cot(FuShu f);
FuShu cosh(FuShu f);
FuShu sinh(FuShu f);
FuShu tanh(FuShu f);
FuShu coth(FuShu f);
FuShu exp(FuShu f);
FuShu ln(FuShu f);
FuShu log(FuShu f);//��ʮΪ��

FuShu csc(FuShu f);
FuShu sec(FuShu f);
FuShu csch(FuShu f);
FuShu sech(FuShu f);

FuShu sqrt(FuShu f);
FuShu asin(FuShu f);
FuShu acos(FuShu f);
FuShu atan(FuShu f);

FuShu pow(FuShu f,long double n);//����n�η���
FuShu pow(FuShu f,FuShu n);//����n�η���

