#include <string>
#include <vector>
#include "windows.h"
#include "Resource.h"
#include "FuShu.h"
#include "FuShu4.h"
#include "time.h"
using std::string;
using std::vector;

HWND MAIN_HANDLE=NULL;
HWND MAIN_DIALOG=NULL;
HWND MAIN_INSTANCE=NULL;
HDC  hMemDC=NULL;

#define SIGN(t) (t>=0?1:-1)

//T0������������
#define FRACTAL_T0_MANDELBROT   1//Mandelbrot��
#define FRACTAL_T0_JULIA        2//Julia��
#define FRACTAL_T0_NEWTON       4//ţ�ٷ���

//��������
#define PARAMS_ATTR_T0          1
#define PARAMS_ATTR_T1          2
#define PARAMS_ATTR_T3          4
#define PARAMS_ATTR_T2          8
#define PARAMS_ATTR_T4          16
struct PARAMS//�����ṹ��(����DLL��)
{
    int attr;//����
    HWND main_handle;//�����ھ��
    HWND main_dlg;//���Ի�����
    HINSTANCE main_instance;//���洰��ʵ�����
    HWND hTabDlg;//Tab�ӶԻ�����
    HDC  hmemdc;//����DC
    vector<string> gongshi_array;//���湫ʽ�ַ���������
    RECT myrect;//�����ѡ��������
    BOOL isfangsuo;//�Ƿ��ڷ���
};

long double kx=0.006,ky=0.006,fx1=0,fx2=0,fy1=0,fy2=0;//�Ŵ����
PARAMS Params;//����

