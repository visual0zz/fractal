#include "FuShu4.h"

FuShu4::FuShu4()
{
	w=0;
	x=0;
	y=0;
	z=0;

}
FuShu4::FuShu4(long double m)
{
	w=m;
	x=0;
	y=0;
	z=0;
}
FuShu4::FuShu4(long double m,long double a,long double b,long double c)
{
	w=m;
	x=a;
	y=b;
	z=c;

}
FuShu4::~FuShu4()
{

}

void FuShu4::Get(long double m,long double a,long double b,long double c)
{
	w=m;
	x=a;
	y=b;
	z=c;

}
void FuShu4::Get(long double m)
{
	w=m;
	x=0;
	y=0;
	z=0;

}
void FuShu4::Get(FuShu4 f)
{
	w=f.w;
	x=f.x;
	y=f.y;
	z=f.z;

}
FuShu4 FuShu4::operator +(FuShu4 f)
{
	FuShu4 r;
	r.w=w+f.w;
	r.x=x+f.x;
	r.y=y+f.y;
	r.z=z+f.z;

	return r;

}

FuShu4 FuShu4::operator -(FuShu4 f)
{

	FuShu4 r;
	r.w=w-f.w;
	r.x=x-f.x;
	r.y=y-f.y;
	r.z=z-f.z;

	return r;

}

FuShu4 FuShu4::operator *(FuShu4 f)
{
	FuShu4 r;
	r.w=w*f.w-x*f.x-y*f.y-z*f.z;
	r.x=w*f.x+x*f.w+y*f.z-z*f.y;
	r.y=w*f.y-x*f.z+y*f.w+z*f.x;
	r.z=w*f.z+x*f.y-y*f.x+z*f.w;

	return r;

}

FuShu4 FuShu4::operator /(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧�ֳ�������","����!",MB_OK);
	return r;

}

/*FuShu4 FuShu4::operator =(FuShu4 f)
{
	FuShu4 r;
	r.a=f.a;
	r.b=f.b;

	a=f.a;
	b=f.b;

	return r;
}*/
FuShu4 FuShu4::operator =(long double m)
{
	FuShu4 r;
	r.w=m;
	r.x=0;
	r.y=0;
	r.z=0;

	w=m;
	x=0;
	y=0;
	z=0;

	return r;

}
FuShu4 FuShu4::operator +(long double m)
{
	FuShu4 r;
	r.w=w+m;
	r.x=x;
	r.y=y;
	r.z=z;

	return r;

}

FuShu4 FuShu4::operator -(long double m)
{

	FuShu4 r;
	r.w=w-m;
	r.x=x;
	r.y=y;
	r.z=z;

	return r;

}

FuShu4 FuShu4::operator *(long double m)
{
	FuShu4 r;
	r.w=w*m;
	r.x=x*m;
	r.y=y*m;
	r.z=z*m;

	return r;

}

FuShu4 FuShu4::operator /(long double m)
{
	FuShu4 r;
	if(m==0)
	{
		//MessageBox(NULL,"����Ϊ0","����!",MB_OK);
		r.w=0;
		r.x=0;
		r.y=0;
		r.z=0;
		return r;
	}
	
	r.w=w/m;
	r.x=x/m;
	r.y=y/m;
	r.z=z/m;
	
	return r;

}
//ǰ׺����
FuShu4 FuShu4::operator ++()
{
	w++;//������
	FuShu4 r;

	r.w=w;//��ֵ
	r.x=x;
	r.y=y;
	r.z=z;

	return r;

}
//��׺����
FuShu4 FuShu4::operator ++(int)
{
	FuShu4 r;
	r.w=w;//�ȸ�ֵ
	r.x=x;
	r.y=y;
	r.z=z;

	w++;//������
	return r;

}
FuShu4 operator +(long double m,FuShu4 f)
{
	FuShu4 r;
	r.w=m+f.w;
	r.x=f.x;
	r.y=f.y;
	r.z=f.z;

	return r;

}
FuShu4 operator -(long double m,FuShu4 f)
{
	FuShu4 r;
	r.w=m+f.w;
	r.x=0-f.x;
	r.y=0-f.y;
	r.z=0-f.z;

	return r;

}
FuShu4 operator *(long double m,FuShu4 f)
{
	FuShu4 r;
	r.w=m*f.w;
	r.x=m*f.x;
	r.y=m*f.y;
	r.z=m*f.z;

	return r;

}
FuShu4 operator /(long double m,FuShu4 f)
{
	FuShu4 r;

	MessageBox(NULL,"��Ԫ���ݲ�֧�ֳ�������","����!",MB_OK);
	return r;

}

FuShu4 cos(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'cos'����","����!",MB_OK);

	return r;

}
FuShu4 sin(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'sin'����","����!",MB_OK);

	return r;

}
FuShu4 tan(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'tan'����","����!",MB_OK);

	return r;

}
FuShu4 cot(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'cot'����","����!",MB_OK);
	return r;

}
FuShu4 exp(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'exp'����","����!",MB_OK);

	return r;

}
FuShu4 pow(FuShu4 f,long double n)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'pow'����","����!",MB_OK);
	return r;

}
FuShu4 pow(FuShu4 f,FuShu4 n)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'pow'����","����!",MB_OK);
	return r;

}
long double FuShu4::m2()
{
	long double m=w*w+x*x+y*y+z*z;
	return m;

}
long double FuShu4::m()
{
	long double m=w*w+x*x+y*y+z*z;
	return sqrt(m);

}
FuShu4 cosh(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'cosh'����","����!",MB_OK);

	return r;

}
FuShu4 sinh(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'sinh'����","����!",MB_OK);

	return r;

}
FuShu4 tanh(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'tanh'����","����!",MB_OK);

	return r;

}
FuShu4 coth(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'coth'����","����!",MB_OK);
	return r;

}
FuShu4 ln(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'ln'����","����!",MB_OK);
	return r;

}
FuShu4 log(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'log'����","����!",MB_OK);
	return r;

}
FuShu4 sqrt(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'sqrt'����","����!",MB_OK);
	return r;

}
FuShu4 atan(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'atan'����","����!",MB_OK);
	return r;

}
FuShu4 asin(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'asin'����","����!",MB_OK);
	return r;

}
FuShu4 acos(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'acos'����","����!",MB_OK);
	return r;

}
FuShu4 csc(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'csc'����","����!",MB_OK);
	return r;

}
FuShu4 csch(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'csch'����","����!",MB_OK);
	return r; 

}
FuShu4 sec(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'sec'����","����!",MB_OK);
	return r;

}
FuShu4 sech(FuShu4 f)
{
	FuShu4 r;
	MessageBox(NULL,"��Ԫ���ݲ�֧��'sech'����","����!",MB_OK);
	return r;

}