//z*z+c(T0)
DWORD WINAPI GongShi_1(LPVOID lpParam)
{
    RECT r;
    GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

    HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC
    //��������
    HBRUSH hbrush=CreateSolidBrush(RGB(0,0,0));
    //FillRect(hDC,&r,hbrush);//������ɫ
    
    //��ʼ���㲢����(���ڴ��л���)
    //========================================================
    long double width=r.right-r.left;
    long double height=r.bottom-r.top;
    unsigned long n;
    double i,j,t;
    int R,G,B;
    long double dx,dy,dz;

    //�õ�����
    //===========================================================
    HWND hT0=Params.hTabDlg;

    //��ɫ����
    int Ar=(int)GetDlgItemInt(hT0,IDC_T0_AR,NULL,FALSE);
    int Ag=(int)GetDlgItemInt(hT0,IDC_T0_AG,NULL,FALSE);
    int Ab=(int)GetDlgItemInt(hT0,IDC_T0_AB,NULL,FALSE);
    int Br=(int)GetDlgItemInt(hT0,IDC_T0_BR,NULL,FALSE);
    int Bg=(int)GetDlgItemInt(hT0,IDC_T0_BG,NULL,FALSE);
    int Bb=(int)GetDlgItemInt(hT0,IDC_T0_BB,NULL,FALSE);

    unsigned long Rmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_RMAX,NULL,FALSE);//Rmax
    unsigned long Nmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_NMAX,NULL,FALSE);//Nmax

    long double Newton_Rmax=(long double)1/Rmax;//Newton���ݰ뾶


    //ѡ����Ƶļ���
    int select;
    if(IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON))
    {
        select=FRACTAL_T0_MANDELBROT;//Mandelbrot
    }
    else if(!IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON)\
        &&IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA))
    {
        select=FRACTAL_T0_JULIA;//Julia
    }
    else if(!IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA)\
        &&IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON))
    {
        select=FRACTAL_T0_NEWTON;//Newton
    }
    else select=0;

    //�õ�ά��
    int weishu=2;
    if(IsDlgButtonChecked(hT0,IDC_RBN_T0_3))weishu=3;

    if(weishu==2)
    {

        FuShu c;
        FuShu z;
        if(select==FRACTAL_T0_JULIA||select==FRACTAL_T0_NEWTON)//��ΪJulia������Newton����
        {
            //�õ�����C
            HWND hReal=GetDlgItem(hT0,IDC_T0_C_REAL);
            HWND hImag=GetDlgItem(hT0,IDC_T0_C_IMAG);
            char real[20],imag[20];
            GetWindowText(hReal,real,20);
            GetWindowText(hImag,imag,20);

            //���ַ���תΪdoubleֵ
            char *st;
            long double Real=(long double)strtod(real,&st);
            long double Imag=(long double)strtod(imag,&st);

            c.Get(Real,Imag);
        }

        //===========================================================
        //�Ŵ��㷨
        //===================================================
        RECT myrect={0,0,width,height};//ȱʡ�Ŵ��������
        if(Params.isfangsuo)//�õ��Ŵ��������
        {
            myrect=Params.myrect;
        
        }
        else //����һ�λ������ó�ʼ��ƽ������
        {
            kx=0.006;
            ky=0.006;
            fx1=-(width/2)*kx;
            fy1=-(height/2)*ky;
            fx2=(width/2)*kx;
            fy2=(height/2)*ky;
        }

        fx1=fx1+(myrect.left-0)*kx;//��ηŴ�
        fy1=fy1+(myrect.top-0)*ky;
        fx2=fx2+(myrect.right-width)*kx;
        fy2=fy2+(myrect.bottom-height)*ky;

        kx*=(myrect.right-myrect.left)/width;//����Ŵ����
        ky*=(myrect.bottom-myrect.top)/height;

        //��ʾ�Ŵ����
        char bitstr[20];
        sprintf(bitstr,"���λ���1.0(�Ŵ����: x�� 1:%ld,y�� 1:%ld)",(long)(1/kx),(long)(1/ky));
        SetWindowText(MAIN_DIALOG,bitstr);

        //��ͼ
        //========================================
        //��ʧ��Ŵ�
        StretchBlt(hDC,0,0,width,height,hMemDC,myrect.left,myrect.top,\
            myrect.right-myrect.left,myrect.bottom-myrect.top,SRCCOPY);

        FillRect(hMemDC,&r,hbrush);//������ɫ
        //=========================================
        dx=(fx2-fx1)/width;
        dy=(fy2-fy1)/height;

        FuShu z1;

        for(i=0;i<height;i++)
        {
            for(j=0;j<width;j++)
            {
                if(select==FRACTAL_T0_MANDELBROT)
                {
                    c.Get(fx1+j*dx,fy1+i*dy);
                    z.Get(0);
                }
                else if(select==FRACTAL_T0_JULIA)
                {
                    z.Get(fx1+j*dx,fy1+i*dy);
                }
                else if(select==FRACTAL_T0_NEWTON)
                {
                    z1.Get(fx1+j*dx,fy1+i*dy);
                }
                else goto end;
                n=0;
                while (1)
                {
                    if(select==FRACTAL_T0_MANDELBROT||select==FRACTAL_T0_JULIA)
                    {
                        z=z*z+c;
                        if((z.m2())>=Rmax)
                        {
                            R = n*Ar+Br;
                            G = n*Ag+Bg;
                            B = n*Ab+Bb;
                            if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
                            if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
                            if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
                            SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));
                            break;
                        }
                        if(n++>=Nmax)
                        {
                            break;
                        }
                    }
                    else if(select==FRACTAL_T0_NEWTON)
                    {
                        z=z1-(z1*z1+c)/(2*z1);//z*z+c
                        z1=z-z1;
                        if(z1.m2()>Newton_Rmax)
                        {
                            R = n*Ar+Br;
                            G = n*Ag+Bg;
                            B = n*Ab+Bb;
                            if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
                            if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
                            if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
                            SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));
                        }
                        if(n++>=Nmax)
                        {
                            break;
                        }
                        z1=z;
                    }
                }

            }
                
        BitBlt(hDC,r.left,r.top,r.right-r.left,(int)i,hMemDC,0,0,SRCCOPY);//������
        }
    }
    else if(weishu==3)
    {
        FuShu4 c;
        FuShu4 z;
        if(select==FRACTAL_T0_JULIA)//��ΪJulia��
        {
            //�õ�����C
            HWND hReal_w=GetDlgItem(hT0,IDC_T0_C_REAL);
            HWND hImag_i=GetDlgItem(hT0,IDC_T0_C_IMAG);
            HWND hImag_j=GetDlgItem(hT0,IDC_T0_C_IMAG_J);
            HWND hImag_k=GetDlgItem(hT0,IDC_T0_C_IMAG_K);
            char real_w[20],imag_i[20],imag_j[20],imag_k[20];
            GetWindowText(hReal_w,real_w,20);
            GetWindowText(hImag_i,imag_i,20);
            GetWindowText(hImag_j,imag_j,20);
            GetWindowText(hImag_k,imag_k,20);

            //���ַ���תΪdoubleֵ
            char *st;
            long double Real_w=(long double)strtod(real_w,&st);
            long double Imag_i=(long double)strtod(imag_i,&st);
            long double Imag_j=(long double)strtod(imag_j,&st);
            long double Imag_k=(long double)strtod(imag_k,&st);

            c.Get(Real_w,Imag_i,Imag_j,Imag_k);
        }
        FillRect(hMemDC,&r,hbrush);//������ɫ
        //=========================================
        long double x3_1=-2.1,x3_2=1.7,y3_1=-1,y3_2=1,z3_1=-1.66,z3_2=1.6;
        dx=(x3_2-x3_1)/width;
        dy=(y3_2-y3_1)/height;
        dz=(z3_2-z3_1)/height;

        FuShu4 z1;
        vector<vector<long double > >z_highs;
        z_highs.resize(height);
        for(i=0;i<height;i++)
        {
            z_highs[i].resize(width);
            for(j=0;j<width;j++)
                z_highs[i][j]=-1;
        }
        
        for(i=0;i<height;i++)
            for(j=0;j<width;j++)
                for(t=0;t<height;t++)
                {
                    z.Get(0,x3_1+j*dx,y3_1+i*dy,z3_2-t*dz);
                    
                    n=0;
                    while (1)
                    {
                        z=z*z+c;
                        if((z.m2())>=2)
                        {
                            /*R = n*Ar+Br;
                            G = n*Ag+Bg;
                            B = n*Ab+Bb;
                            if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
                            if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
                            if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
                            SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));*/
                            break;
                        }
                        if(n++>=10)
                        {
                            SetPixel(hDC,(int)(j),(int)(i),RGB(t,t,t));
                            break;
                        }
                        
                    }
                    if(n>=11)
                    {
                        z_highs[i][j]=t;
                        break;
                    }
                
                
                }

        for(i=0;i<height;i++)
            for(j=0;j<width;j++)
            {
                if(z_highs[i][j]>=0)
                SetPixel(hDC,(int)(j),(int)(i),RGB(255,255,255));

            }
        //BitBlt(hDC,r.left,r.top,r.right-r.left,(int)i,hMemDC,0,0,SRCCOPY);//������
    
    }
    //========================================================
end:
    //�ͷ���Դ
    ReleaseDC(MAIN_HANDLE,hDC);

    if(!Params.isfangsuo)//���ڷ���״̬�£�������
    {
        //���û�ͼ��ť
        HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
        EnableWindow(hDraw,TRUE);
        //���ò�����ť
        HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
        EnableWindow(hClear,TRUE);
    }
    //ȡ����ʾ�Ŵ����
    SetWindowText(MAIN_DIALOG,"���λ���1.0");
    return 1;

}
//z^3+c(T0)
DWORD WINAPI GongShi_2(LPVOID lpParam)
{
    RECT r;
    GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

    HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC
    //��������
    HBRUSH hbrush=CreateSolidBrush(RGB(0,0,0));
    //FillRect(hDC,&r,hbrush);//������ɫ
    
    //��ʼ���㲢����(���ڴ��л���)
    //========================================================
    long double width=r.right-r.left;
    long double height=r.bottom-r.top;
    unsigned long n;
    double i,j;
    int R,G,B;
    long double dx,dy;

    //�õ�����
    //===========================================================
    HWND hT0=Params.hTabDlg;

    //��ɫ����
    int Ar=(int)GetDlgItemInt(hT0,IDC_T0_AR,NULL,FALSE);
    int Ag=(int)GetDlgItemInt(hT0,IDC_T0_AG,NULL,FALSE);
    int Ab=(int)GetDlgItemInt(hT0,IDC_T0_AB,NULL,FALSE);
    int Br=(int)GetDlgItemInt(hT0,IDC_T0_BR,NULL,FALSE);
    int Bg=(int)GetDlgItemInt(hT0,IDC_T0_BG,NULL,FALSE);
    int Bb=(int)GetDlgItemInt(hT0,IDC_T0_BB,NULL,FALSE);

    unsigned long Rmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_RMAX,NULL,FALSE);//Rmax
    unsigned long Nmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_NMAX,NULL,FALSE);//Nmax

    long double Newton_Rmax=(long double)1/Rmax;//Newton���ݰ뾶
    //������ɫ����
    vector<COLORREF> ColorArray;
    ColorArray.resize(Nmax);

    for(i=0;i<ColorArray.size();i++)
    {
        R = i*Ar+Br;
        G = i*Ag+Bg;
        B = i*Ab+Bb;
        if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
        if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
        if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;

        ColorArray[i]=RGB(R,G,B);
    }


    //ѡ����Ƶļ���
    int select;
    if(IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON))
    {
        select=FRACTAL_T0_MANDELBROT;//Mandelbrot
    }
    else if(!IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON)\
        &&IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA))
    {
        select=FRACTAL_T0_JULIA;//Julia
    }
    else if(!IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA)\
        &&IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON))
    {
        select=FRACTAL_T0_NEWTON;//Newton
    }
    else select=0;

    FuShu c;
    FuShu z;
    if(select==FRACTAL_T0_JULIA||select==FRACTAL_T0_NEWTON)//��ΪJulia������Newton����
    {
        //�õ�����C
        HWND hReal=GetDlgItem(hT0,IDC_T0_C_REAL);
        HWND hImag=GetDlgItem(hT0,IDC_T0_C_IMAG);
        char real[20],imag[20];
        GetWindowText(hReal,real,20);
        GetWindowText(hImag,imag,20);

        //���ַ���תΪdoubleֵ
        char *st;
        long double Real=(long double)strtod(real,&st);
        long double Imag=(long double)strtod(imag,&st);

        c.Get(Real,Imag);
    
    }

    //===========================================================
    //�Ŵ��㷨
    //===================================================
    RECT myrect={0,0,width,height};//ȱʡ�Ŵ��������
    if(Params.isfangsuo)//�õ��Ŵ��������
    {
        myrect=Params.myrect;
    
    }
    else //����һ�λ������ó�ʼ��ƽ������
    {
        kx=0.006;
        ky=0.006;
        fx1=-(width/2)*kx;
        fy1=-(height/2)*ky;
        fx2=(width/2)*kx;
        fy2=(height/2)*ky;
    }

    fx1=fx1+(myrect.left-0)*kx;//��ηŴ�
    fy1=fy1+(myrect.top-0)*ky;
    fx2=fx2+(myrect.right-width)*kx;
    fy2=fy2+(myrect.bottom-height)*ky;

    kx*=(myrect.right-myrect.left)/width;//����Ŵ����
    ky*=(myrect.bottom-myrect.top)/height;

    //��ʾ�Ŵ����
    char bitstr[20];
    sprintf(bitstr,"���λ���1.0(�Ŵ����: x�� 1:%ld,y�� 1:%ld)",(long)(1/kx),(long)(1/ky));
    SetWindowText(MAIN_DIALOG,bitstr);

    //��ͼ
    //========================================
    //��ʧ��Ŵ�
    StretchBlt(hDC,0,0,width,height,hMemDC,myrect.left,myrect.top,\
        myrect.right-myrect.left,myrect.bottom-myrect.top,SRCCOPY);

    FillRect(hMemDC,&r,hbrush);//������ɫ
    //=========================================
    dx=(fx2-fx1)/width;
    dy=(fy2-fy1)/height;

    FuShu z1;
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            if(select==FRACTAL_T0_MANDELBROT)
            {
                c.Get(fx1+j*dx,fy1+i*dy);
                z.Get(0);
            }
            else if(select==FRACTAL_T0_JULIA)
            {
                z.Get(fx1+j*dx,fy1+i*dy);
            }
            else if(select==FRACTAL_T0_NEWTON)
            {
                z1.Get(fx1+j*dx,fy1+i*dy);
            }
            else goto end;
            n=0;
            while (1)
            {
                if(select==FRACTAL_T0_MANDELBROT||select==FRACTAL_T0_JULIA)
                {
                    z=z*z*z+c;
                    if((z.m2())>=Rmax)
                    {
                        /*R = n*Ar+Br;
                        G = n*Ag+Bg;
                        B = n*Ab+Bb;
                        if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
                        if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
                        if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
                        SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));*/
                        SetPixel(hMemDC,(int)(j),(int)(i),ColorArray[n]);
                        
                        break;
                    }
                    if(n++>=Nmax)
                    {
                        break;
                    }
                    
                }
                else if(select==FRACTAL_T0_NEWTON)
                {
                    z=(0.66666666)*z1-c/(3*z1*z1);//z^3+c
                    z1=z-z1;
                    if(z1.m2()>Newton_Rmax)
                    {
                        /*R = n*Ar+Br;
                        G = n*Ag+Bg;
                        B = n*Ab+Bb;
                        if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
                        if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
                        if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
                        SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));*/
                        SetPixel(hMemDC,(int)(j),(int)(i),ColorArray[n]);
                        
                    }
                    if(n++>=Nmax)
                    {
                        break;
                    }
                    z1=z;
                }

                
            }

        }
            
    BitBlt(hDC,r.left,r.top,r.right-r.left,(int)i,hMemDC,0,0,SRCCOPY);//������
    }
    //========================================================
end:
    //�ͷ���Դ
    ReleaseDC(MAIN_HANDLE,hDC);

    if(!Params.isfangsuo)//���ڷ���״̬�£�������
    {
        //���û�ͼ��ť
        HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
        EnableWindow(hDraw,TRUE);
        //���ò�����ť
        HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
        EnableWindow(hClear,TRUE);
    }
    //ȡ����ʾ�Ŵ����
    SetWindowText(MAIN_DIALOG,"���λ���1.0");
    return 1;

}
//sinz+c(T0)
DWORD WINAPI GongShi_3(LPVOID lpParam)
{
    RECT r;
    GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

    HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC
    //��������
    HBRUSH hbrush=CreateSolidBrush(RGB(0,0,0));
    //FillRect(hDC,&r,hbrush);//������ɫ
    
    //��ʼ���㲢����(���ڴ��л���)
    //========================================================
    long double width=r.right-r.left;
    long double height=r.bottom-r.top;
    unsigned long n;
    double i,j;
    int R,G,B;
    long double dx,dy;

    //�õ�����
    //===========================================================
    HWND hT0=Params.hTabDlg;

    //��ɫ����
    int Ar=(int)GetDlgItemInt(hT0,IDC_T0_AR,NULL,FALSE);
    int Ag=(int)GetDlgItemInt(hT0,IDC_T0_AG,NULL,FALSE);
    int Ab=(int)GetDlgItemInt(hT0,IDC_T0_AB,NULL,FALSE);
    int Br=(int)GetDlgItemInt(hT0,IDC_T0_BR,NULL,FALSE);
    int Bg=(int)GetDlgItemInt(hT0,IDC_T0_BG,NULL,FALSE);
    int Bb=(int)GetDlgItemInt(hT0,IDC_T0_BB,NULL,FALSE);

    unsigned long Rmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_RMAX,NULL,FALSE);//Rmax
    unsigned long Nmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_NMAX,NULL,FALSE);//Nmax

    long double Newton_Rmax=(long double)1/Rmax;//Newton���ݰ뾶


    //ѡ����Ƶļ���
    int select;
    if(IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON))
    {
        select=FRACTAL_T0_MANDELBROT;//Mandelbrot
    }
    else if(!IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON)\
        &&IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA))
    {
        select=FRACTAL_T0_JULIA;//Julia
    }
    else if(!IsDlgButtonChecked(hT0,IDC_RBN_T0_MANDELBROT)\
        &&!IsDlgButtonChecked(hT0,IDC_RBN_T0_JULIA)\
        &&IsDlgButtonChecked(hT0,IDC_RBN_T0_NEWTON))
    {
        select=FRACTAL_T0_NEWTON;//Newton
    }
    else select=0;

    FuShu c;
    FuShu z;
    if(select==FRACTAL_T0_JULIA||select==FRACTAL_T0_NEWTON)//��ΪJulia������Newton����
    {
        //�õ�����C
        HWND hReal=GetDlgItem(hT0,IDC_T0_C_REAL);
        HWND hImag=GetDlgItem(hT0,IDC_T0_C_IMAG);
        char real[20],imag[20];
        GetWindowText(hReal,real,20);
        GetWindowText(hImag,imag,20);

        //���ַ���תΪdoubleֵ
        char *st;
        long double Real=(long double)strtod(real,&st);
        long double Imag=(long double)strtod(imag,&st);

        c.Get(Real,Imag);
    
    }

    //===========================================================
    //�Ŵ��㷨
    //===================================================
    RECT myrect={0,0,width,height};//ȱʡ�Ŵ��������
    if(Params.isfangsuo)//�õ��Ŵ��������
    {
        myrect=Params.myrect;
    
    }
    else //����һ�λ������ó�ʼ��ƽ������
    {
        kx=0.006;
        ky=0.006;
        fx1=-(width/2)*kx;
        fy1=-(height/2)*ky;
        fx2=(width/2)*kx;
        fy2=(height/2)*ky;
    }

    fx1=fx1+(myrect.left-0)*kx;//��ηŴ�
    fy1=fy1+(myrect.top-0)*ky;
    fx2=fx2+(myrect.right-width)*kx;
    fy2=fy2+(myrect.bottom-height)*ky;

    kx*=(myrect.right-myrect.left)/width;//����Ŵ����
    ky*=(myrect.bottom-myrect.top)/height;

    //��ʾ�Ŵ����
    char bitstr[20];
    sprintf(bitstr,"���λ���1.0(�Ŵ����: x�� 1:%ld,y�� 1:%ld)",(long)(1/kx),(long)(1/ky));
    SetWindowText(MAIN_DIALOG,bitstr);

    //��ͼ
    //========================================
    //��ʧ��Ŵ�
    StretchBlt(hDC,0,0,width,height,hMemDC,myrect.left,myrect.top,\
        myrect.right-myrect.left,myrect.bottom-myrect.top,SRCCOPY);

    FillRect(hMemDC,&r,hbrush);//������ɫ
    //=========================================
    dx=(fx2-fx1)/width;
    dy=(fy2-fy1)/height;

    FuShu z1;
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            if(select==FRACTAL_T0_MANDELBROT)
            {
                c.Get(fx1+j*dx,fy1+i*dy);
                z.Get(0);
            }
            else if(select==FRACTAL_T0_JULIA)
            {
                z.Get(fx1+j*dx,fy1+i*dy);
            }
            else if(select==FRACTAL_T0_NEWTON)
            {
                z1.Get(fx1+j*dx,fy1+i*dy);
            }
            else goto end;
            n=0;
            while (1)
            {
                if(select==FRACTAL_T0_MANDELBROT||select==FRACTAL_T0_JULIA)
                {
                    z=sin(z)+c;
                    if((z.m2())>=Rmax)
                    {
                        R = n*Ar+Br;
                        G = n*Ag+Bg;
                        B = n*Ab+Bb;
                        if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
                        if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
                        if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
                        SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));
                        break;
                    }
                    if(n++>=Nmax)
                    {
                        break;
                    }
                }
                else if(select==FRACTAL_T0_NEWTON)
                {
                    z=z1-(sin(z)+c)/(cos(z));//sin(z)+c
                    z1=z-z1;
                    if(z1.m2()>Newton_Rmax)
                    {
                        R = n*Ar+Br;
                        G = n*Ag+Bg;
                        B = n*Ab+Bb;
                        if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
                        if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
                        if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
                        SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));
                        
                    }
                    if(n++>=Nmax)
                    {
                        break;
                    }
                    z1=z;
                }

                
            }

        }
            
    BitBlt(hDC,r.left,r.top,r.right-r.left,(int)i,hMemDC,0,0,SRCCOPY);//������
    }
    //========================================================
end:
    //�ͷ���Դ
    ReleaseDC(MAIN_HANDLE,hDC);

    if(!Params.isfangsuo)//���ڷ���״̬�£�������
    {
        //���û�ͼ��ť
        HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
        EnableWindow(hDraw,TRUE);
        //���ò�����ť
        HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
        EnableWindow(hClear,TRUE);
    }
    //ȡ����ʾ�Ŵ����
    SetWindowText(MAIN_DIALOG,"���λ���1.0");
    return 1;

}
//a*x+(2*(1-a)*x*x)/(1+x*x)(T0)
/*DWORD WINAPI GongShi_11(LPVOID lpParam)
{
    RECT r;
    GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

    HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC
    //��������
    HBRUSH hbrush=CreateSolidBrush(RGB(0,0,0));
    FillRect(hDC,&r,hbrush);//������ɫ
    
    //��ʼ���㲢����(���ڴ��л���)
    //========================================================
    long double width=r.right-r.left;
    long double height=r.bottom-r.top;
    unsigned long n;
    double i,j;
    int R,G,B;
    long double a,w,x,y,z,b;
    //�õ�����
    //===========================================================
    HWND hT0=Params.hTabDlg;

    //��ɫ����
    int Ar=(int)GetDlgItemInt(hT0,IDC_T0_AR,NULL,FALSE);
    int Ag=(int)GetDlgItemInt(hT0,IDC_T0_AG,NULL,FALSE);
    int Ab=(int)GetDlgItemInt(hT0,IDC_T0_AB,NULL,FALSE);
    int Br=(int)GetDlgItemInt(hT0,IDC_T0_BR,NULL,FALSE);
    int Bg=(int)GetDlgItemInt(hT0,IDC_T0_BG,NULL,FALSE);
    int Bb=(int)GetDlgItemInt(hT0,IDC_T0_BB,NULL,FALSE);

    unsigned long Rmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_RMAX,NULL,FALSE);//Rmax
    unsigned long Nmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_NMAX,NULL,FALSE);//Nmax

    HWND hB_Edit=GetDlgItem(hT0,IDC_T0_B);//����B
    HWND hA_Edit=GetDlgItem(hT0,IDC_T0_A);//ϵ��A
    HWND hN_Edit=GetDlgItem(hT0,IDC_T0_N);//��������N
    char buffer[100],*st;
    GetWindowText(hA_Edit,buffer,100);
    a=(long double)strtod(buffer,&st);
    GetWindowText(hB_Edit,buffer,100);
    b=(long double)strtod(buffer,&st);
    GetWindowText(hN_Edit,buffer,100);
    n=(long double)strtod(buffer,&st);

    FillRect(hMemDC,&r,hbrush);//������ɫ
    //=========================================
    x=0.004;
    y=0.004;
    w=1;
    for(i=0;i<n;i++)
    {
        z=x;
        x=b*y+w;
        w=a*x+(2*(1-a)*x*x)/(1+x*x);
        //w=SIGN(x)*(a*x+2*(1-a)*x*x/(1+x*x))+sin(a+x);
        //w=2*PI*a*5*SIGN(x)+PI+cos(x)+sin(PI+sin(PI+x));
        //w=-0.05*a*x+a*(PI-a*x)*x*x/(1+x*x);
        y=w-z;

        if((int)(width/2+x*10)>0&&(int)(width/2+x*10)<width&&(int)(height/2-y*10)>0&&(int)(height/2-y*10)<height)
        {
            R = i*Ar+Br;
            G = i*Ag+Bg;
            B = i*Ab+Bb;
            if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
            if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
            if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
            SetPixel(hMemDC,(int)(width/2+x*10),(int)(height/2-y*10),RGB(255,255,255));
        }
    
    }
    BitBlt(hDC,r.left,r.top,(int)width,(int)height,hMemDC,0,0,SRCCOPY);
    
    //========================================================

    //�ͷ���Դ
    ReleaseDC(MAIN_HANDLE,hDC);

    if(!Params.isfangsuo)//���ڷ���״̬�£�������
    {
        //���û�ͼ��ť
        HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
        EnableWindow(hDraw,TRUE);
        //���ò�����ť
        HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
        EnableWindow(hClear,TRUE);
    }
    //ȡ����ʾ�Ŵ����
    SetWindowText(MAIN_DIALOG,"���λ���1.0");
    return 1;

}*/


//���������������ͼ
extern "C" int _declspec(dllexport)StartDrawByDLL(LPVOID lpParam)
{
    PARAMS *params=(PARAMS *)lpParam;//��ȡ����
    MAIN_HANDLE=params->main_handle;
    MAIN_DIALOG=params->main_dlg;
    hMemDC=params->hmemdc;//����DC
    int attr=params->attr;//����

    Params=*params;//�������в����Ŀ���

    if(attr==PARAMS_ATTR_T0)
    {
        HWND hT0=params->hTabDlg;//��ȡTAB�ӶԻ�����
        char gongshi[260];
        string GongShi;
        //�õ���ʽ
        HWND hEdit=GetDlgItem(hT0,IDC_T0_GONGSHI_EDT);
        GetWindowText(hEdit,gongshi,260);
        GongShi=gongshi;

        HANDLE hthread;
        if(GongShi.compare("z*z+c")==0||GongShi.compare("z^2+c")==0)
        {
            hthread=CreateThread(NULL,0,GongShi_1,0,0,0);//�����̻߳�ͼ
            CloseHandle(hthread);

            return 1;
        }
        else if(GongShi.compare("z*z*z+c")==0||GongShi.compare("z^3+c")==0)
        {
            hthread=CreateThread(NULL,0,GongShi_2,0,0,0);//�����̻߳�ͼ
            CloseHandle(hthread);

            return 1;
        }
        else if(GongShi.compare("sinz+c")==0||GongShi.compare("sin(z)+c")==0)
        {
            hthread=CreateThread(NULL,0,GongShi_3,0,0,0);//�����̻߳�ͼ
            CloseHandle(hthread);

            return 1;
        }
        /*else if(GongShi.compare("a*x+(2*(1-a)*x*x)/(1+x*x)")==0||GongShi.compare("a*x+(2*(1-a)*(x^2))/(1+x^2)")==0)
        {
            hthread=CreateThread(NULL,0,GongShi_11,0,0,0);//�����̻߳�ͼ
            CloseHandle(hthread);

            return 1;
        }*/
        else return 0;

    }
    else return 0;


    return 1;
}
//�����������õ����п�湫ʽ
extern "C" int _declspec(dllexport)GetAllGongShi(LPVOID lpParam)
{
    PARAMS *params=(PARAMS *)lpParam;//��ȡ����
    int attr=params->attr;
    if(attr==PARAMS_ATTR_T0)
    {
        params->gongshi_array.push_back("z*z+c");
        params->gongshi_array.push_back("z^3+c");
        params->gongshi_array.push_back("sinz+c");
        //params->gongshi_array.push_back("a*x+(2*(1-a)*x*x)/(1+x*x)");
    }
    else return 0;

    return 1;
}
