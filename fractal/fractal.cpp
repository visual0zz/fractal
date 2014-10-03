//VC-Win32
#include <windows.h>
#include <windowsx.h>
#include "time.h"

#include "FuShu.h"//������
#include "FuShu4.h"//��Ԫ����
#include "GongShi.h"

//��Դ�����ļ�
#include "Resource.h"
#include "commctrl.h"//�ؼ�ͷ�ļ�
#pragma comment(lib,"comctl32.lib")//����ͨ�ÿؼ���
#pragma comment(lib,"comdlg32.lib")//��

#define WINDOW_CLASS_NAME "Fractal"//��������
#define WINDOW_TITLE "���λ��� 1.0"//���ڱ���
#define WINDOW_WIDTH 640//���ڳߴ�
#define WINDOW_HEIGHT 480

//T0������������
#define FRACTAL_T0_MANDELBROT	1//Mandelbrot��
#define FRACTAL_T0_JULIA		2//Julia��
#define FRACTAL_T0_NEWTON		4//ţ�ٷ���

//ȫ�ֱ���
HWND          MAIN_HANDLE        =NULL;//���������ھ��
HINSTANCE     MAIN_INSTANCE      =NULL;//���洰��ʵ�����

HWND          MAIN_DIALOG		 =NULL;//���Ի�����
HWND          hT0		         =NULL;//Tab�ӶԻ�����
HWND          hT1		         =NULL;//Tab�ӶԻ�����
HWND          hT2		         =NULL;//Tab�ӶԻ�����
HWND          hT3		         =NULL;//Tab�ӶԻ�����
HWND          hT4		         =NULL;//Tab�ӶԻ�����
HWND          hT10		         =NULL;//Tab�ӶԻ�����
HDC           hMemDC             =NULL;//����DC

HANDLE        hDrawThreadT0      =NULL;//��ͼ�߳̾��
HANDLE        hDrawThreadT1      =NULL;//��ͼ�߳̾��
HANDLE        hDrawThreadT2      =NULL;//��ͼ�߳̾��
HANDLE        hDrawThreadT3      =NULL;//��ͼ�߳̾��
HANDLE        hDrawThreadT4      =NULL;//��ͼ�߳̾��
HANDLE        hDrawThreadT10     =NULL;//��ͼ�߳̾��
HWND		  hTab               =NULL;//Tab�ؼ����

HINSTANCE     hStockDll          =NULL;//��湫ʽ��̬���ӿ���
RECT		  MyRect			 ={0,0,0,0};//�����ѡ��������
RECT		  MyOldRect			 ={0,0,0,0};//�������ԭ����������
POINT         OldPoint		     ={0,0};//�������ԭ���������
BOOL		  IsCapture			 =FALSE;//����Ƿ񲶻�
BOOL		  IsFangSuo			 =FALSE;//�Ƿ��ڷ���

BOOL		  IsScreen			 =FALSE;//�Ƿ�ȫ��

COLORREF	  BackGroundColor	 =0;//���屳��ɫ
COLORREF	  T3_Color			 =RGB(255,255,255);//T3�Ի�������ɫ

WNDPROC		  OldT3ColorMsgProc  =NULL;//��¼ԭ��T3�Ի�����ɫ��̬�ؼ���Ϣ��������ȫ�ֱ���

long double kx=0.006,ky=0.006,fx1=0,fx2=0,fy1=0,fy2=0;//�Ŵ����

//��������
#define PARAMS_ATTR_T0			1
#define PARAMS_ATTR_T1			2
#define PARAMS_ATTR_T3			4
#define PARAMS_ATTR_T2			8
#define PARAMS_ATTR_T4			16
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

//IFS��ṹ��
struct IFS
{
	double a,b,c,d,e,f;//�任ϵ��
	double p;//����
	double psum;//ǰ������ifs�ĸ��ʺͣ����ڼ�����ʿռ�
	double x0,y0;//��ʼ����
	COLORREF color;//��ɫ
};

//LS�ķ�����ṹ��
struct LS
{
	string head;//����ͷ
	string body;//������
	double p;//����
};

//LS�ڵ�
struct LSDOT
{
	double x,y,z;//����
	//double xr,yr,zr;//��λ����
	double xdeg,ydeg,zdeg;//����н�
};

#define SIGN(t) (t>=0?1:-1)

typedef int (*StockDllProc)(LPVOID lpParam);
StockDllProc StartDrawByDLL=NULL;//��̬���ӿ⺯��(�����ͼ)
StockDllProc GetAllGongShiFromDLL=NULL;//��̬���ӿ⺯��(�õ����п�溯��) 

string        rules_str;//LS��������
string		  wenfastr;//LS�ķ��ı�
long		  ls_count=0;//LS��������

//T2�������ƶԻ�����Ϣ����(�����ķ�����������)
BOOL WINAPI Dlg_T2_SetName(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hName=GetDlgItem(hWnd,IDC_T4_SETNAME);
	HWND hN=GetDlgItem(hT2,IDC_T2_N);
	HWND hLen=GetDlgItem(hT2,IDC_T2_LEN);
	HWND hX0=GetDlgItem(hT2,IDC_T2_X0);
	HWND hY0=GetDlgItem(hT2,IDC_T2_Y0);
	HWND hZ0=GetDlgItem(hT2,IDC_T2_Z0);
	HWND hAngle=GetDlgItem(hT2,IDC_T2_DEGREE);
	HWND hWenfa=GetDlgItem(hT2,IDC_T2_WENFA_EDIT);

	//�õ������ļ�·��
	char filename[260];
	string buffer;
	GetModuleFileName(NULL,filename,260);
	buffer=filename;
	buffer.assign(buffer,0,buffer.find_last_of('\\'));
	buffer+="\\LS.ini";

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
		

			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID=LOWORD(wParam);
			int ControlNotify=HIWORD(wParam);
			HWND ControlHandle=(HWND)lParam;

			if(ControlID==IDC_SETNAME_OK)//ȷ��
			{
				char name[256],str[256];
				GetWindowText(hName,name,256);//�õ�����

				int weishu;//�õ�ά��
				if(IsDlgButtonChecked(hT2,IDC_RBN_T2_2)&&!IsDlgButtonChecked(hT2,IDC_RBN_T2_3))
					weishu=2;
				else if(!IsDlgButtonChecked(hT2,IDC_RBN_T2_2)&&IsDlgButtonChecked(hT2,IDC_RBN_T2_3))
					weishu=3;
				else weishu=2;

				//д�������ļ�
				sprintf(str,"%d",weishu);//ά��
				WritePrivateProfileString(name,"WeiShu",str,buffer.c_str());
				GetWindowText(hN,str,256);//��������
				WritePrivateProfileString(name,"N",str,buffer.c_str());
				GetWindowText(hAngle,str,256);//ȱʡ�Ƕ�
				WritePrivateProfileString(name,"Angle",str,buffer.c_str());
				GetWindowText(hLen,str,256);//��λ����
				WritePrivateProfileString(name,"Len",str,buffer.c_str());
				GetWindowText(hX0,str,256);
				WritePrivateProfileString(name,"X0",str,buffer.c_str());
				GetWindowText(hY0,str,256);
				WritePrivateProfileString(name,"Y0",str,buffer.c_str());
				GetWindowText(hZ0,str,256);
				WritePrivateProfileString(name,"Z0",str,buffer.c_str());
				

				char wenfa[10000];
				GetWindowText(hWenfa,wenfa,10000);
				string wenfastr(wenfa);
				int index=wenfastr.find_first_of("\r\n");
				int endindex=wenfastr.find_first_of('@');
				int oldindex=0;
				int i=0;
				while(index!=-1&&oldindex<index&&index<endindex)//д�����
				{
					sprintf(str,"Rule_%d",i++);
					string strbuf(wenfa,oldindex,index-oldindex);
					WritePrivateProfileString(name,str,strbuf.c_str(),buffer.c_str());
					oldindex=index+2;
					index=wenfastr.find_first_of("\r\n",oldindex);
				}
				//==========================

				EndDialog(hWnd,NULL);
			}
			else if(ControlID==IDC_SETNAME_CANCEL)//ȡ��
			{
			
				EndDialog(hWnd,NULL);
			}
	
			return TRUE;
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hWnd,NULL);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;

}
//T2��湫ʽ�Ի�����Ϣ����
BOOL WINAPI DlgT2Stock(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hN=GetDlgItem(hT2,IDC_T2_N);
	HWND hLen=GetDlgItem(hT2,IDC_T2_LEN);
	HWND hX0=GetDlgItem(hT2,IDC_T2_X0);
	HWND hY0=GetDlgItem(hT2,IDC_T2_Y0);
	HWND hZ0=GetDlgItem(hT2,IDC_T2_Z0);
	HWND hAngle=GetDlgItem(hT2,IDC_T2_DEGREE);
	HWND hWenfa=GetDlgItem(hT2,IDC_T2_WENFA_EDIT);

	//�õ������ļ�·��
	char filename[260];
	string buffer;
	GetModuleFileName(NULL,filename,260);
	buffer=filename;
	buffer.assign(buffer,0,buffer.find_last_of('\\'));
	buffer+="\\LS.ini";

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
		
			//�����б��
			char inibuffer[1000];
			//�õ����н�����inibuffer,ÿ��������0����������2��0����
			if(GetPrivateProfileSectionNames(inibuffer,1000,buffer.c_str()))//�����ļ��ɹ�
			{
				char *p=inibuffer;
				while(*(p))
				{
					SendDlgItemMessage(hWnd,IDC_LST_STOCK,LB_ADDSTRING,0,(LPARAM)p);//���浽�б��
					p+=strlen(p)+1;//��������һ������
				
				}
			}
			
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID=LOWORD(wParam);
			int ControlNotify=HIWORD(wParam);
			HWND ControlHandle=(HWND)lParam;
			if(ControlID==IDC_LST_STOCK)
			{
				if(ControlNotify==LBN_DBLCLK)//��Ϊ˫��
				{
					char name[260];
					int sel=SendMessage(ControlHandle,LB_GETCURSEL,0,0);//�õ���ǰ����
					SendMessage(ControlHandle,LB_GETTEXT,(WPARAM)sel,(LPARAM)name);//�õ��ķ���

					char str[256];
					GetPrivateProfileString(name,"WeiShu","2",str,256,buffer.c_str());//�õ�ά��
					int select=atol(str);
					if(select==3)
					{
						CheckDlgButton(hT2,IDC_RBN_T2_3,BST_CHECKED);
						CheckDlgButton(hT2,IDC_RBN_T2_2,BST_UNCHECKED);
					}
					else
					{
						CheckDlgButton(hT2,IDC_RBN_T2_2,BST_CHECKED);
						CheckDlgButton(hT2,IDC_RBN_T2_3,BST_UNCHECKED);
					
					}
					GetPrivateProfileString(name,"N","0",str,256,buffer.c_str());//�õ���������
					SetWindowText(hN,str);
					GetPrivateProfileString(name,"Angle","0",str,256,buffer.c_str());//�õ�ȱʡ�Ƕ�
					SetWindowText(hAngle,str);
					GetPrivateProfileString(name,"Len","1",str,256,buffer.c_str());
					SetWindowText(hLen,str);
					GetPrivateProfileString(name,"X0","0",str,256,buffer.c_str());
					SetWindowText(hX0,str);
					GetPrivateProfileString(name,"Y0","0",str,256,buffer.c_str());
					SetWindowText(hY0,str);
					GetPrivateProfileString(name,"Z0","0",str,256,buffer.c_str());
					SetWindowText(hZ0,str);

					string wenfa,strbuf;
					GetPrivateProfileString(name,"Rule_0","",str,256,buffer.c_str());
					strbuf=str;
					int i=1;
					char indexstr[20];
					while(strbuf.size()>0)
					{
						wenfa+=strbuf;
						wenfa+="\r\n";
						sprintf(indexstr,"Rule_%d",i++);
						GetPrivateProfileString(name,indexstr,"",str,256,buffer.c_str());
						strbuf=str;
					}
					wenfa+="@\r\n";
					SetWindowText(hWenfa,wenfa.c_str());

					EndDialog(hWnd,NULL);//�رնԻ���
				}
			
			}
	
			return TRUE;
		}
		break;
	case WM_CLOSE:
		{
			EndDialog(hWnd,NULL);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}

//T3�Ի�����ɫ��̬�ؼ�����Ϣ������
LRESULT MyT3ColorMsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT p;
			HDC hDC=BeginPaint(hWnd,&p);
			//��ӿؼ���ɫ
			HBRUSH hbrush=CreateSolidBrush(T3_Color);
			RECT cr;
			GetClientRect(hWnd,&cr);
			FillRect(hDC,&cr,hbrush);

			EndPaint(hWnd,&p);
			
			return TRUE;
		}
		break;

	}
	//���ÿؼ�ԭ������Ϣ������
	return CallWindowProc(OldT3ColorMsgProc,hWnd,uMsg,wParam,lParam);
}

COLORREF Edit_Color=0;//�öԻ���ר��ȫ�ֱ�������ʾ�ؼ�����ɫ
//T4�������ƶԻ�����Ϣ����
BOOL WINAPI Dlg_T4_SetName(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hName=GetDlgItem(hWnd,IDC_T4_SETNAME);
	HWND hN=GetDlgItem(hT4,IDC_T4_N);
	HWND hXmax=GetDlgItem(hT4,IDC_T4_XMAX);
	HWND hYmax=GetDlgItem(hT4,IDC_T4_YMAX);
	HWND hX0=GetDlgItem(hT4,IDC_T4_X0);
	HWND hY0=GetDlgItem(hT4,IDC_T4_Y0);
	HWND hListView=GetDlgItem(hT4,IDC_T4_LISTVIEW);
	HWND hList=GetDlgItem(hT4,IDC_T4_STOCKLIST);

	//�õ������ļ�·��
	char filename[260];
	string buffer;
	GetModuleFileName(NULL,filename,260);
	buffer=filename;
	buffer.assign(buffer,0,buffer.find_last_of('\\'));
	buffer+="\\IFS.ini";

	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			char name[256];
			int sel=SendMessage(hList,LB_GETCURSEL,0,0);//�õ���ǰ����
			if(sel>=0)
			{
				SendMessage(hList,LB_GETTEXT,(WPARAM)sel,(LPARAM)name);//�õ���ǰѡ���ַ���
				SetWindowText(hName,name);
			}
			else
			{
				SetWindowText(hName,"noname");
			}

			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID=LOWORD(wParam);
			int ControlNotify=HIWORD(wParam);
			HWND ControlHandle=(HWND)lParam;

			if(ControlID==IDC_SETNAME_OK)//ȷ��
			{
				char name[256],str[256];
				GetWindowText(hName,name,256);//�õ�����

				//д�������ļ�
				GetWindowText(hN,str,256);//��������
				WritePrivateProfileString(name,"N",str,buffer.c_str());
				GetWindowText(hXmax,str,256);//Xmax
				WritePrivateProfileString(name,"Xmax",str,buffer.c_str());
				GetWindowText(hYmax,str,256);//Ymax
				WritePrivateProfileString(name,"Ymax",str,buffer.c_str());
				GetWindowText(hX0,str,256);//X0
				WritePrivateProfileString(name,"X0",str,buffer.c_str());
				GetWindowText(hY0,str,256);//Y0
				WritePrivateProfileString(name,"Y0",str,buffer.c_str());

				int itemcounts=ListView_GetItemCount(hListView);//�õ���ǰ����
				sprintf(str,"%d",itemcounts);
				WritePrivateProfileString(name,"Counts",str,buffer.c_str());//д��ifs����

				int i,j;
				//д��IFS��
				for(i=0;i<itemcounts;i++)
				{
					string ifs_str;
					for(j=1;j<=8;j++)
					{
						ListView_GetItemText(hListView,i,j,str,256);
						ifs_str+=str;
						ifs_str+="|";//�ָ���
					}
					sprintf(str,"IFS_%d",i);
					WritePrivateProfileString(name,str,ifs_str.c_str(),buffer.c_str());
				}

				//ˢ���б��
				SendDlgItemMessage(hT4,IDC_T4_STOCKLIST,LB_RESETCONTENT,0,0);//����б��

				char inibuffer[1000];
				//�õ����н�����inibuffer,ÿ��������0����������2��0����
				if(GetPrivateProfileSectionNames(inibuffer,1000,buffer.c_str()))//�����ļ��ɹ�
				{
					char *p=inibuffer;
			
					while(*(p))
					{
						SendDlgItemMessage(hT4,IDC_T4_STOCKLIST,LB_ADDSTRING,0,(LPARAM)p);//���浽�б��
						p+=strlen(p)+1;//��������һ������
					
					}
				}

				EndDialog(hWnd,NULL);
			}
			else if(ControlID==IDC_SETNAME_CANCEL)//ȡ��
			{
			
				EndDialog(hWnd,NULL);
			}
	
			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			EndDialog(hWnd,NULL);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;

}
//T4����ֵ�Ի�����Ϣ����
BOOL WINAPI Dlg_T4_SetValue(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hListView=GetDlgItem(hT4,IDC_T4_LISTVIEW);
	HWND hA=GetDlgItem(hWnd,IDC_ST_T4_A);
	HWND hB=GetDlgItem(hWnd,IDC_ST_T4_B);
	HWND hC=GetDlgItem(hWnd,IDC_ST_T4_C);
	HWND hD=GetDlgItem(hWnd,IDC_ST_T4_D);
	HWND hE=GetDlgItem(hWnd,IDC_ST_T4_E);
	HWND hF=GetDlgItem(hWnd,IDC_ST_T4_F);
	HWND hP=GetDlgItem(hWnd,IDC_ST_T4_P);
	HWND hColor=GetDlgItem(hWnd,IDC_ST_T4_COLOR);
	char buffer[256];
	int curitem;
	
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			curitem=ListView_GetNextItem(hListView,-1,LVNI_FOCUSED);//�õ���ǰ��
			//�õ���ǰ���ı����г�ʼ���༭��
			ListView_GetItemText(hListView,curitem,1,buffer,256);
			SetWindowText(hA,buffer);
			ListView_GetItemText(hListView,curitem,2,buffer,256);
			SetWindowText(hB,buffer);
			ListView_GetItemText(hListView,curitem,3,buffer,256);
			SetWindowText(hC,buffer);
			ListView_GetItemText(hListView,curitem,4,buffer,256);
			SetWindowText(hD,buffer);
			ListView_GetItemText(hListView,curitem,5,buffer,256);
			SetWindowText(hE,buffer);
			ListView_GetItemText(hListView,curitem,6,buffer,256);
			SetWindowText(hF,buffer);
			ListView_GetItemText(hListView,curitem,7,buffer,256);
			SetWindowText(hP,buffer);

			// �õ���ɫ
			ListView_GetItemText(hListView,curitem,8,buffer,256);
			Edit_Color=atol(buffer);
			
			//���Ϳؼ���Ϣ������IDC_ST_T4_COLOR��ɫ
			PostMessage(hWnd,WM_CTLCOLOR,(WPARAM)0,(LPARAM)IDC_ST_T4_COLOR);

			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID=LOWORD(wParam);
			int ControlNotify=HIWORD(wParam);
			HWND ControlHandle=(HWND)lParam;
			if(ControlID==IDC_ST_T4_OK)//��Ϊȷ����ť�������ֵ
			{
				curitem=ListView_GetNextItem(hListView,-1,LVNI_FOCUSED);//�õ���ǰ��
				//���������ı�
				GetWindowText(hA,buffer,256);
				ListView_SetItemText(hListView,curitem,1,buffer);
				GetWindowText(hB,buffer,256);
				ListView_SetItemText(hListView,curitem,2,buffer);
				GetWindowText(hC,buffer,256);
				ListView_SetItemText(hListView,curitem,3,buffer);
				GetWindowText(hD,buffer,256);
				ListView_SetItemText(hListView,curitem,4,buffer);
				GetWindowText(hE,buffer,256);
				ListView_SetItemText(hListView,curitem,5,buffer);
				GetWindowText(hF,buffer,256);
				ListView_SetItemText(hListView,curitem,6,buffer);
				GetWindowText(hP,buffer,256);
				ListView_SetItemText(hListView,curitem,7,buffer);
				sprintf(buffer,"%ld",Edit_Color);
				ListView_SetItemText(hListView,curitem,8,buffer);
				
				EndDialog(hWnd,NULL);
			}
			else if(ControlID==IDC_ST_T4_CANCEL)//��Ϊȡ����ť
			{
				EndDialog(hWnd,NULL);
			}
			else if(ControlID==IDC_ST_T4_RESET)//��Ϊ���ð�ť
			{
				SetWindowText(hA,"0");
				SetWindowText(hB,"0");
				SetWindowText(hC,"0");
				SetWindowText(hD,"0");
				SetWindowText(hE,"0");
				SetWindowText(hF,"0");
				SetWindowText(hP,"0");
				
			}
			else if(ControlID==IDC_ST_T4_CHOOSECOLOR)//��Ϊ��ɫѡ��ť
			{
				//��ͨ����ɫ�Ի���
				COLORREF cusColor[16];

				CHOOSECOLOR lcc;
				lcc.lStructSize=sizeof(CHOOSECOLOR);
				lcc.hwndOwner=hWnd;
				lcc.hInstance=NULL;
				lcc.rgbResult=RGB(0,0,0);
				lcc.lpCustColors=cusColor; 
				lcc.Flags=CC_RGBINIT|CC_FULLOPEN| CC_ANYCOLOR;
				lcc.lCustData=NULL;
				lcc.lpfnHook=NULL;
				lcc.lpTemplateName=NULL;

				if(ChooseColor(&lcc))// �õ���ɫ
				{
					Edit_Color=lcc.rgbResult;
					//����ɫ��䵽��̬����ʾ
					HDC hdc=GetDC(hColor);
					HBRUSH hbrush=CreateSolidBrush(Edit_Color);
					RECT r;
					GetClientRect(hColor,&r);
					FillRect(hdc,&r,hbrush);
					ReleaseDC(hColor,hdc);
				
				}
			}

			return TRUE;
		}
		break;
	case WM_CTLCOLOR://�ؼ���Ϣ
		{
			
			int ControlID=LOWORD(lParam);
			if(ControlID==IDC_ST_T4_COLOR)
			{
				//����ɫ��䵽��̬����ʾ
				HDC hdc=GetDC(hColor);
				HBRUSH hbrush=CreateSolidBrush(Edit_Color);
				RECT r;
				GetClientRect(hColor,&r);
				FillRect(hdc,&r,hbrush);
				ReleaseDC(hColor,hdc);
			
			}

			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			EndDialog(hWnd,NULL);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}
//T1��湫ʽ�Ի�����Ϣ����
BOOL WINAPI DlgT1Stock(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			//��ȡ���п�湫ʽ���������й�ʽ��ӵ��б�
			if(GetAllGongShiFromDLL)
			{
				PARAMS param;
				param.attr=PARAMS_ATTR_T0;
				if(GetAllGongShiFromDLL((LPVOID)&param))//�õ���湫ʽ
				{
					vector<string> gongshi_array=param.gongshi_array;
					if(gongshi_array.size())//�����й�ʽ��ӵ��б�
					{
						for(int i=0;i<gongshi_array.size();i++)
						{
							SendDlgItemMessage(hWnd,IDC_LST_STOCK,\
								LB_ADDSTRING,0,(LPARAM)gongshi_array[i].c_str());
						
						}
					
					}
				}
			}
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID=LOWORD(wParam);
			int ControlNotify=HIWORD(wParam);
			HWND ControlHandle=(HWND)lParam;
			if(ControlID==IDC_LST_STOCK)
			{
				if(ControlNotify==LBN_DBLCLK)//��Ϊ˫��
				{
					char gongshi[260];
					int sel=SendMessage(ControlHandle,LB_GETCURSEL,0,0);//�õ���ǰ����
					SendMessage(ControlHandle,LB_GETTEXT,(WPARAM)sel,(LPARAM)gongshi);//�õ���ʽ
			
					HWND hEdit=GetDlgItem(hT0,IDC_T0_GONGSHI_EDT);
					SetWindowText(hEdit,gongshi);//����ʽ��ر༭��

					EndDialog(hWnd,NULL);//�رնԻ���
				}
			
			}
	
			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			EndDialog(hWnd,NULL);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}

//T0�Ի����ͼ�߳�
DWORD WINAPI OnDrawT0(LPVOID lpParam)
{
	RECT r;
	GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

	HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC

	//��������
	HBRUSH hbrush=CreateSolidBrush(BackGroundColor);
	//FillRect(hDC,&r,hbrush);//������ɫ
	//�õ�����
	//========================================================
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

	//��ʼ���㲢����(���ڴ��л���)
	//========================================================
	long double width=r.right-r.left;
	long double height=r.bottom-r.top;
	unsigned long n;
	long double i,j;
	int R,G,B;
	long double dx,dy;

	//===========================================================
	//�Ŵ��㷨
	//===================================================
	RECT myrect={0,0,width,height};//ȱʡ�Ŵ��������
	if(IsFangSuo)
	{
		myrect=MyRect;//�õ��Ŵ��������
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

	char gongshi[260];
	vector<TOKEN<FuShu> > arrays;
	vector<TOKEN<FuShu> > val(2);
	val[0].str="z";//����z
	val[1].str="c";//����c

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

	FuShu z;
	FuShu z1,t1,t2;
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

		val[1].num.Get(Real,Imag);
	
	}

	//�õ���ʽ
	HWND hEdit=GetDlgItem(hT0,IDC_T0_GONGSHI_EDT);
	GetWindowText(hEdit,gongshi,260);

	if(!GongShi_Change(gongshi,val,arrays))//��ʽת��
	{
		MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
		goto end;
	}

	//��ͼ
	//========================================
	//��ʧ��Ŵ�
	StretchBlt(hDC,0,0,width,height,hMemDC,myrect.left,myrect.top,\
		myrect.right-myrect.left,myrect.bottom-myrect.top,SRCCOPY);

	FillRect(hMemDC,&r,hbrush);//������ɫ
	//=========================================
	dx=(fx2-fx1)/width;
	dy=(fy2-fy1)/height;
	
	for(i=0;i<height;i++)
	{
		for(j=0;j<width;j++)
		{
			if(select==FRACTAL_T0_MANDELBROT)//Mandelbrot
			{
				val[0].num.Get(0);//z=(0,0)
				val[1].num.Get(fx1+j*dx,fy1+i*dy);//c=(fx1+j*dx,fy1+i*dy)
			}
			else if(select==FRACTAL_T0_JULIA)//Julia
			{
				val[0].num.Get(fx1+j*dx,fy1+i*dy);//z=(fx1+j*dx,fy1+i*dy)
				//c=(Real,Imag)
			}
			else if(select==FRACTAL_T0_NEWTON)//Newton
			{
				z1.Get(fx1+j*dx,fy1+i*dy);//z1=(fx1+j*dx,fy1+i*dy)
			}
			else goto end;

			n=0;
			while (1)
			{
				if(select==FRACTAL_T0_MANDELBROT||select==FRACTAL_T0_JULIA)
				{
					if(!GongShi_Compute(arrays,val,val[0].num))//���㹫ʽ
					{
						MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
						goto end;
		
					}
					if((val[0].num.m2())>=Rmax)
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
					val[0].num=z1;
					if(!GongShi_Compute(arrays,val,t1))//����ԭ��ʽ
					{
						MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
						goto end;
		
					}
					FuShu h(0.0000001,0.0000001);
					val[0].num=z1+h;
					if(!GongShi_Compute(arrays,val,t2))//���㹫ʽ(������)
					{
						MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
						goto end;
		
					}
					z=z1-(t1*h)/(t2-t1);//Newtonʸ��
					z1=z-z1;//�������ϴ�ֵ�ľ���
					if(z1.m2()>Newton_Rmax)//������0
					{
						R = n*Ar+Br;
						G = n*Ag+Bg;
						B = n*Ab+Bb;
						if ((R & 0x1FF) > 0xFF) R = R ^ 0xFF;
						if ((G & 0x1FF) > 0xFF) G = G ^ 0xFF;
						if ((B & 0x1FF) > 0xFF) B = B ^ 0xFF;
						SetPixel(hMemDC,(int)(j),(int)(i),RGB(R,G,B));
						
					}
					if(n++>=Nmax)//������
					{
						break;
					}
					z1=z;//ʸ��
				}
			}
			
			
		}
	BitBlt(hDC,r.left,r.top,r.right-r.left,i,hMemDC,0,0,SRCCOPY);//������
	}
	//========================================================

	//��ʾͼ��
	//BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);
end:
	ReleaseDC(MAIN_HANDLE,hDC);

	if(!IsFangSuo)//���ڷ���״̬�£�������
	{
		//���û�ͼ��ť
		HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
		EnableWindow(hDraw,TRUE);
		//���ò�����ť
		HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
		EnableWindow(hClear,TRUE);
	}
	else
	{
		//�Ŵ�������������
		MyRect.left=0;
		MyRect.top=0;
		MyRect.right=0;
		MyRect.bottom=0;
	
	}

	//ȡ����ʾ�Ŵ����
	SetWindowText(MAIN_DIALOG,"���λ���1.0");
	
	return 1;
}
//T1�Ի����ͼ�߳�
DWORD WINAPI OnDrawT1(LPVOID lpParam)
{
	RECT r;
	GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

	HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC

	//��������
	HBRUSH hbrush=CreateSolidBrush(BackGroundColor);
	FillRect(hDC,&r,hbrush);//������ɫ

	//��ʼ���㲢����(���ڴ��л���)
	//========================================================
	int width=r.right-r.left;
	int height=r.bottom-r.top;


	//========================================================

	//��ʾͼ��
	//BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);

	ReleaseDC(MAIN_HANDLE,hDC);

	//���û�ͼ��ť
	HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
	EnableWindow(hDraw,TRUE);
	//���ò�����ť
	HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
	EnableWindow(hClear,TRUE);
	
	return 1;
}
//T2�Ի����ͼ�߳�
DWORD WINAPI OnDrawT2(LPVOID lpParam)
{
	RECT r;
	GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

	HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC

	//��������
	HBRUSH hbrush=CreateSolidBrush(BackGroundColor);
	FillRect(hDC,&r,hbrush);//������ɫ
	FillRect(hMemDC,&r,hbrush);//������ɫ


	//��ʼ���㲢����(���ڴ��л���)
	//========================================================
	double width=r.right-r.left;
	double height=r.bottom-r.top;
	double scrsize2=sqrt(width*width+height*height);//�õ�2ά��Ļ�ߴ�
	double x0=width/2;
	double y0=height/2;
	int i;
	string bufrules;
	//�õ�����
	//=======================================================
	char len[20],*st;
	//�õ���λ����
	HWND hLen=GetDlgItem(hT2,IDC_T2_LEN);
	GetWindowText(hLen,len,20);
	double lenght=strtod(len,&st);

	//�õ���ʼ����
	double X_0,Y_0,Z_0;
	char x_0[20],y_0[20],z_0[20];
	HWND hX0=GetDlgItem(hT2,IDC_T2_X0);
	GetWindowText(hX0,x_0,20);
	X_0=strtod(x_0,&st);
	HWND hY0=GetDlgItem(hT2,IDC_T2_Y0);
	GetWindowText(hY0,y_0,20);
	Y_0=strtod(y_0,&st);
	HWND hZ0=GetDlgItem(hT2,IDC_T2_Z0);
	GetWindowText(hZ0,z_0,20);
	Z_0=strtod(z_0,&st);

	int weishu;//�õ�ά��
	if(IsDlgButtonChecked(hT2,IDC_RBN_T2_2)&&!IsDlgButtonChecked(hT2,IDC_RBN_T2_3))
		weishu=2;
	else if(!IsDlgButtonChecked(hT2,IDC_RBN_T2_2)&&IsDlgButtonChecked(hT2,IDC_RBN_T2_3))
		weishu=3;
	else weishu=2;

	long n;//�õ���������
	char nstr[20];
	HWND hN=GetDlgItem(hT2,IDC_T2_N);
	GetWindowText(hN,nstr,20);
	n=atol(nstr);

	//�õ���ת�Ƕ�
	double angle;
	char anglestr[20];
	HWND hAngle=GetDlgItem(hT2,IDC_T2_DEGREE);
	GetWindowText(hAngle,anglestr,20);
	angle=strtod(anglestr,&st);
	
	//==========================

	char wenfa[10000];//�õ��ķ��ı�
	HWND hWenfa=GetDlgItem(hT2,IDC_T2_WENFA_EDIT);
	GetWindowText(hWenfa,wenfa,10000);

	vector<LS> Rules;//תΪ��������(�ֽ����)
	if(wenfastr.compare(wenfa)!=0||ls_count!=n)//�ж��ķ��Ƿ�ı�
	{
		wenfastr.assign(wenfa);
		ls_count=n;
		
		string str;
		char *p=wenfa;

		//�õ���ʼ����
		string initrule;
		int init_index=wenfastr.find_first_of("\r\n");
		initrule.assign(wenfa,init_index);
		p+=init_index+2;
		rules_str=initrule;

		if(rules_str.size()==0||rules_str.find_first_of('=')!=-1)
		{
			MessageBox(MAIN_HANDLE,"�������ʼ����!","����",MB_OK);
			goto end;
		}
	 
	
		if(wenfastr.find_first_of('@')==-1)
		{
			MessageBox(MAIN_HANDLE,"�������ķ�������:'@'!","����",MB_OK);
			goto end;

		}
		while(*p)
		{
			if(*p=='\r'||*p=='#')
			{
				//�ֽ����
				int index=str.find_first_of('=');
				if(index!=-1)
				{
					LS rule;
					if(index!=1)
					{
						rule.head.assign(str,0,index);
						char errstr[100];
						sprintf(errstr,"����Ԫ'%s'�����ַ�����!",rule.head.c_str());
						MessageBox(MAIN_HANDLE,errstr,"����",MB_OK);
						goto end;
					}
				
					rule.head.assign(str,0,1);
					rule.body.assign(str,2,str.length()-2);
					Rules.push_back(rule);
					if(rule.head.size()==0)
					{
						MessageBox(MAIN_HANDLE,"LS�﷨����!","����",MB_OK);
						goto end;
					}
				}
				str.resize(0);
				//����ע�Ͳ���
				if(*p=='#')
				{
					while(*(p+1))
					{
						p++;
						if(*p=='\n')
						{
							break;
						}
				
					}
				}
			}
			else if(!isspace(*p))//���˵��հ��ַ�
			{
				str+=*p;
			}
			if(*p=='@')break;//�ļ�������
		
			p++;
		}
	//for(i=0;i<Rules.size();i++)
		//DebugBox("head:%s,body:%s",Rules[i].head.c_str(),Rules[i].body.c_str());
	//========================================================
		//�������
		while(n-->0)
		{
			bufrules=rules_str;
			char *p=(char *)bufrules.c_str();
			rules_str.resize(0);
			while(*p)
			{
				string s;
				s.assign(p,1);
				for(i=0;i<Rules.size();i++)
				{
					if(Rules[i].head.compare(s)==0)
					{
						rules_str+=Rules[i].body;
						break;
					}
			
			
				}
				if(i==Rules.size())
				{
					rules_str+=s;
				}
				p++;
		
			}
		}
		if(rules_str.compare(initrule)==0)
		{
			MessageBox(MAIN_HANDLE,"��ƥ������Ԫ!","����",MB_OK);
			goto end;
		}
		//DebugBox("%s",rules_str.c_str());
	}
	//========================================================
	
	//������ͼ
	if(weishu==2)//����ά�ķ�ͼ
	{
		LSDOT CurDot;
		stack<LSDOT> DotStack;

		CurDot.x=X_0;//���ó�ʼ����
		CurDot.y=Y_0;
		CurDot.xdeg=0;

		char *p=(char *)rules_str.c_str();

		HPEN hPen=CreatePen(PS_SOLID,1,RGB(255,255,255));
		SelectObject(hMemDC,hPen);

		MoveToEx(hMemDC,x0+CurDot.x,y0-CurDot.y,NULL);

		while(*p)
		{
			if(*p=='F')//��ǰ��һ��������(1����λ����)
			{
				CurDot.x+=lenght*cos(CurDot.xdeg*PI/180);
				CurDot.y+=lenght*sin(CurDot.xdeg*PI/180);
				LineTo(hMemDC,x0+CurDot.x,y0-CurDot.y);
			}
			else if(*p=='L')
			{
			}
			else if(*p=='R')
			{
			}
			else if(*p=='+')//��ת
			{
				string bufstr(p);
				int begin_index=bufstr.find_first_of('(');
				int end_index=bufstr.find_first_of(')');

				if(begin_index==1&&end_index>begin_index)//��ת����Ƕ�
				{
					//ȡ�����е�ֵ
					bufstr.assign(p,begin_index+1,end_index-begin_index-1);
					double ang=strtod(bufstr.c_str(),&st);
					CurDot.xdeg+=ang;

					p+=end_index-begin_index+2;
					continue;
				}
				else//��תĬ�ϽǶ�
				{
					CurDot.xdeg+=angle;
				}
			
			}
			else if(*p=='-')//��ת
			{
				string bufstr(p);
				int begin_index=bufstr.find_first_of('(');
				int end_index=bufstr.find_first_of(')');

				if(begin_index==1&&end_index>begin_index)//��ת����Ƕ�
				{
					bufstr.assign(p,begin_index+1,end_index-begin_index-1);
					double ang=strtod(bufstr.c_str(),&st);
					CurDot.xdeg-=ang;

					p+=end_index-begin_index+2;
					continue;
				}
				else//��תĬ�ϽǶ�
				{
					CurDot.xdeg-=angle;
				}
			}
			else if(*p=='[')//ѹջ
			{
				DotStack.push(CurDot);
			
			}
			else if(*p==']')//��ջ
			{
				if(DotStack.size()>0)
				{
					CurDot=DotStack.top();
					DotStack.pop();
					MoveToEx(hMemDC,x0+CurDot.x,y0-CurDot.y,NULL);
				}
			
			}
			else ;
			p++;
		}
	
	
	}
	else if(weishu==3)//����ά�ķ�ͼ
	{

	
	}

	//��ʾͼ��
	BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);
end:
	ReleaseDC(MAIN_HANDLE,hDC);

	//���û�ͼ��ť
	HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
	EnableWindow(hDraw,TRUE);
	//���ò�����ť
	HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
	EnableWindow(hClear,TRUE);
	
	return 1;
}
//T3�Ի����ͼ�߳�
DWORD WINAPI OnDrawT3(LPVOID lpParam)
{
	RECT r;
	GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

	HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC
	//��������
	HBRUSH hbrush=CreateSolidBrush(BackGroundColor);
	FillRect(hMemDC,&r,hbrush);//������ɫ

	//�õ�����ֵ
	//==================================
	double Xmax=2*(double)GetDlgItemInt(hT3,IDC_T3_XMAX,NULL,FALSE);//x�����
	double Ymax=2*(double)GetDlgItemInt(hT3,IDC_T3_YMAX,NULL,FALSE);//y�����
	double Zmax=2*(double)GetDlgItemInt(hT3,IDC_T3_ZMAX,NULL,FALSE);//z�����

	
	int P_Range=(int)GetDlgItemInt(hT3,IDC_EDT_T3_PMAX,NULL,FALSE);//����P��Χ
	int Q_Range=(int)GetDlgItemInt(hT3,IDC_EDT_T3_QMAX,NULL,FALSE);//����P��Χ

	int select;
	if(IsDlgButtonChecked(hT3,IDC_T3_RBN1)&&!IsDlgButtonChecked(hT3,IDC_T3_RBN2))
		select=1;
	else if(!IsDlgButtonChecked(hT3,IDC_T3_RBN1)&&IsDlgButtonChecked(hT3,IDC_T3_RBN2))
		select=2;
	else select=0;

	//========================================================
	double width=(double)r.right-r.left;
	double height=(double)r.bottom-r.top;
	double xr=(Xmax)/width;
	double yr=(Ymax)/height;
	double x0=(double)r.left+width/2;
	double y0=(double)r.top+height/2;
	double i,j;

	//������
	for(i=0;i<height;i++)
		SetPixel(hMemDC,x0,int(i),RGB(255,255,255));
	
	for(j=0;j<width;j++)
		SetPixel(hMemDC,int(j),y0,RGB(255,255,255));

	//��ʾ����
	BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);
	
	//��ʼ���㲢����(���ڴ��л���)
	//========================================================
	if(select==1)//���ʽ��ͼ
	{
		char gongshi[260];
		vector<TOKEN<double> > arrays;
		vector<TOKEN<double> > val(2);
		val[0].str="x";//����x
		val[1].str="y";//����y

		HWND hEdit=GetDlgItem(hT3,IDC_T3_GONGSHI);
		GetWindowText(hEdit,gongshi,260);//�õ���ʽ

		double result=0;
		string str=gongshi;
		if(gongshi[0]=='y'&&gongshi[1]=='='&&str.find_first_of('x',1)!=-1&&str.find_first_of('y',1)==-1)
		{
			if(!GongShi_Change(gongshi,val,arrays))//��ʽת��
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			}
			for(j=-width/2;j<width/2;j+=0.05)
			{	
				val[0].num=j*xr;//x���������

				if(!GongShi_Compute(arrays,val,result))//���㹫ʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
		
				}
		
				SetPixel(hMemDC,int(x0+j),int(y0-result/yr),T3_Color);
				
			}
		}
		else if(gongshi[0]=='x'&&gongshi[1]=='='&&str.find_first_of('y',1)!=-1&&str.find_first_of('x',1)==-1)
		{
			if(!GongShi_Change(gongshi,val,arrays))//��ʽת��
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			}
			for(i=-height/2;i<height/2;i+=0.05)
			{	
				val[1].num=i*yr;//x���������

				if(!GongShi_Compute(arrays,val,result))//���㹫ʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
		
				}
				SetPixel(hMemDC,int(x0+result/xr),int(y0+i),T3_Color);
				
			}
	
		}
		else//��=�ڹ�ʽ�м䣬��ת����ʽ������Newtonʸ������ֵ����
		{
			string gstr(gongshi);
			string s;
			
			int index=gstr.find_first_of('=');
			if(index>0)
			{
				gstr.at(index)='-';//ת����ʽ����:x+y=1 ==> x+y-1
				gstr.insert(index+1,"(");
				gstr+=")";
				if(!GongShi_Change((char *)gstr.c_str(),val,arrays))//��ʽת��,��׺���ʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
				}

				double dd=height/Ymax;//�����ƽ�ֵ֮��ļ����(����y�᷶Χ���е���)
				double yd,y1,preyd;
				long n;
				
				//����x����yֵ
				for(j=-width/2;j<width/2;j+=0.05)
				{	
					val[0].num=j*xr;//x���������
					for(i=-height/2;i<height/2;i+=dd)//���������Աƽ�ֵ(����x��Ӧy���ж��ֵ�����)
					{
						y1=i*yr;//y��ƽ�ֵy1
						n=0;
						preyd=height*yr;
						while(1)//Newtonʸ����yֵ
						{
							val[1].num=y1;
							
							if(!GongShi_Compute(arrays,val,result))//���㹫ʽ
							{
								MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
								goto end;
		
							}
							val[1].num=y1+0.00001;
							if(!GongShi_Compute(arrays,val,yd))//���㹫ʽ
							{
								MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
								goto end;
		
							}
							result=y1-(result*0.00001)/(yd-result);//�ý��

							if(fabs(preyd)<=fabs(result-y1))//���޽�(���ϴεĲ�ֵС����εĲ�ֵ������������)
							{
								result=-1;
								break;
							}

							if(fabs(result-y1)<0.005)//���ƽ���㣬����ý�
							{
								result=y0-result/yr;//����ʵ����Ļ����
								break;
						
							}
							preyd=result-y1;//�����ϴεĲ�ֵ

							y1=result;
							
						}
						if(result>=0&&result<height)
						{
							SetPixel(hDC,int(x0+j),int(result),T3_Color);
							SetPixel(hMemDC,int(x0+j),int(result),T3_Color);
						}
						
					}
					SetPixel(hDC,int(x0+j),0,RGB(255,255,0));//��ʾ����
				}
			
			}
			else
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			
			}

		
		}
	}
	else if(select==2)//������ʽ��ͼ
	{
		
		//��Ϊ2ά
		if(IsDlgButtonChecked(hT3,IDC_RBN_T3_2)&&!IsDlgButtonChecked(hT3,IDC_RBN_T3_3))
		{
			char gongshiX[260];
			char gongshiY[260];
			vector<TOKEN<double> > arraysX;
			vector<TOKEN<double> > arraysY;
			vector<TOKEN<double> > val(1);
			val[0].str="p";//����p
		
			HWND hEditX=GetDlgItem(hT3,IDC_T3_EDIT_X);
			HWND hEditY=GetDlgItem(hT3,IDC_T3_EDIT_Y);
			GetWindowText(hEditX,gongshiX,260);//�õ�x�ṫʽ
			GetWindowText(hEditY,gongshiY,260);//�õ�y�ṫʽ

			if(!GongShi_Change(gongshiX,val,arraysX))//��ʽת��
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			}
			if(!GongShi_Change(gongshiY,val,arraysY))//��ʽת��
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			}
			double resultX=0;
			double resultY=0;

			for(j=-P_Range;j<P_Range;j+=0.005)
			{	
				val[0].num=j;

				if(!GongShi_Compute(arraysX,val,resultX))//����x�ṫʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
		
				}
				if(!GongShi_Compute(arraysY,val,resultY))//����y�ṫʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
		
				}
				//����Ч�����ڻ���
				if(resultX/xr>(-width/2)&&resultX<(width/2)&&resultY/yr>(-height/2)&&resultY<(height/2))
				SetPixel(hMemDC,int(x0+resultX/xr),int(y0-resultY/yr),T3_Color);
			
			}
		}
		else if(!IsDlgButtonChecked(hT3,IDC_RBN_T3_2)&&IsDlgButtonChecked(hT3,IDC_RBN_T3_3))//��Ϊ3ά
		{
			char gongshiX[260];
			char gongshiY[260];
			char gongshiZ[260];
			vector<TOKEN<double> > arraysX;
			vector<TOKEN<double> > arraysY;
			vector<TOKEN<double> > arraysZ;
			vector<TOKEN<double> > val(2);
			val[0].str="p";//����p
			val[1].str="q";//����q
		
			HWND hEditX=GetDlgItem(hT3,IDC_T3_EDIT_X);
			HWND hEditY=GetDlgItem(hT3,IDC_T3_EDIT_Y);
			HWND hEditZ=GetDlgItem(hT3,IDC_T3_EDIT_Z);
			GetWindowText(hEditX,gongshiX,260);//�õ�x�ṫʽ
			GetWindowText(hEditY,gongshiY,260);//�õ�y�ṫʽ
			GetWindowText(hEditZ,gongshiZ,260);//�õ�z�ṫʽ

			//��y������
			for(i=-width/2;i<width/2;i++)
				SetPixel(hMemDC,int(x0+i),int(y0-i),RGB(255,255,255));
			//��ʾ����
			BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);

			
			if(!GongShi_Change(gongshiX,val,arraysX))//��ʽת��
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			}
			if(!GongShi_Change(gongshiY,val,arraysY))//��ʽת��
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			}
			if(!GongShi_Change(gongshiZ,val,arraysZ))//��ʽת��
			{
				MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
				goto end;
			}
			double resultX=0;
			double resultY=0;
			double resultZ=0;
			double xx,yy;

			for(i=-P_Range;i<P_Range;i+=0.025)
			for(j=-Q_Range;j<Q_Range;j+=0.025)
			{	
				val[0].num=i;
				val[1].num=j;
				
				if(!GongShi_Compute(arraysX,val,resultX))//����x�ṫʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
		
				}
				if(!GongShi_Compute(arraysY,val,resultY))//����y�ṫʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
		
				}
				if(!GongShi_Compute(arraysZ,val,resultZ))//����z�ṫʽ
				{
					MessageBox(MAIN_HANDLE,"��Ч��ʽ!","����",MB_OK);
					goto end;
		
				}
		
				resultX=resultX+0.7071067811865*resultY;
				resultY=resultZ+0.7071067811865*resultY;
				xx=resultX/xr;
				yy=resultY/yr;
				//����Ч�����ڻ���
			
				if(xx>(-width/2)&&resultX<(width/2)&&yy>(-height/2)&&resultY<(height/2))
				{
					SetPixel(hMemDC,int(x0+xx),int(y0-yy),T3_Color);
					SetPixel(hDC,int(x0+xx),int(y0-yy),T3_Color);
				}
				
			
			}
			
		}
		else ;
	
	
	}
	else MessageBox(MAIN_HANDLE,"��ѡ��ʽ���͹�ʽ!","����",MB_OK);
	//========================================================
end:
	//��ʾͼ��
	BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);

	ReleaseDC(MAIN_HANDLE,hDC);

	//���û�ͼ��ť
	HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
	EnableWindow(hDraw,TRUE);
	//���ò�����ť
	HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
	EnableWindow(hClear,TRUE);
	
	return 1;
}
//T4�Ի����ͼ�߳�
DWORD WINAPI OnDrawT4(LPVOID lpParam)
{
	RECT r;
	GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

	HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC

	//��������
	HBRUSH hbrush=CreateSolidBrush(BackGroundColor);
	FillRect(hDC,&r,hbrush);//������ɫ
	FillRect(hMemDC,&r,hbrush);//������ɫ

	//��ʼ���㲢����(���ڴ��л���)
	//========================================================
	double width=r.right-r.left;
	double height=r.bottom-r.top;
	double x0=width/2;
	double y0=height/2;
	double xr,yr;
	//========================================================
	//�õ�����
	vector<IFS > ifs_array;
	int i;

	HWND hListView=GetDlgItem(hT4,IDC_T4_LISTVIEW);
	int itemcounts=ListView_GetItemCount(hListView);//�õ���ǰ����

	ifs_array.resize(itemcounts);
	char buffer[256];
	char *st;
	double ps=0;
	for(i=0;i<itemcounts;i++)
	{

			//�õ���ǰ���ı����г�ʼ���༭��
			ListView_GetItemText(hListView,i,1,buffer,256);
			ifs_array[i].a=strtod(buffer,&st);
			ListView_GetItemText(hListView,i,2,buffer,256);
			ifs_array[i].b=strtod(buffer,&st);
			ListView_GetItemText(hListView,i,3,buffer,256);
			ifs_array[i].c=strtod(buffer,&st);
			ListView_GetItemText(hListView,i,4,buffer,256);
			ifs_array[i].d=strtod(buffer,&st);
			ListView_GetItemText(hListView,i,5,buffer,256);
			ifs_array[i].e=strtod(buffer,&st);
			ListView_GetItemText(hListView,i,6,buffer,256);
			ifs_array[i].f=strtod(buffer,&st);
			ListView_GetItemText(hListView,i,7,buffer,256);
			ifs_array[i].p=strtod(buffer,&st);

			ifs_array[i].psum=ps;//������ʿռ���ʼֵ
			ps+=ifs_array[i].p;

			// �õ���ɫ
			ListView_GetItemText(hListView,i,8,buffer,256);
			ifs_array[i].color=atol(buffer);
			
	}
	//�õ���������
	HWND hN=GetDlgItem(hT4,IDC_T4_N);
	GetWindowText(hN,buffer,256);
	long n=atol(buffer);
	//�õ�x,y�����ֵ
	HWND hXmax=GetDlgItem(hT4,IDC_T4_XMAX);
	GetWindowText(hXmax,buffer,256);
	double xmax=strtod(buffer,&st);
	HWND hYmax=GetDlgItem(hT4,IDC_T4_YMAX);
	GetWindowText(hYmax,buffer,256);
	double ymax=strtod(buffer,&st);

	//�������ű���
	xr=xmax/width;
	yr=ymax/height;

	HWND hX0=GetDlgItem(hT4,IDC_T4_X0);//�õ�����ƫ��
	GetWindowText(hX0,buffer,256);
	double xd=strtod(buffer,&st);
	HWND hY0=GetDlgItem(hT4,IDC_T4_Y0);
	GetWindowText(hY0,buffer,256);
	double yd=strtod(buffer,&st);

	//========================================================
	//��ͼ
	double pr;//�����
	double x=0,y=0,newx,newy;
	srand(time(0));
	while((n--)>0)
	{
		pr=(double)rand()/RAND_MAX;//����0-1�������
		
		for(i=0;i<ifs_array.size();i++)
		{	
			//���ڸø��ʿռ��ڣ�����Ƹ�ifs�任
			if(pr>=ifs_array[i].psum&&pr<(ifs_array[i].psum+ifs_array[i].p))
			{
				newx=ifs_array[i].a*x+ifs_array[i].b*y+ifs_array[i].e;
				newy=ifs_array[i].c*x+ifs_array[i].d*y+ifs_array[i].f;

				x=newx;
				y=newy;
				
				if((int)(x0+(xd+x)/xr)>=0&&(int)(x0+(xd+x)/xr)<=width&&\
					(int)(y0-(yd+y)/yr)>=0&&(int)(y0-(yd+y)/yr)<=height)
				{
					SetPixel(hMemDC,(int)(x0+(xd+x)/xr),(int)(y0-(yd+y)/yr),ifs_array[i].color);
				}

				break;
			
			}
		
		}
	}
	//========================================================

	//��ʾͼ��
	BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);

	ReleaseDC(MAIN_HANDLE,hDC);

	//���û�ͼ��ť
	HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
	EnableWindow(hDraw,TRUE);
	//���ò�����ť
	HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
	EnableWindow(hClear,TRUE);
	
	return 1;
}


typedef struct DOT_COLOR_TAG
{
	int color;//��ɫ
	int grade;//�Ҷ�ֵ

}DOT_COLOR;

//T10�Ի����ͼ�߳�
DWORD WINAPI OnDrawT10(LPVOID lpParam)
{
	RECT r;
	GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

	HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC

	//��������
	HBRUSH hbrush=CreateSolidBrush(BackGroundColor);
	FillRect(hDC,&r,hbrush);//������ɫ

	//��ʼ���㲢����(���ڴ��л���)
	//========================================================
	double width=r.right-r.left;
	double height=r.bottom-r.top;
	double x,y,z,w,b=0.999,a=0;
	int i,j,n=0,color=0;
	
	//========================================================
	//�õ�����


	//========================================================
	//DebugTestException();
	//��ʼ��ͼ
	vector<vector<DOT_COLOR> > dots;//���ڱ�����Ļ�ѻ������ص�
	vector<vector<COLORREF> > ColorArray;//ɫ������
	int R,G,B;

	ColorArray.resize(192);
	//��ɫ��192����ɫ����
	for(i=0;i<192;i++)
	{
		if(i<32)
		{
			R=255;
			G=i*8;
			B=0;
		}
		else if(i<64&&i>=32)
		{
			R=255-(i-32)*8;
			G=255;
			B=0;
		
		}
		else if(i<96&&i>=64)
		{
			R=0;
			G=255;
			B=(i-64)*8;
		
		}
		else if(i<128&&i>=96)
		{
			R=0;
			G=255-(i-96)*8;
			B=255;
		
		}
		else if(i<160&&i>=128)
		{
			R=(i-128)*8;
			G=0;
			B=255;
		
		}
		else if(i<192&&i>=160)
		{
			R=255;
			G=0;
			B=255-(i-160)*8;
		
		}
		ColorArray[i].resize(128);
		ColorArray[i][0]=RGB(R,G,B);
		for(j=0;j<128;j++)
		{
			ColorArray[i][127-j]=RGB((((0-R)*j*2)>>8)+R,(((0-G)*j*2)>>8)+G,(((0-B)*j*2)>>8)+B);
			
		}
		//���Ե�ɫ��
		//for(j=0;j<128;j++)
		//SetPixel(hDC,i,100+j,ColorArray[i][j]);
	
	}
	//��ʼ����Ļɫ�ʵ�
	dots.resize(height);
	for(i=0;i<dots.size();i++)
	{
		dots[i].resize(width);
		for(j=0;j<dots[i].size();j++)
		{
			dots[i][j].color=-1;
			dots[i][j].grade=127;
		}
	}
	color=0;
	while(1)
	{
	if(a>=1) 
	{
		a=0;
		b=0.5+(double)rand()/(RAND_MAX);
		//b=1.011;
		
	}
			
	/*if(!(int(a*10000)%100))
	{
		b-=0.005;
		if(b<=0)b=0.999;
	}*/
	
	
	a+=0.0005;
	
	x=0.004;
	y=0.004;
	w=1;
	//================
	//ʵ��ɫ�ʵ�����ʧ��Ч��������������ĵ�
	int size=dots.size();
	for(i=0;i<height;i++)
		for(j=0;j<width;j++)
		{
			if(dots[i][j].color>=0)
			{
				if(--dots[i][j].grade>=0)//�ݼ�����
				{
					SetPixel(hMemDC,j,i,ColorArray[dots[i][j].color][dots[i][j].grade]);

				}
				else //ֱ����ʧ
				{
					dots[i][j].color=-1;
					dots[i][j].grade=127;
				
				}
			}
		}

	//================
	color++;//ɫ��ѭ��
	if(color==ColorArray.size())color=0;

	for(n=0;n<100;n++)//��������
	{
		z=x;
		x=b*y+w;
		//w=a*x+(2*(1-a)*x*x)/(1+x*x);
		w=-0.05*a*x+a*(PI-a*x)*x*x/(1+x*x);
		//w=SIGN(x)*(a*x+2*(1-a)*x*x/(1+x*x))+sin(a+x);
		//w=2*PI*a*5*SIGN(x)+PI+cos(x)+sin(PI+sin(PI+x));
		
		y=w-z;

		//������Ļ�����
		int hh=(int)(height/2-y*10);
		int ww=(int)(width/2+x*10);

		if(ww>0&&ww<width&&hh>0&&hh<height)
		{
			//����õ�
			dots[hh][ww].color=color;
			dots[hh][ww].grade=127;
			
		}
	
	}
	BitBlt(hDC,r.left,r.top,(int)width,(int)height,hMemDC,0,0,SRCCOPY);
	}
	//========================================================
	

	//��ʾͼ��
	//BitBlt(hDC,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);

	ReleaseDC(MAIN_HANDLE,hDC);

	//���û�ͼ��ť
	HWND hDraw=GetDlgItem(MAIN_DIALOG,IDC_DRAW);
	EnableWindow(hDraw,TRUE);
	//���ò�����ť
	HWND hClear=GetDlgItem(MAIN_DIALOG,IDC_CLEAR);
	EnableWindow(hClear,TRUE);
	
	return 1;
}
WNDPROC OldAboutStaticMsgProc=NULL;
//T3�Ի�����ɫ��̬�ؼ�����Ϣ������
LRESULT MyAboutStaticMsgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT p;
			HDC hDC=BeginPaint(hWnd,&p);
			//���ؼ�λͼ
			HBITMAP hbitmap=LoadBitmap(MAIN_INSTANCE,MAKEINTRESOURCE(IDB_APP_BITMAP));
			HBRUSH hbrush=CreatePatternBrush(hbitmap);
			/*HDC hmemdc=CreateCompatibleDC(hDC);

			RECT cr;
			GetClientRect(hWnd,&cr);

			HBITMAP bm;//�����ڴ����λͼ,����ֻ���ǵ�ɫ����DC
			bm=CreateCompatibleBitmap(hDC,cr.right-cr.left,cr.bottom-cr.top);
			SelectObject(hmemdc,bm);

			FillRect(hmemdc,&cr,hbrush);
			BitBlt(hDC,0,0,cr.right-cr.left,cr.bottom-cr.top,hmemdc,0,0,SRCPAINT);*/
			RECT cr;
			GetClientRect(hWnd,&cr);
			FillRect(hDC,&cr,hbrush);

			EndPaint(hWnd,&p);
			
			return TRUE;
		}
		break;

	}
	//���ÿؼ�ԭ������Ϣ������
	return CallWindowProc(OldT3ColorMsgProc,hWnd,uMsg,wParam,lParam);
}
//���ڶԻ�����Ϣ����
BOOL WINAPI DlgAboutProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
		{
			HICON hicon=LoadIcon(MAIN_INSTANCE,MAKEINTRESOURCE(IDI_APP));
			//����ͼ�꣬���ַ���
			//SetClassLong(hWnd,GCL_HICON,(long)hicon);
			SendMessage(hWnd,WM_SETICON,ICON_BIG,(LPARAM)hicon);

			HWND hStatic=GetDlgItem(hWnd,IDC_APP_ICON);
			//Ƕ���µľ�̬�ؼ���Ϣ������
			OldAboutStaticMsgProc=(WNDPROC)SetWindowLong(hStatic,GWL_WNDPROC,(LONG)MyAboutStaticMsgProc);
			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			EndDialog(hWnd,NULL);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}
//Tab�ӶԻ���T0��Ϣ����
BOOL WINAPI DlgT0(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hReal_w=GetDlgItem(hWnd,IDC_T0_C_REAL);
	HWND hImag_i=GetDlgItem(hWnd,IDC_T0_C_IMAG);
	HWND hImag_j=GetDlgItem(hWnd,IDC_T0_C_IMAG_J);
	HWND hImag_k=GetDlgItem(hWnd,IDC_T0_C_IMAG_K);
	HWND hMandelbrot=GetDlgItem(hWnd,IDC_RBN_T0_MANDELBROT);
	HWND hNewton=GetDlgItem(hWnd,IDC_RBN_T0_NEWTON);

	//�õ������ļ�·��
	char filename[260];
	string buffer;
	GetModuleFileName(NULL,filename,260);
	buffer=filename;
	buffer.assign(buffer,0,buffer.find_last_of('\\'));
	buffer+="\\Fractal.ini";

	int Ar,Ag,Ab,Br,Bg,Bb;
	unsigned long Rmax,Nmax;

	switch(uMsg)
	{
	case WM_INITDIALOG://�򿪳���ʱ���г����б���Ĺ�ʽ
		{
			//=================================================
			//��ʼ��T0�Ի���
			//=================================================
			//�����б��
			char inibuffer[1000];
			//�õ����н�����inibuffer,ÿ��������0����������2��0����
			if(GetPrivateProfileSectionNames(inibuffer,1000,buffer.c_str()))//�����ļ��ɹ�
			{
				char *p=inibuffer;
			
				while(*(p))
				{
					SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_ADDSTRING,0,(LPARAM)p);//���浽�б��
					p+=strlen(p)+1;//��������һ������
				
				}
			}

			//����΢����ť(�����༭��)
			HWND hUdn_Rmax=GetDlgItem(hWnd,IDC_UDN_T0_RMAX);//RMAX
			HWND hRmax=GetDlgItem(hWnd,IDC_T0_RMAX);
			SendMessage(hUdn_Rmax,UDM_SETBUDDY,(WPARAM)hRmax,(LPARAM)0);
			SendMessage(hUdn_Rmax,UDM_SETRANGE,(WPARAM)0,(LPARAM)10000);

			HWND hUdn_Nmax=GetDlgItem(hWnd,IDC_UDN_T0_NMAX);//NMAX
			HWND hNmax=GetDlgItem(hWnd,IDC_T0_NMAX);
			SendMessage(hUdn_Nmax,UDM_SETBUDDY,(WPARAM)hNmax,(LPARAM)0);
			SendMessage(hUdn_Nmax,UDM_SETRANGE,(WPARAM)0,(LPARAM)10000);

			HWND hUdn_Ar=GetDlgItem(hWnd,IDC_UDN_T0_AR);//Ar
			HWND hAr=GetDlgItem(hWnd,IDC_T0_AR);
			SendMessage(hUdn_Ar,UDM_SETBUDDY,(WPARAM)hAr,(LPARAM)0);
			SendMessage(hUdn_Ar,UDM_SETRANGE,(WPARAM)0,(LPARAM)255);

			HWND hUdn_Br=GetDlgItem(hWnd,IDC_UDN_T0_BR);//Br
			HWND hBr=GetDlgItem(hWnd,IDC_T0_BR);
			SendMessage(hUdn_Br,UDM_SETBUDDY,(WPARAM)hBr,(LPARAM)0);
			SendMessage(hUdn_Br,UDM_SETRANGE,(WPARAM)0,(LPARAM)255);

			HWND hUdn_Ag=GetDlgItem(hWnd,IDC_UDN_T0_AG);//Ag
			HWND hAg=GetDlgItem(hWnd,IDC_T0_AG);
			SendMessage(hUdn_Ag,UDM_SETBUDDY,(WPARAM)hAg,(LPARAM)0);
			SendMessage(hUdn_Ag,UDM_SETRANGE,(WPARAM)0,(LPARAM)255);

			HWND hUdn_Bg=GetDlgItem(hWnd,IDC_UDN_T0_BG);//Bg
			HWND hBg=GetDlgItem(hWnd,IDC_T0_BG);
			SendMessage(hUdn_Bg,UDM_SETBUDDY,(WPARAM)hBg,(LPARAM)0);
			SendMessage(hUdn_Bg,UDM_SETRANGE,(WPARAM)0,(LPARAM)255);

			HWND hUdn_Ab=GetDlgItem(hWnd,IDC_UDN_T0_AB);//Ab
			HWND hAb=GetDlgItem(hWnd,IDC_T0_AB);
			SendMessage(hUdn_Ab,UDM_SETBUDDY,(WPARAM)hAb,(LPARAM)0);
			SendMessage(hUdn_Ab,UDM_SETRANGE,(WPARAM)0,(LPARAM)255);

			HWND hUdn_Bb=GetDlgItem(hWnd,IDC_UDN_T0_BB);//Bb
			HWND hBb=GetDlgItem(hWnd,IDC_T0_BB);
			SendMessage(hUdn_Bb,UDM_SETBUDDY,(WPARAM)hBb,(LPARAM)0);
			SendMessage(hUdn_Bb,UDM_SETRANGE,(WPARAM)0,(LPARAM)255);

			//=================================================
			//���õ�ѡ��ť��ʼ״̬
			CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_CHECKED);//Mandelbrot
			CheckDlgButton(hWnd,IDC_RBN_T0_2,BST_CHECKED);//ʹ�ö�ά��ѡ��ť
			//��ʼ���ó���c�ı༭��
			EnableWindow(hReal_w,FALSE);
			EnableWindow(hImag_i,FALSE);
			EnableWindow(hImag_j,FALSE);
			EnableWindow(hImag_k,FALSE);
			//==================================================
			//���ñ༭���ֵ
			HWND hGongshi=GetDlgItem(hWnd,IDC_T0_GONGSHI_EDT);
			SetWindowText(hGongshi,"z*z+c");//��ʽ
			SetWindowText(hRmax,"50");//���ݰ뾶
			SetWindowText(hNmax,"50");//����ʱ��
			SetWindowText(hAr,"10");//��ɫ����
			SetWindowText(hBr,"5");
			SetWindowText(hAg,"10");
			SetWindowText(hBg,"10");
			SetWindowText(hAb,"16");
			SetWindowText(hBb,"10");
			SetWindowText(hReal_w,"0");//����C
			SetWindowText(hImag_i,"1");
			SetWindowText(hImag_j,"0");
			SetWindowText(hImag_k,"0");
			
			
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID=LOWORD(wParam);//�ؼ�ID
			int ControlNotify=HIWORD(wParam);//�ؼ�֪ͨ��
			HWND ControlHandle=(HWND)lParam;//�ؼ����

			if(ControlID==IDC_T0_SAVE)//���湫ʽ
			{
				char gongshi[260];
				HWND hEdit=GetDlgItem(hWnd,IDC_T0_GONGSHI_EDT);
				GetWindowText(hEdit,gongshi,260);//�õ���ʽ

				SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_ADDSTRING,0,(LPARAM)gongshi);//���浽�б��

				//�õ�����
				//========================================================
				//��ɫ����
				char canshu[10];
				Ar=(int)GetDlgItemInt(hT0,IDC_T0_AR,NULL,FALSE);
				Ag=(int)GetDlgItemInt(hT0,IDC_T0_AG,NULL,FALSE);
				Ab=(int)GetDlgItemInt(hT0,IDC_T0_AB,NULL,FALSE);
				Br=(int)GetDlgItemInt(hT0,IDC_T0_BR,NULL,FALSE);
				Bg=(int)GetDlgItemInt(hT0,IDC_T0_BG,NULL,FALSE);
				Bb=(int)GetDlgItemInt(hT0,IDC_T0_BB,NULL,FALSE);

				Rmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_RMAX,NULL,FALSE);//Rmax
				Nmax=(unsigned long)GetDlgItemInt(hT0,IDC_T0_NMAX,NULL,FALSE);//Nmax
				
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

				//����C,B
				char real[20],imag_i[20],imag_j[20],imag_k[20];
				GetWindowText(hReal_w,real,20);
				GetWindowText(hImag_i,imag_i,20);
				GetWindowText(hImag_j,imag_j,20);
				GetWindowText(hImag_k,imag_k,20);

				//�õ�ά��
				int weishu=2;
				if(IsDlgButtonChecked(hWnd,IDC_RBN_T0_3))weishu=3;
				

				//д�������ļ�
				sprintf(canshu,"%d",weishu);//ά��
				WritePrivateProfileString(gongshi,"WeiShu",canshu,buffer.c_str());

				sprintf(canshu,"%d",select);//ѡ�񼯺�
				WritePrivateProfileString(gongshi,"Select",canshu,buffer.c_str());

				WritePrivateProfileString(gongshi,"C_Real",real,buffer.c_str());//����C
				WritePrivateProfileString(gongshi,"C_Imag_i",imag_i,buffer.c_str());
				WritePrivateProfileString(gongshi,"C_Imag_j",imag_j,buffer.c_str());
				WritePrivateProfileString(gongshi,"C_Imag_k",imag_k,buffer.c_str());

				sprintf(canshu,"%d",Ar);//��ɫ
				WritePrivateProfileString(gongshi,"Ar",canshu,buffer.c_str());
				sprintf(canshu,"%d",Ag);
				WritePrivateProfileString(gongshi,"Ag",canshu,buffer.c_str());
				sprintf(canshu,"%d",Ab);
				WritePrivateProfileString(gongshi,"Ab",canshu,buffer.c_str());
				sprintf(canshu,"%d",Br);
				WritePrivateProfileString(gongshi,"Br",canshu,buffer.c_str());
				sprintf(canshu,"%d",Bg);
				WritePrivateProfileString(gongshi,"Bg",canshu,buffer.c_str());
				sprintf(canshu,"%d",Bb);
				WritePrivateProfileString(gongshi,"Bb",canshu,buffer.c_str());

				sprintf(canshu,"%d",Rmax);//���ݰ뾶��ʱ��
				WritePrivateProfileString(gongshi,"Rmax",canshu,buffer.c_str());
				sprintf(canshu,"%d",Nmax);
				WritePrivateProfileString(gongshi,"Nmax",canshu,buffer.c_str());

			}
			else if(ControlID==IDC_T0_CLEAR)//�����ʽ
			{
				int sel=SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_GETCURSEL,0,0);//�õ���ǰ����

				if(sel>=0)
				{
					//�������ļ�������ù�ʽ
					char gongshi[260];
					SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_GETTEXT,(WPARAM)sel,(LPARAM)gongshi);
					WritePrivateProfileString(gongshi,0,0,buffer.c_str());
	
					SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_DELETESTRING,(WPARAM)sel,0);//���б���������ʽ
				}
			
			}
			else if(ControlID==IDC_T0_CLEAN)//��չ�ʽ�б��
			{
				
				int selcounts=SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_GETCOUNT,0,0);//�õ��б��������
				for(int i=0;i<selcounts;i++)//ѭ���������С��
				{
					char str[260];
					SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_GETTEXT,(WPARAM)i,(LPARAM)str);
					WritePrivateProfileString(str,NULL,NULL,buffer.c_str());
				
				}
				SendDlgItemMessage(hWnd,IDC_T0_LIST,LB_RESETCONTENT,0,0);//���б������չ�ʽ
			
			}
			else if(ControlID==IDC_T0_LIST)//��Ϊ�б��
			{
				if(ControlNotify==LBN_DBLCLK)//��Ϊ˫��
				{
					char gongshi[260];
					int sel=SendMessage(ControlHandle,LB_GETCURSEL,0,0);//�õ���ǰ����
					SendMessage(ControlHandle,LB_GETTEXT,(WPARAM)sel,(LPARAM)gongshi);//�õ���ʽ
			
					HWND hEdit=GetDlgItem(hWnd,IDC_T0_GONGSHI_EDT);
					SetWindowText(hEdit,gongshi);//����ʽ��ر༭��

					//�õ�����������ر༭��
					int color;
					color=GetPrivateProfileInt(gongshi,"Ar",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_AR,color,FALSE);
					color=GetPrivateProfileInt(gongshi,"Ag",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_AG,color,FALSE);
					color=GetPrivateProfileInt(gongshi,"Ab",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_AB,color,FALSE);
					color=GetPrivateProfileInt(gongshi,"Br",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_BR,color,FALSE);
					color=GetPrivateProfileInt(gongshi,"Bg",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_BG,color,FALSE);
					color=GetPrivateProfileInt(gongshi,"Bb",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_BB,color,FALSE);

					Rmax=GetPrivateProfileInt(gongshi,"Rmax",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_RMAX,Rmax,FALSE);
					Nmax=GetPrivateProfileInt(gongshi,"Nmax",0,buffer.c_str());
					SetDlgItemInt(hWnd,IDC_T0_NMAX,Nmax,FALSE);

					//���ó���c,b�ı༭��
					char real[20],imag_i[20],imag_j[20],imag_k[20];
					GetPrivateProfileString(gongshi,"C_Real","0",real,20,buffer.c_str());
					GetPrivateProfileString(gongshi,"C_Imag_i","0",imag_i,20,buffer.c_str());
					GetPrivateProfileString(gongshi,"C_Imag_j","0",imag_j,20,buffer.c_str());
					GetPrivateProfileString(gongshi,"C_Imag_k","0",imag_k,20,buffer.c_str());
					SetWindowText(hReal_w,real);
					SetWindowText(hImag_i,imag_i);
					SetWindowText(hImag_j,imag_j);
					SetWindowText(hImag_k,imag_k);

					int weishu=GetPrivateProfileInt(gongshi,"WeiShu",2,buffer.c_str());
					if(weishu==3)
					{
						EnableWindow(hReal_w,TRUE);
						EnableWindow(hImag_i,TRUE);
						EnableWindow(hImag_j,TRUE);
						EnableWindow(hImag_k,TRUE);
						EnableWindow(hMandelbrot,FALSE);
						EnableWindow(hNewton,FALSE);
						CheckDlgButton(hWnd,IDC_RBN_T0_JULIA,BST_CHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_NEWTON,BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_3,BST_CHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_2,BST_UNCHECKED);
					
					}
					else
					{
						EnableWindow(hReal_w,TRUE);
						EnableWindow(hImag_i,TRUE);
						EnableWindow(hImag_j,FALSE);
						EnableWindow(hImag_k,FALSE);
						EnableWindow(hMandelbrot,TRUE);
						EnableWindow(hNewton,TRUE);
						CheckDlgButton(hWnd,IDC_RBN_T0_2,BST_CHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_3,BST_UNCHECKED);
					
					
					}
			

					//ѡ����Ƶļ���
					int select=GetPrivateProfileInt(gongshi,"Select",1,buffer.c_str());
					if(select==FRACTAL_T0_MANDELBROT)//Mandelbrot
					{
						CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_CHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_JULIA,BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_NEWTON,BST_UNCHECKED);
						EnableWindow(hReal_w,FALSE);
						EnableWindow(hImag_i,FALSE);
					}
					else if(select==FRACTAL_T0_JULIA)//Julia
					{
						CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_NEWTON,BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_JULIA,BST_CHECKED);
						EnableWindow(hReal_w,TRUE);
						EnableWindow(hImag_i,TRUE);
					}
					else if(select==FRACTAL_T0_NEWTON)//Newton
					{
						CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_JULIA,BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_NEWTON,BST_CHECKED);
						EnableWindow(hReal_w,TRUE);
						EnableWindow(hImag_i,TRUE);
					}
					else //ȱʡΪMandelbrot
					{
						CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_CHECKED);
						CheckDlgButton(hWnd,IDC_RBN_T0_JULIA,BST_UNCHECKED);
						EnableWindow(hReal_w,FALSE);
						EnableWindow(hImag_i,FALSE);
					
					}
					
				}
				
			
			}
			else if(ControlID==IDC_RBN_T0_MANDELBROT)
			{
				EnableWindow(hReal_w,FALSE);
				EnableWindow(hImag_i,FALSE);
				HWND hGongShi_Edit=GetDlgItem(hWnd,IDC_T0_GONGSHI_EDT);//���ó�ʼ��ʽ
				SetWindowText(hGongShi_Edit,"z*z+c");
			
			}
			else if(ControlID==IDC_RBN_T0_JULIA)
			{
				EnableWindow(hReal_w,TRUE);
				EnableWindow(hImag_i,TRUE);
				HWND hGongShi_Edit=GetDlgItem(hWnd,IDC_T0_GONGSHI_EDT);//���ó�ʼ��ʽ
				SetWindowText(hGongShi_Edit,"z*z+c");
			
			}
			else if(ControlID==IDC_RBN_T0_NEWTON)
			{
				EnableWindow(hReal_w,TRUE);
				EnableWindow(hImag_i,TRUE);
				HWND hGongShi_Edit=GetDlgItem(hWnd,IDC_T0_GONGSHI_EDT);//���ó�ʼ��ʽ
				SetWindowText(hGongShi_Edit,"z*z+c");
			
			}
			else if(ControlID==IDC_T0_STOCK)//ѡ���湫ʽ
			{
				DialogBoxParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_DLG_STOCK),hWnd,DlgT1Stock,0);//��湫ʽѡ��Ի���
			
			}
			else if(ControlID==IDC_RBN_T0_2)//��Ϊ2ά
			{
				EnableWindow(hReal_w,TRUE);
				EnableWindow(hImag_i,TRUE);
				EnableWindow(hImag_j,FALSE);
				EnableWindow(hImag_k,FALSE);
				EnableWindow(hMandelbrot,TRUE);
				EnableWindow(hNewton,TRUE);
				CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_CHECKED);
				CheckDlgButton(hWnd,IDC_RBN_T0_JULIA,BST_UNCHECKED);
				CheckDlgButton(hWnd,IDC_RBN_T0_NEWTON,BST_UNCHECKED);
			}
			else if(ControlID==IDC_RBN_T0_3)//��Ϊ3ά
			{
				EnableWindow(hReal_w,TRUE);
				EnableWindow(hImag_i,TRUE);
				EnableWindow(hImag_j,TRUE);
				EnableWindow(hImag_k,TRUE);
				EnableWindow(hMandelbrot,FALSE);
				EnableWindow(hNewton,FALSE);
				CheckDlgButton(hWnd,IDC_RBN_T0_JULIA,BST_CHECKED);
				CheckDlgButton(hWnd,IDC_RBN_T0_MANDELBROT,BST_UNCHECKED);
				CheckDlgButton(hWnd,IDC_RBN_T0_NEWTON,BST_UNCHECKED);
			}
			return TRUE;
		}
		break;
	case WM_CLOSE:
		{
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}
//Tab�ӶԻ���T1��Ϣ����
BOOL WINAPI DlgT1(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}
DWORD WINAPI OnT2Test(LPVOID lpParam)//�����Ƽ�ֵ,������Ѳ���ֵ
{
	RECT r;
	GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С
	//========================================================
	double width=r.right-r.left;
	double height=r.bottom-r.top;
	double x0=width/2;
	double y0=height/2;
	double min_x=0,max_x=0,min_y=0,max_y=0;
	int i;
	string bufrules;
	//�õ�����
	//=======================================================
	char *st;
	//��ʼ��λ����
	double lenght=1;

	int weishu;//�õ�ά��
	if(IsDlgButtonChecked(hT2,IDC_RBN_T2_2)&&!IsDlgButtonChecked(hT2,IDC_RBN_T2_3))
		weishu=2;
	else if(!IsDlgButtonChecked(hT2,IDC_RBN_T2_2)&&IsDlgButtonChecked(hT2,IDC_RBN_T2_3))
		weishu=3;
	else weishu=2;

	long n;//�õ���������
	char nstr[20];
	HWND hN=GetDlgItem(hT2,IDC_T2_N);
	GetWindowText(hN,nstr,20);
	n=atol(nstr);

	//�õ���ת�Ƕ�
	double angle;
	char anglestr[20];
	HWND hAngle=GetDlgItem(hT2,IDC_T2_DEGREE);
	GetWindowText(hAngle,anglestr,20);
	angle=strtod(anglestr,&st);
	
	//==========================

	char wenfa[10000];//�õ��ķ��ı�
	HWND hWenfa=GetDlgItem(hT2,IDC_T2_WENFA_EDIT);
	GetWindowText(hWenfa,wenfa,10000);

	vector<LS> Rules;//תΪ��������(�ֽ����)
	if(wenfastr.compare(wenfa)!=0||ls_count!=n)//�ж��ķ��Ƿ�ı�
	{
		wenfastr.assign(wenfa);
		ls_count=n;
		
		string str;
		char *p=wenfa;

		//�õ���ʼ����
		string initrule;
		int init_index=wenfastr.find_first_of("\r\n");
		initrule.assign(wenfa,init_index);
		p+=init_index+2;
		rules_str=initrule;

		if(rules_str.size()==0||rules_str.find_first_of('=')!=-1)
		{
			MessageBox(MAIN_HANDLE,"�������ʼ����!","����",MB_OK);
			return TRUE;
		}
	 
	
		if(wenfastr.find_first_of('@')==-1)
		{
			MessageBox(MAIN_HANDLE,"�������ķ�������:'@'!","����",MB_OK);
			return TRUE;

		}
		while(*p)
		{
			if(*p=='\r'||*p=='#')
			{
				//�ֽ����
				int index=str.find_first_of('=');
				if(index!=-1)
				{
					LS rule;
					if(index!=1)
					{
						rule.head.assign(str,0,index);
						char errstr[100];
						sprintf(errstr,"����Ԫ'%s'�����ַ�����!",rule.head.c_str());
						MessageBox(MAIN_HANDLE,errstr,"����",MB_OK);
						return TRUE;
					}
				
					rule.head.assign(str,0,1);
					rule.body.assign(str,2,str.length()-2);
					Rules.push_back(rule);
					if(rule.head.size()==0)
					{
						MessageBox(MAIN_HANDLE,"LS�﷨����!","����",MB_OK);
						return TRUE;
					}
				}
				str.resize(0);
				//����ע�Ͳ���
				if(*p=='#')
				{
					while(*(p+1))
					{
						p++;
						if(*p=='\n')
						{
							break;
						}
				
					}
				}
			}
			else if(!isspace(*p))//���˵��հ��ַ�
			{
				str+=*p;
			}
			if(*p=='@')break;//�ļ�������
		
			p++;
		}
	//for(i=0;i<Rules.size();i++)
		//DebugBox("head:%s,body:%s",Rules[i].head.c_str(),Rules[i].body.c_str());
	//========================================================
		//�������
		while(n-->0)
		{
			bufrules=rules_str;
			char *p=(char *)bufrules.c_str();
			rules_str.resize(0);
			while(*p)
			{
				string s;
				s.assign(p,1);
				for(i=0;i<Rules.size();i++)
				{
					if(Rules[i].head.compare(s)==0)
					{
						rules_str+=Rules[i].body;
						break;
					}
			
			
				}
				if(i==Rules.size())
				{
					rules_str+=s;
				}
				p++;
		
			}
		}
		if(rules_str.compare(initrule)==0)
		{
			MessageBox(MAIN_HANDLE,"��ƥ������Ԫ!","����",MB_OK);
			return TRUE;
		}
	}
	if(weishu==2)//����ά�ķ�ͼ
	{
		LSDOT CurDot;
		stack<LSDOT> DotStack;

		CurDot.x=0;//���ó�ʼ����
		CurDot.y=0;
		CurDot.xdeg=0;

		char *p=(char *)rules_str.c_str();

		while(*p)
		{
			if(*p=='F')//��ǰ��һ��������(1����λ����)
			{
				CurDot.x+=lenght*cos(CurDot.xdeg*PI/180);
				CurDot.y+=lenght*sin(CurDot.xdeg*PI/180);

				min_x=CurDot.x>min_x?min_x:CurDot.x;
				max_x=CurDot.x<max_x?max_x:CurDot.x;
				min_y=CurDot.y>min_y?min_y:CurDot.y;
				max_y=CurDot.y<max_y?max_y:CurDot.y;
	
			}
			else if(*p=='L')
			{
			}
			else if(*p=='R')
			{
			}
			else if(*p=='+')//��ת
			{
				string bufstr(p);
				int begin_index=bufstr.find_first_of('(');
				int end_index=bufstr.find_first_of(')');

				if(begin_index==1&&end_index>begin_index)//��ת����Ƕ�
				{
					//ȡ�����е�ֵ
					bufstr.assign(p,begin_index+1,end_index-begin_index-1);
					double ang=strtod(bufstr.c_str(),&st);
					CurDot.xdeg+=ang;

					p+=end_index-begin_index+2;
					continue;
				}
				else//��תĬ�ϽǶ�
				{
					CurDot.xdeg+=angle;
				}

			}
			else if(*p=='-')//��ת
			{
				string bufstr(p);
				int begin_index=bufstr.find_first_of('(');
				int end_index=bufstr.find_first_of(')');

				if(begin_index==1&&end_index>begin_index)//��ת����Ƕ�
				{
					bufstr.assign(p,begin_index+1,end_index-begin_index-1);
					double ang=strtod(bufstr.c_str(),&st);
					CurDot.xdeg-=ang;

					p+=end_index-begin_index+2;
					continue;
				}
				else//��תĬ�ϽǶ�
				{
					CurDot.xdeg-=angle;
				}
			}
			else if(*p=='[')//ѹջ
			{
				DotStack.push(CurDot);
			
			}
			else if(*p==']')//��ջ
			{
				if(DotStack.size()>0)
				{
					CurDot=DotStack.top();
					DotStack.pop();
				}
			
			}
			else ;
			p++;
		}
		
		//=================
		//����
		double br=(height/(max_y-min_y))>(width/(max_x-min_x))?(width/(max_x-min_x)):(height/(max_y-min_y));
		//������������
		min_x*=br;
		min_y*=br;
		max_x*=br;
		max_y*=br;
		//��������
		lenght=br*lenght;
		//��ʾ�Ƽ�����ֵ
		char s[20];
		sprintf(s,"%lf",lenght);
		HWND hLen=GetDlgItem(hT2,IDC_T2_LEN);
		SetWindowText(hLen,s);

		//�����Ƽ�����ֵ�����ʼ����
		//��ʾ�Ƽ���ʼ����
		sprintf(s,"%lf",-(min_x+max_x)/2);
		HWND hX0=GetDlgItem(hT2,IDC_T2_X0);
		SetWindowText(hX0,s);
		sprintf(s,"%lf",-(min_y+max_y)/2);
		HWND hY0=GetDlgItem(hT2,IDC_T2_Y0);
		SetWindowText(hY0,s);

	}
	else if(weishu==3)//����ά�ķ�ͼ
	{

	
	}
						

	return 1;
}
//Tab�ӶԻ���T2��Ϣ����
BOOL WINAPI DlgT2(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			CheckDlgButton(hWnd,IDC_RBN_T2_2,BST_CHECKED);//ʹ�ö�ά��ѡ��ť
			HWND hN=GetDlgItem(hWnd,IDC_T2_N);
			SetWindowText(hN,"5");//��������
			HWND hDegree=GetDlgItem(hWnd,IDC_T2_DEGREE);
			SetWindowText(hDegree,"0");//��ת�Ƕ�
			//��ʼ����
			HWND hX0=GetDlgItem(hWnd,IDC_T2_X0);
			SetWindowText(hX0,"0");
			HWND hY0=GetDlgItem(hWnd,IDC_T2_Y0);
			SetWindowText(hY0,"0");
			HWND hZ0=GetDlgItem(hWnd,IDC_T2_Z0);
			SetWindowText(hZ0,"0");
			//�һ��ؼ�
			EnableWindow(hZ0,FALSE);
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID= LOWORD(wParam);//�ؼ�ID

			HWND hZ0=GetDlgItem(hWnd,IDC_T2_Z0);
			if(ControlID==IDC_RBN_T2_2)
			{
				EnableWindow(hZ0,FALSE);
			}
			else if(ControlID==IDC_RBN_T2_3)
			{
				EnableWindow(hZ0,TRUE);
			
			}
			else if(ControlID==IDC_T2_SAVE)//�����ķ�
			{
				DialogBoxParam(MAIN_INSTANCE,MAKEINTRESOURCE\
						(IDD_T4_SETNAME_DLG),hWnd,Dlg_T2_SetName,0);//�������ƶԻ���
			}
			else if(ControlID==IDC_T2_OPEN_STOCK)//ѡ�����ķ�
			{
				DialogBoxParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_DLG_STOCK),hWnd,DlgT2Stock,0);//��湫ʽѡ��Ի���
			
			}
			else if(ControlID==IDC_T2_TEST)//������Ѳ���ֵ
			{
				HANDLE hthread=CreateThread(NULL,0,OnT2Test,0,0,0);
				CloseHandle(hthread);
						
			}

			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}
//Tab�ӶԻ���T3��Ϣ����
BOOL WINAPI DlgT3(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hColor=GetDlgItem(hWnd,IDC_T3_COLOR);
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			//=================================================
			//��ʼ��T3�Ի���
			//=================================================
			//����΢����ť(�����༭��)
			HWND hUdn_Xmax=GetDlgItem(hWnd,IDC_UDN_T3_XMAX);//X_MAX
			HWND hXmax=GetDlgItem(hWnd,IDC_T3_XMAX);
			SendMessage(hUdn_Xmax,UDM_SETBUDDY,(WPARAM)hXmax,(LPARAM)0);
			SendMessage(hUdn_Xmax,UDM_SETRANGE,(WPARAM)0,(LPARAM)10000);

			HWND hUdn_Ymax=GetDlgItem(hWnd,IDC_UDN_T3_YMAX);//Y_MAX
			HWND hYmax=GetDlgItem(hWnd,IDC_T3_YMAX);
			SendMessage(hUdn_Ymax,UDM_SETBUDDY,(WPARAM)hYmax,(LPARAM)0);
			SendMessage(hUdn_Ymax,UDM_SETRANGE,(WPARAM)0,(LPARAM)10000);

			HWND hUdn_Zmax=GetDlgItem(hWnd,IDC_UDN_T3_ZMAX);//Z_MAX
			HWND hZmax=GetDlgItem(hWnd,IDC_T3_ZMAX);
			SendMessage(hUdn_Zmax,UDM_SETBUDDY,(WPARAM)hZmax,(LPARAM)0);
			SendMessage(hUdn_Zmax,UDM_SETRANGE,(WPARAM)0,(LPARAM)10000);

			HWND hUdn_Pmax=GetDlgItem(hWnd,IDC_UDN_T3_PMAX);
			HWND hPmax=GetDlgItem(hWnd,IDC_EDT_T3_PMAX);//P_MAX�༭��
			SendMessage(hUdn_Pmax,UDM_SETBUDDY,(WPARAM)hPmax,(LPARAM)0);
			SendMessage(hUdn_Pmax,UDM_SETRANGE,(WPARAM)-10000,(LPARAM)10000);

			HWND hUdn_Qmax=GetDlgItem(hWnd,IDC_UDN_T3_QMAX);
			HWND hQmax=GetDlgItem(hWnd,IDC_EDT_T3_QMAX);//Q_MAX�༭��
			SendMessage(hUdn_Qmax,UDM_SETBUDDY,(WPARAM)hQmax,(LPARAM)0);
			SendMessage(hUdn_Qmax,UDM_SETRANGE,(WPARAM)-10000,(LPARAM)10000);
			
			//��ʼ����ѡ��ť
			//CheckRadioButton(hWnd,IDC_T3_RBN1,IDC_T3_RBN1,IDC_T3_RBN1);
			CheckDlgButton(hWnd,IDC_T3_RBN1,BST_CHECKED);//Ĭ��ʹ�ñ��ʽ
			CheckDlgButton(hWnd,IDC_RBN_T3_2,BST_CHECKED);//ʹ�ö�ά��ѡ��ť
			//�һ������༭��ؼ�
			HWND hEditX=GetDlgItem(hWnd,IDC_T3_EDIT_X);
			HWND hEditY=GetDlgItem(hWnd,IDC_T3_EDIT_Y);
			HWND hEditZ=GetDlgItem(hWnd,IDC_T3_EDIT_Z);
			EnableWindow(hEditX,FALSE);
			EnableWindow(hEditY,FALSE);
			EnableWindow(hEditZ,FALSE);
			EnableWindow(hPmax,FALSE);
			EnableWindow(hQmax,FALSE);
			EnableWindow(hZmax,FALSE);
			//������ά��ѡ��ť
			HWND hRbn3=GetDlgItem(hWnd,IDC_RBN_T3_3);
			HWND hRbn2=GetDlgItem(hWnd,IDC_RBN_T3_2);
			EnableWindow(hRbn3,FALSE);
			EnableWindow(hRbn2,FALSE);

			//=================================================
			//���ñ༭���ֵ
			HWND hgongshi=GetDlgItem(hWnd,IDC_T3_GONGSHI);
			SetWindowText(hgongshi,"y=x^2+2*x+1");
			SetWindowText(hXmax,"7");
			SetWindowText(hYmax,"5");
			SetWindowText(hZmax,"5");
			SetWindowText(hPmax,"3");
			SetWindowText(hQmax,"3");

			//=================================================

			//Ƕ���µ���ɫ��̬�ؼ���Ϣ������
			OldT3ColorMsgProc=(WNDPROC)SetWindowLong(hColor,GWL_WNDPROC,(LONG)MyT3ColorMsgProc);

			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID= LOWORD(wParam);//�ؼ�ID

			HWND hEditX=GetDlgItem(hWnd,IDC_T3_EDIT_X);//������ʽ�༭��
			HWND hEditY=GetDlgItem(hWnd,IDC_T3_EDIT_Y);
			HWND hEditZ=GetDlgItem(hWnd,IDC_T3_EDIT_Z);
			HWND hEdit=GetDlgItem(hWnd,IDC_T3_GONGSHI);//��ͨ��ʽ�༭��
			HWND hPmax=GetDlgItem(hWnd,IDC_EDT_T3_PMAX);//P_MAX�༭��
			HWND hQmax=GetDlgItem(hWnd,IDC_EDT_T3_QMAX);//Q_MAX�༭��
			HWND hZmax=GetDlgItem(hWnd,IDC_T3_ZMAX);//Z_MAX�༭��
			HWND hRbn2=GetDlgItem(hWnd,IDC_RBN_T3_2);//2ά��ѡ��ť
			HWND hRbn3=GetDlgItem(hWnd,IDC_RBN_T3_3);//3ά��ѡ��ť

			//��ֹ��ʹ�ÿؼ�
			if(ControlID==IDC_T3_RBN1)//����ʽ����Ϊ���ʽ
			{
				EnableWindow(hEditX,FALSE);
				EnableWindow(hEditY,FALSE);
				EnableWindow(hEditZ,FALSE);
				EnableWindow(hPmax,FALSE);
				EnableWindow(hQmax,FALSE);
				EnableWindow(hZmax,FALSE);
				EnableWindow(hRbn2,FALSE);
				EnableWindow(hRbn3,FALSE);
				EnableWindow(hEdit,TRUE);
			


			}
			else if(ControlID==IDC_T3_RBN2)//����ʽ����Ϊ�������ʽ
			{
				EnableWindow(hEdit,FALSE);
				EnableWindow(hEditX,TRUE);
				EnableWindow(hEditY,TRUE);
				EnableWindow(hRbn3,TRUE);
				EnableWindow(hRbn2,TRUE);
				EnableWindow(hPmax,TRUE);
				
				//����ά�����ñ༭��
				if(IsDlgButtonChecked(hWnd,IDC_RBN_T3_2)&&!IsDlgButtonChecked(hWnd,IDC_RBN_T3_3))//2ά
				{
					EnableWindow(hEditZ,FALSE);
					EnableWindow(hZmax,FALSE);
					EnableWindow(hQmax,FALSE);
				}
				else if(!IsDlgButtonChecked(hWnd,IDC_RBN_T3_2)&&IsDlgButtonChecked(hWnd,IDC_RBN_T3_3))//3ά
				{
					EnableWindow(hEditZ,TRUE);
					EnableWindow(hZmax,TRUE);
					EnableWindow(hQmax,TRUE);
				}
				else //ȱʡ2ά
				{
					EnableWindow(hEditZ,FALSE);
					EnableWindow(hZmax,FALSE);
					EnableWindow(hQmax,FALSE);
				}
			
			}
			else if(ControlID==IDC_RBN_T3_2)//��Ϊ2ά��ѡ��ť
			{
				EnableWindow(hEditZ,FALSE);
				EnableWindow(hQmax,FALSE);
				EnableWindow(hZmax,FALSE);
			
			}
			else if(ControlID==IDC_RBN_T3_3)//��Ϊ3ά��ѡ��ť
			{
				EnableWindow(hEditZ,TRUE);
				EnableWindow(hQmax,TRUE);
				EnableWindow(hZmax,TRUE);
			
			}
			else if(ControlID==IDC_T3_SETCOLOR)//��Ϊ������ɫ�Ի���
			{
				//��ͨ����ɫ�Ի���
				COLORREF cusColor[16];

				CHOOSECOLOR lcc;
				lcc.lStructSize=sizeof(CHOOSECOLOR);
				lcc.hwndOwner=hWnd;
				lcc.hInstance=NULL;
				lcc.rgbResult=RGB(0,0,0);
				lcc.lpCustColors=cusColor; 
				lcc.Flags=CC_RGBINIT|CC_FULLOPEN| CC_ANYCOLOR;
				lcc.lCustData=NULL;
				lcc.lpfnHook=NULL;
				lcc.lpTemplateName=NULL;

				if(ChooseColor(&lcc))
				{
					T3_Color=lcc.rgbResult;//���滭����ɫ

					//��ʾ��̬�ؼ���ɫ
					HDC hdc=GetDC(hColor);
					RECT cr;
					GetClientRect(hColor,&cr);
					HBRUSH hbrush=CreateSolidBrush(T3_Color);
					FillRect(hdc,&cr,hbrush);
					ReleaseDC(hColor,hdc);
				
				}
			
			}
			return TRUE;
		}
		break;
	case WM_NOTIFY://�ؼ�֪ͨ��Ϣ
		{
			int ControlID= LOWORD(wParam);//�ؼ�ID
			LPNMHDR pNmhdr=(LPNMHDR)lParam;//NMHDR�ṹ��(�ؼ���Ϣͷ)
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}
//Tab�ӶԻ���T4��Ϣ����
BOOL WINAPI DlgT4(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	HWND hListView=GetDlgItem(hWnd,IDC_T4_LISTVIEW);

	LVCOLUMN lvc;//listview�нṹ��
	lvc.mask=LVCF_TEXT|LVCF_WIDTH|LVCF_FMT|LVCF_SUBITEM;
	lvc.fmt=LVCFMT_CENTER;//��ʽ����
	lvc.cchTextMax=256;

	LVITEM litem;//listview��ṹ��
	litem.mask=LVIF_TEXT;
	litem.cchTextMax=256;

	//�õ������ļ�·��
	char filename[260];
	string inistr;
	GetModuleFileName(NULL,filename,260);
	inistr=filename;
	inistr.assign(inistr,0,inistr.find_last_of('\\'));
	inistr+="\\IFS.ini";

	int i;
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			//=================================================
			//��ʼ��T4�Ի���
			//=================================================
			//��ʼ��ListView
			//����8��
			//------------------------------------
			lvc.iSubItem=0;
			lvc.cx=24;//�п�
			lvc.pszText="i";//�еı���
			ListView_InsertColumn(hListView,0,&lvc);
			lvc.cx=38;//�п�
			lvc.pszText="a";//�еı���
			ListView_InsertColumn(hListView,1,&lvc);
			lvc.pszText="b";
			ListView_InsertColumn(hListView,2,&lvc);
			lvc.pszText="c";
			ListView_InsertColumn(hListView,3,&lvc);
			lvc.pszText="d";
			ListView_InsertColumn(hListView,4,&lvc);
			lvc.pszText="e";
			ListView_InsertColumn(hListView,5,&lvc);
			lvc.pszText="f";
			ListView_InsertColumn(hListView,6,&lvc);
			lvc.pszText="p";
			ListView_InsertColumn(hListView,7,&lvc);
			lvc.cx=65;//�п�
			lvc.pszText="color";
			ListView_InsertColumn(hListView,8,&lvc);
			//---------------------------------------
			//�����1��
			litem.iItem=0;//��itemcounts��
			litem.iSubItem=0;
			litem.pszText="1";//���ı�
			ListView_InsertItem(hListView,&litem);//������

			//�������ֵ
			ListView_SetItemText(hListView,0,1,"0");
			ListView_SetItemText(hListView,0,2,"0");
			ListView_SetItemText(hListView,0,3,"0");
			ListView_SetItemText(hListView,0,4,"0");
			ListView_SetItemText(hListView,0,5,"0");
			ListView_SetItemText(hListView,0,6,"0");
			ListView_SetItemText(hListView,0,7,"0");
			ListView_SetItemText(hListView,0,8,"0");
			//=================================================
			//=================================================

			HWND hN=GetDlgItem(hWnd,IDC_T4_N);//N
			HWND hXmax=GetDlgItem(hWnd,IDC_T4_XMAX);//xmax
			HWND hYmax=GetDlgItem(hWnd,IDC_T4_YMAX);//ymax
			HWND hX0=GetDlgItem(hWnd,IDC_T4_X0);//x0
			HWND hY0=GetDlgItem(hWnd,IDC_T4_Y0);//y0

			//���ñ༭���ʼֵ
			SetWindowText(hXmax,"5");
			SetWindowText(hYmax,"4");
			SetWindowText(hX0,"0");
			SetWindowText(hY0,"0");
			SetWindowText(hN,"10000");

			//�����б��,�õ�����ifs
			char inibuffer[1000];
			//�õ����н�����inibuffer,ÿ��������0����������2��0����
			if(GetPrivateProfileSectionNames(inibuffer,1000,inistr.c_str()))//�����ļ��ɹ�
			{
				char *p=inibuffer;
			
				while(*(p))
				{
					SendDlgItemMessage(hWnd,IDC_T4_STOCKLIST,LB_ADDSTRING,0,(LPARAM)p);//���浽�б��
					p+=strlen(p)+1;//��������һ������
					
				}
			}

			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID= LOWORD(wParam);//�ؼ�ID
			HWND ControlHandle=(HWND)lParam;//�ؼ����
			int ControlNotify=HIWORD(wParam);//�ؼ�֪ͨ��

			if(ControlID==IDC_T4_SETVALUE)//����ֵ
			{
				int curitem= ListView_GetNextItem(hListView,-1,LVNI_SELECTED);//�õ���ǰ��
				if(curitem!=-1)
				{
					DialogBoxParam(MAIN_INSTANCE,MAKEINTRESOURCE\
					(IDD_T4_SETVALUE_DLG),hWnd,Dlg_T4_SetValue,0);//����ֵѡ��Ի���
				}
				else
				{
					MessageBox(MAIN_HANDLE,"��ѡ����Ҫ�޸���!","����",MB_OK);
				
				}
			
			}
			else if(ControlID==IDC_T4_ADD_LINE)//������
			{
				int itemcounts=ListView_GetItemCount(hListView);//�õ���ǰ����

				//������
				litem.iItem=itemcounts;//��itemcounts��
				litem.iSubItem=0;
				char buffer[256];
				sprintf(buffer,"%d",itemcounts+1);
				litem.pszText=buffer;//���ı�
				ListView_InsertItem(hListView,&litem);//������

				//�������ֵ
				ListView_SetItemText(hListView,itemcounts,1,"0");
				ListView_SetItemText(hListView,itemcounts,2,"0");
				ListView_SetItemText(hListView,itemcounts,3,"0");
				ListView_SetItemText(hListView,itemcounts,4,"0");
				ListView_SetItemText(hListView,itemcounts,5,"0");
				ListView_SetItemText(hListView,itemcounts,6,"0");
				ListView_SetItemText(hListView,itemcounts,7,"0");
				ListView_SetItemText(hListView,itemcounts,8,"0");
			
			}
			else if(ControlID==IDC_T4_DELETE_LINE)//ɾ������ѡ����
			{
				while(1)
				{
					int curitem= ListView_GetNextItem(hListView,-1,LVNI_SELECTED);//�õ���ǰ��
					ListView_DeleteItem(hListView,curitem);//ɾ����
					if(curitem<0)break;
				}

				//��������
				int itemcounts=ListView_GetItemCount(hListView);//�õ���ǰ����
				char buffer[10];
				for(i=1;i<=itemcounts;i++)
				{
					sprintf(buffer,"%d",i);
					ListView_SetItemText(hListView,i-1,0,buffer);
				}
			
			}
			else if(ControlID==IDC_T4_CLEAN_ALLLINE)//���ListView
			{
				ListView_DeleteAllItems(hListView);
			}
			else if(ControlID==IDC_T4_SAVE)//����IFS��
			{
				int itemcounts=ListView_GetItemCount(hListView);//�õ���ǰ����
				if(itemcounts>0)
				{
					DialogBoxParam(MAIN_INSTANCE,MAKEINTRESOURCE\
						(IDD_T4_SETNAME_DLG),hWnd,Dlg_T4_SetName,0);//�������ƶԻ���
				}
				else
				{
					MessageBox(MAIN_HANDLE,"û����Ҫ�����IFS��!","����",MB_OK);
				
				}
				

			}
			else if(ControlID==IDC_T4_STOCKLIST)//��Ϊ�б��
			{
				if(ControlNotify==LBN_DBLCLK)//��Ϊ˫��
				{
					char name[260];
					int sel=SendMessage(ControlHandle,LB_GETCURSEL,0,0);//�õ���ǰ����
					SendMessage(ControlHandle,LB_GETTEXT,(WPARAM)sel,(LPARAM)name);//�õ��б����ַ���

					//�õ��ؼ����
					HWND hN=GetDlgItem(hWnd,IDC_T4_N);
					HWND hXmax=GetDlgItem(hWnd,IDC_T4_XMAX);
					HWND hYmax=GetDlgItem(hWnd,IDC_T4_YMAX);
					HWND hX0=GetDlgItem(hWnd,IDC_T4_X0);
					HWND hY0=GetDlgItem(hWnd,IDC_T4_Y0);
					HWND hListView=GetDlgItem(hWnd,IDC_T4_LISTVIEW);

					//��ini�ļ��õ���Ӧ�������ifs��
					char str[256];
					GetPrivateProfileString(name,"N","0",str,256,inistr.c_str());
					SetWindowText(hN,str);
					GetPrivateProfileString(name,"Xmax","0",str,256,inistr.c_str());
					SetWindowText(hXmax,str);
					GetPrivateProfileString(name,"Ymax","0",str,256,inistr.c_str());
					SetWindowText(hYmax,str);
					GetPrivateProfileString(name,"X0","0",str,256,inistr.c_str());
					SetWindowText(hX0,str);
					GetPrivateProfileString(name,"Y0","0",str,256,inistr.c_str());
					SetWindowText(hY0,str);
					//�õ�ifs
					long counts,i,j;
					char ifsname[20];

					GetPrivateProfileString(name,"Counts","0",str,256,inistr.c_str());
					counts=atol(str);//�õ�ifs����

					ListView_DeleteAllItems(hListView);//���ListView
					
					//��������ListView
					for(i=0;i<counts;i++)
					{
						//�����i��
						litem.iItem=i;
						litem.iSubItem=0;
						char buf[20];
						sprintf(buf,"%d",i+1);
						litem.pszText=buf;//���ı�(���)
						ListView_InsertItem(hListView,&litem);

						sprintf(ifsname,"IFS_%ld",i);//�õ�ifs������
						GetPrivateProfileString(name,ifsname,"0",str,256,inistr.c_str());

						
						char *p=str;
						for(j=1;j<=8;j++)
						{
							//�ֽ�ifs������
							string ps;
							while((*p))
							{
								if((*p)!='|')
								{
									ps+=*p;
									p++;
								}
								else
								{
									p++;
									break;
								
								}
							}
							//���ListView
							if(ps.length()>0)
							{
								ListView_SetItemText(hListView,i,j,(char *)ps.c_str());
							}
							else
							{
								ListView_SetItemText(hListView,i,j,"0");
							}
						
						
						}
					
					}
				}
		
			}
			else if(ControlID==IDC_T4_CLEAN_LIST)//����б���ini�ļ�
			{
				int selcounts=SendDlgItemMessage(hWnd,IDC_T4_STOCKLIST,LB_GETCOUNT,0,0);//�õ��б��������
				for(int i=0;i<selcounts;i++)//ѭ���������С��
				{
					char str[260];
					SendDlgItemMessage(hWnd,IDC_T4_STOCKLIST,LB_GETTEXT,(WPARAM)i,(LPARAM)str);
					WritePrivateProfileString(str,NULL,NULL,inistr.c_str());
				
				}
				SendDlgItemMessage(hWnd,IDC_T4_STOCKLIST,LB_RESETCONTENT,0,0);//���б�������
			
			
			}
			else if(ControlID==IDC_T4_DELETE_LIST)//ɾ���б��ˢ��ini�ļ�
			{
				int sel=SendDlgItemMessage(hWnd,IDC_T4_STOCKLIST,LB_GETCURSEL,0,0);//�õ���ǰ����

				if(sel>=0)
				{
					//�������ļ������
					char str[260];
					SendDlgItemMessage(hWnd,IDC_T4_STOCKLIST,LB_GETTEXT,(WPARAM)sel,(LPARAM)str);
					WritePrivateProfileString(str,0,0,inistr.c_str());
	
					SendDlgItemMessage(hWnd,IDC_T4_STOCKLIST,LB_DELETESTRING,(WPARAM)sel,0);//���б�������
				}
			}
			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}

//Tab�ӶԻ���T10��Ϣ����
BOOL WINAPI DlgT10(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			return TRUE;
		}
		break;
	case WM_CLOSE://��������Ϣ
		{
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}

//���Ի�����Ϣ����
BOOL WINAPI DlgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			
			//����TAB�ؼ�
			//================================================
			RECT r;
			GetClientRect(hWnd,&r);

			/*hTab=CreateWindowEx(0,WC_TABCONTROL,"",\
			TCS_TABS|TCS_FOCUSNEVER|TCS_BOTTOM|WS_VISIBLE|WS_CHILD|WS_TABSTOP,
			r.left,r.top,r.right-r.left-90,r.bottom-r.top,\
			hWnd,(HMENU)MAKEINTRESOURCE(IDC_TAB), //�ؼ�ID
			MAIN_INSTANCE, //ʵ�����
			NULL);*/
			hTab=GetDlgItem(hWnd,IDC_TAB);
			//����TAB�ؼ���
			TCITEM tb;
			tb.mask = TCIF_TEXT;
			tb.cchTextMax = 256;
			tb.pszText="����/������";
			TabCtrl_InsertItem(hTab,0,&tb);
			tb.pszText="��Ԫ������";
			TabCtrl_InsertItem(hTab,1,&tb);
			tb.pszText="LS�ķ�����";
			TabCtrl_InsertItem(hTab,2,&tb);
			tb.pszText="��ѧ����";
			TabCtrl_InsertItem(hTab,3,&tb);
			tb.pszText="IFS����任";
			TabCtrl_InsertItem(hTab,4,&tb);
			tb.pszText="���ζ���";
			TabCtrl_InsertItem(hTab,5,&tb);
			TabCtrl_SetCurSel(hTab,0);
	
			//=================================================
			
			//=================================================
			//����TAB�ӶԻ���
			RECT tr;
			GetClientRect(hTab,&tr);
			hT0=CreateDialogParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_T0),hTab,DlgT0,0);
			hT1=CreateDialogParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_T1),hTab,DlgT1,0);
			hT2=CreateDialogParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_T2),hTab,DlgT2,0);
			hT3=CreateDialogParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_T3),hTab,DlgT3,0);
			hT4=CreateDialogParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_T4),hTab,DlgT4,0);
			hT10=CreateDialogParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_T10),hTab,DlgT10,0);
			MoveWindow(hT0,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
			MoveWindow(hT1,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
			MoveWindow(hT2,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
			MoveWindow(hT3,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
			MoveWindow(hT4,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
			MoveWindow(hT10,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
			ShowWindow(hT0,TRUE);
			ShowWindow(hT1,FALSE);
			ShowWindow(hT2,FALSE);
			ShowWindow(hT3,FALSE);
			ShowWindow(hT4,FALSE);
			ShowWindow(hT10,FALSE);
			
			//=================================================

			//=================================================

			//Ϊ�Ի�����ز˵�
			HMENU hmenu=LoadMenu(MAIN_INSTANCE,MAKEINTRESOURCE(IDR_MENU));
			SetMenu(hWnd,hmenu);

			//�������Ի���λ��
			GetWindowRect(hWnd,&r);
			MoveWindow(hWnd,100,400,r.right-r.left,r.bottom-r.top+25,TRUE);

			//����ͼ��
			//HICON hicon=LoadIcon(NULL,IDI_APPLICATION);//Ӧ�ó���ͼ��
			HICON hicon=LoadIcon(MAIN_INSTANCE,MAKEINTRESOURCE(IDI_APP));//Ӧ�ó���ͼ��
			SendMessage(hWnd,WM_SETICON,ICON_BIG,(LPARAM)hicon);

			return TRUE;
		}
		break;
	case WM_COMMAND:
		{
			int ControlID=LOWORD(wParam);//�ؼ�ID

			if(ControlID==IDC_DRAW)//��Ϊ��ͼ��ť
			{
				int sel;
				sel=TabCtrl_GetCurSel(hTab);
				switch(sel)//�����߳�
				{
					case 0://����ƽ�漯��
					{
						if(StartDrawByDLL)//��DLL�������سɹ�
						{
							//=====================================
							
							GetClientRect(MAIN_HANDLE,&MyRect);//ֻ��ԭͼ(������)
							
							//���ݲ���
							PARAMS param;
							param.main_handle=MAIN_HANDLE;
							param.main_dlg=hWnd;
							param.hmemdc=hMemDC;
							param.hTabDlg=hT0;
							param.attr=PARAMS_ATTR_T0;
							param.myrect=MyRect;
							param.isfangsuo=IsFangSuo;
							//���п�湫ʽƥ�䣬�򻭿��ͼ
							if(StartDrawByDLL((LPVOID)&param))
							{
							}
							else
							{
								hDrawThreadT0=CreateThread(NULL,0,OnDrawT0,0,0,0);
								CloseHandle(hDrawThreadT0);
							}
					
						}
						else
						{
							hDrawThreadT0=CreateThread(NULL,0,OnDrawT0,0,0,0);
							CloseHandle(hDrawThreadT0);
						}
						
					}
					break;
					case 1:
					{
						hDrawThreadT1=CreateThread(NULL,0,OnDrawT1,0,0,0);
						CloseHandle(hDrawThreadT1);//��Newton/Nova ����
					}
					break;
					case 2:
					{
						hDrawThreadT2=CreateThread(NULL,0,OnDrawT2,0,0,0);
						CloseHandle(hDrawThreadT2);//��LS�ķ�����

					}
					break;
					case 3:
					{
						hDrawThreadT3=CreateThread(NULL,0,OnDrawT3,0,0,0);
						CloseHandle(hDrawThreadT3);//�����Ⱥ�������

					}
					break;
					case 4:
					{
						hDrawThreadT4=CreateThread(NULL,0,OnDrawT4,0,0,0);
						CloseHandle(hDrawThreadT4);//�����Ⱥ�������

					}
					break;
					case 5:
					{
						hDrawThreadT10=CreateThread(NULL,0,OnDrawT10,0,0,0);
						CloseHandle(hDrawThreadT10);//�����Ⱥ�������

					}
					break;

					default:
						break;
				
				}

				//���û�ͼ��ť
				HWND hDraw=GetDlgItem(hWnd,IDC_DRAW);
				EnableWindow(hDraw,FALSE);

				//���ò�����ť
				HWND hClear=GetDlgItem(hWnd,IDC_CLEAR);
				EnableWindow(hClear,FALSE);


				return TRUE;
			}
			else if(ControlID==IDC_CLEAR)//��Ϊ������ť
			{
				RECT r;
				GetClientRect(MAIN_HANDLE,&r);//�õ����ڿͻ��˴�С

				HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC

				DeleteDC(hMemDC);//�����ڴ����DC
				hMemDC=CreateCompatibleDC(hDC);
				
				HBITMAP bm;//�����ڴ����λͼ,����ֻ���ǵ�ɫ����DC
				bm=CreateCompatibleBitmap(hDC,r.right-r.left,r.bottom-r.top);
				SelectObject(hMemDC,bm);

				HBRUSH hbrush=CreateSolidBrush(RGB(0,0,0));
				FillRect(hMemDC,&r,hbrush);//������ɫ

				//�ͷ���Դ
				if(bm)DeleteObject(bm);
				ReleaseDC(MAIN_HANDLE,hDC);
				
				//�ػ洰��
				InvalidateRect(MAIN_HANDLE,&r,FALSE);

				return TRUE;
			
			}
			else if(ControlID==IDC_EXIT)//��Ϊ�˳���ť
			{
				DestroyWindow(hWnd);
				DestroyWindow(MAIN_HANDLE);

				return TRUE;
			}
			else if(ControlID==IDC_BIG)//��Ϊ�Ŵ�ť
			{
				if(!IsFangSuo)//���÷Ŵ�
				{
					IsFangSuo=TRUE;
					GetClientRect(MAIN_HANDLE,&MyRect);
					//����ʮ���ι��
					HCURSOR hcursor=LoadCursor(MAIN_INSTANCE,MAKEINTRESOURCE(IDC_MYCROSS));
					SetClassLong(MAIN_HANDLE,GCL_HCURSOR,(LONG)hcursor);

					SetWindowText((HWND)lParam,"��ԭ");

					//���û�ͼ��ť
					HWND hDraw=GetDlgItem(hWnd,IDC_DRAW);
					EnableWindow(hDraw,FALSE);

					
				}
				else//��ԭ�Ŵ�
				{
					IsFangSuo=FALSE;
					//���ü�ͷ���
					HCURSOR hcursor=LoadCursor(MAIN_INSTANCE,MAKEINTRESOURCE(IDC_MYARROW));
					SetClassLong(MAIN_HANDLE,GCL_HCURSOR,(LONG)hcursor);

					SetWindowText((HWND)lParam,"�Ŵ�");

					//�����û�ͼ��ť
					HWND hDraw=GetDlgItem(hWnd,IDC_DRAW);
					EnableWindow(hDraw,TRUE);

					//�ػ�ԭͼ
					//======================================
					if(StartDrawByDLL)//��DLL�������سɹ�
					{
						//=====================================
						GetClientRect(MAIN_HANDLE,&MyRect);
						//���ݲ���
						PARAMS param;
						param.main_handle=MAIN_HANDLE;
						param.main_dlg=hWnd;
						param.hmemdc=hMemDC;
						param.hTabDlg=hT0;
						param.attr=PARAMS_ATTR_T0;
						param.myrect=MyRect;
						param.isfangsuo=IsFangSuo;
						//���п�湫ʽƥ�䣬�򻭿��ͼ
						if(StartDrawByDLL((LPVOID)&param))
						{
						}
						else
						{
							hDrawThreadT0=CreateThread(NULL,0,OnDrawT0,0,0,0);
							CloseHandle(hDrawThreadT0);
						}
					
					}
					else
					{
						hDrawThreadT0=CreateThread(NULL,0,OnDrawT0,0,0,0);
						CloseHandle(hDrawThreadT0);
					}
					//=========================================================
				
				}

				return TRUE;
			}
			else if(ControlID==IDC_HIDE)//��Ϊ���ذ�ť
			{
				ShowWindow(hWnd,FALSE);
			
				return TRUE;
			}
			else if(ControlID==IDM_ABOUT)//��Ϊ���ڰ�ť
			{
				DialogBoxParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_ABOUT),hWnd,DlgAboutProc,0);
			
				return TRUE;
			}
			else if(ControlID==IDM_BACKGROUND_COLOR)//��Ϊ���ñ�����ť
			{
				//��ͨ����ɫ�Ի���
				COLORREF cusColor[16];

				CHOOSECOLOR lcc;
				lcc.lStructSize=sizeof(CHOOSECOLOR);
				lcc.hwndOwner=MAIN_HANDLE;
				lcc.hInstance=NULL;
				lcc.rgbResult=RGB(0,0,0);
				lcc.lpCustColors=cusColor; 
				lcc.Flags=CC_RGBINIT|CC_FULLOPEN| CC_ANYCOLOR;
				lcc.lCustData=NULL;
				lcc.lpfnHook=NULL;
				lcc.lpTemplateName=NULL;

				if(ChooseColor(&lcc))
				{
					/*int r,g,b;
					b=255&(lcc.rgbResult>>16);
					g=255&(lcc.rgbResult>>8);
					r=255&lcc.rgbResult;*/
					BackGroundColor=lcc.rgbResult;//���汳��ɫ
				
				}
				
			
				return TRUE;
			}
			else if(ControlID==IDC_SCREEN)//��Ϊȫ����ť
			{
				LONG style=(LONG)GetWindowLong(MAIN_HANDLE,GWL_STYLE);//�õ����ڷ��
				if(!IsScreen)
				{
					style &= ~(WS_DLGFRAME | WS_THICKFRAME); //��Ϊȫ�����
					SetWindowLong(MAIN_HANDLE,GWL_STYLE, style); 
					ShowWindow(MAIN_HANDLE,SW_SHOWMAXIMIZED); //�����ʾ
					RECT rect; 
					GetWindowRect(MAIN_HANDLE,&rect); 
					SetWindowPos(MAIN_HANDLE,HWND_NOTOPMOST,rect.left-1, rect.top-1, \
						rect.right-rect.left + 3, rect.bottom-rect.top + 3, SWP_FRAMECHANGED); //�����߿�

					SetWindowText((HWND)lParam,"����");
					IsScreen=TRUE;

					
				}
				else
				{
					style |= WS_DLGFRAME | WS_THICKFRAME; //��ԭ���ڷ��
					SetWindowLong(MAIN_HANDLE, GWL_STYLE, style); 
					ShowWindow(MAIN_HANDLE,SW_NORMAL); //������ʾ
					SetWindowText((HWND)lParam,"ȫ��");
					IsScreen=FALSE;
				}
				//���´�������DC��������С
				HDC hDC=GetDC(MAIN_HANDLE);//�õ���ĻDC
				if(hMemDC)DeleteDC(hMemDC);//�����ڴ����DC
				hMemDC=CreateCompatibleDC(hDC);
				RECT r;
				GetClientRect(MAIN_HANDLE,&r);
				HBITMAP bm;//�����ڴ����λͼ,����ֻ���ǵ�ɫ����DC
				bm=CreateCompatibleBitmap(hDC,r.right-r.left,r.bottom-r.top);
				SelectObject(hMemDC,bm);
				ReleaseDC(MAIN_HANDLE,hDC);


				return TRUE;
			}
			else return FALSE;

		}
		break;
	case WM_NOTIFY://�ؼ�֪ͨ��Ϣ
		{
			int ControlID= LOWORD(wParam);//�ؼ�ID
			LPNMHDR pNmhdr=(LPNMHDR)lParam;//NMHDR�ṹ��(�ؼ���Ϣͷ)
			if(ControlID==IDC_TAB)
			{
				if(pNmhdr->code==TCN_SELCHANGE)//��ΪTABѡ��ı䣬��ʾ��ͬ��ǩҳ(�Ի���)
				{
					RECT tr;
					GetClientRect(hTab,&tr);
					int sel;
					sel=TabCtrl_GetCurSel(hTab);
					MoveWindow(hT0,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
					MoveWindow(hT1,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
					MoveWindow(hT2,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
					MoveWindow(hT3,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
					MoveWindow(hT4,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
					MoveWindow(hT10,tr.left+5,tr.top+5,tr.right-tr.left-10,tr.bottom-tr.top-30,TRUE);
					
					switch(sel)
					{
						case 0:
						{
						ShowWindow(hT1,FALSE);
						ShowWindow(hT2,FALSE);
						ShowWindow(hT3,FALSE);
						ShowWindow(hT4,FALSE);
						ShowWindow(hT10,FALSE);
						ShowWindow(hT0,TRUE);

						//ʹ�÷Ŵ�ť
						HWND hFangDa=GetDlgItem(hWnd,IDC_BIG);
						EnableWindow(hFangDa,TRUE);

						}

						break;
					case 1:
						{
						ShowWindow(hT0,FALSE);
						ShowWindow(hT2,FALSE);
						ShowWindow(hT3,FALSE);
						ShowWindow(hT4,FALSE);
						ShowWindow(hT10,FALSE);
						ShowWindow(hT1,TRUE);

						//ʹ�÷Ŵ�ť
						HWND hFangDa=GetDlgItem(hWnd,IDC_BIG);
						EnableWindow(hFangDa,TRUE);

						}

						break;
					case 2:
						{
						ShowWindow(hT0,FALSE);
						ShowWindow(hT1,FALSE);
						ShowWindow(hT3,FALSE);
						ShowWindow(hT4,FALSE);
						ShowWindow(hT10,FALSE);
						ShowWindow(hT2,TRUE);

						//���÷Ŵ�ť
						HWND hFangDa=GetDlgItem(hWnd,IDC_BIG);
						EnableWindow(hFangDa,FALSE);
						}

						break;
					case 3:
						{
						ShowWindow(hT0,FALSE);
						ShowWindow(hT1,FALSE);
						ShowWindow(hT2,FALSE);
						ShowWindow(hT4,FALSE);
						ShowWindow(hT10,FALSE);
						ShowWindow(hT3,TRUE);

						//���÷Ŵ�ť
						HWND hFangDa=GetDlgItem(hWnd,IDC_BIG);
						EnableWindow(hFangDa,FALSE);

						}

						break;
					case 4:
						{
						ShowWindow(hT0,FALSE);
						ShowWindow(hT1,FALSE);
						ShowWindow(hT3,FALSE);
						ShowWindow(hT2,FALSE);
						ShowWindow(hT10,FALSE);
						ShowWindow(hT4,TRUE);

						//���÷Ŵ�ť
						HWND hFangDa=GetDlgItem(hWnd,IDC_BIG);
						EnableWindow(hFangDa,FALSE);
						}
						break;
					case 5:
						{
						ShowWindow(hT0,FALSE);
						ShowWindow(hT1,FALSE);
						ShowWindow(hT3,FALSE);
						ShowWindow(hT2,FALSE);
						ShowWindow(hT4,FALSE);
						ShowWindow(hT10,TRUE);
						
						//���÷Ŵ�ť
						HWND hFangDa=GetDlgItem(hWnd,IDC_BIG);
						EnableWindow(hFangDa,FALSE);
						}
						break;

					default:
						break;
					}

				}
				
			}
	
		}
		break;
	case WM_CLOSE:
		{
			
			DestroyWindow(hWnd);
			DestroyWindow(MAIN_HANDLE);
			return TRUE;
		}
		break;
	case WM_DESTROY://���ٴ�����Ϣ
		{
			PostQuitMessage(0);
			return TRUE;
		}
		break;
	default:
		return FALSE;
	}
	return FALSE;
	
}
//���ڴ�����
//------------------------------------------------------------
LRESULT CALLBACK WinProc(HWND hwnd,UINT msg,WPARAM wparam,LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	
	switch(msg)
	{
	case WM_KEYDOWN://������Ϣ
		{
			return 0;
		}
		break;
	case WM_CHAR://�ַ���Ϣ
		{
			return 0;	
		}
		break;
	case WM_LBUTTONDOWN://������������Ϣ
		{
			if(!IsCapture)
			{
				//�������ԭ���������
				OldPoint.x=LOWORD(lparam);
				OldPoint.y=HIWORD(lparam);

				SetCapture(hwnd);//�������
				IsCapture=TRUE;

				if(IsFangSuo)
				{
					//���ǲ���ԭ����
					MyOldRect.left=MyRect.left;
					MyOldRect.right=MyRect.right;
					MyOldRect.top=MyRect.top;
					MyOldRect.bottom=MyRect.bottom;
			
				}
			
			}
			return 0;
		}
		break;
	case WM_LBUTTONUP://������������Ϣ
		{
			POINT Point;
			
			if(IsCapture)
			{
				Point.x=LOWORD(lparam);
				Point.y=HIWORD(lparam);

				ReleaseCapture();//ȡ������
				IsCapture=FALSE;

				HDC hDC=GetDC(hwnd);

				//��ʾ��������
				char showrect[20];
				sprintf(showrect,"���λ��� 1.0"\
					,MyRect.left,MyRect.top,MyRect.right,MyRect.bottom);
				SetWindowText(hwnd,showrect);

			}
			return 0;
		}
		break;
	case WM_LBUTTONDBLCLK://������˫����Ϣ
		{
			POINT Point;
			Point.x=LOWORD(lparam);
			Point.y=HIWORD(lparam);

			if(IsFangSuo)
			{

				if(Point.x<MyRect.left||Point.x>MyRect.right||Point.y<MyRect.top||Point.y>MyRect.bottom)
					return 0;
				else
				{
					MyRect.left+=1;
					MyRect.top+=1;
					MyRect.right-=1;
					MyRect.bottom-=1;
				
				}
				
				if(StartDrawByDLL)//��DLL�������سɹ�
				{
					//=====================================
					//���ݲ���
					PARAMS param;
					param.myrect=MyRect;
					param.main_handle=hwnd;
					param.main_dlg=MAIN_DIALOG;
					param.hmemdc=hMemDC;
					param.hTabDlg=hT0;
					param.attr=PARAMS_ATTR_T0;
					param.isfangsuo=IsFangSuo;
					//���п�湫ʽƥ�䣬�򻭿��ͼ
					if(StartDrawByDLL((LPVOID)&param))
					{
						//�Ŵ�������������
						MyRect.left=0;
						MyRect.top=0;
						MyRect.right=0;
						MyRect.bottom=0;
					}
					else
					{
						hDrawThreadT0=CreateThread(NULL,0,OnDrawT0,0,0,0);
						CloseHandle(hDrawThreadT0);
					}
					
				}
				else
				{
					hDrawThreadT0=CreateThread(NULL,0,OnDrawT0,0,0,0);
					CloseHandle(hDrawThreadT0);
				}
				
			}
			return 0;
		}
		break;
	case WM_MOUSEMOVE://����ƶ���Ϣ
		{
			POINT Point;

			Point.x=LOWORD(lparam);
			Point.y=HIWORD(lparam);

			if(IsCapture)
			{
				if(IsFangSuo)
				{
					//�õ���������
					if(Point.x>=OldPoint.x)
					{
						MyRect.left=OldPoint.x;
						MyRect.right=Point.x;
					}
					else if(Point.x<OldPoint.x)
					{
						MyRect.left=Point.x;
						MyRect.right=OldPoint.x;
				
					}

					if(Point.y>=OldPoint.y)
					{
						MyRect.top=OldPoint.y;
						MyRect.bottom=Point.y;
					}
					else
					{
						MyRect.top=Point.y;
						MyRect.bottom=OldPoint.y;
				
					}
					//��С˫�����������
					if(abs(Point.x-OldPoint.x)<=1&&abs(Point.y-OldPoint.y)<=1)
					{
						GetClientRect(hwnd,&MyRect);
					}
				
					HDC hDC=GetDC(hwnd);

					//������DC�л����
					int oldrop2=SetROP2(hDC,R2_NOT);//���÷�ɫ
					HBRUSH hOldBrush=(HBRUSH)SelectObject(hDC,(HBRUSH)GetStockObject(NULL_BRUSH));//����͸����ˢ
					Rectangle(hDC,MyOldRect.left,MyOldRect.top,MyOldRect.right,MyOldRect.bottom);//����ԭ���ο�
					Rectangle(hDC,MyRect.left,MyRect.top,MyRect.right,MyRect.bottom);//�����ο�
					SetROP2(hDC,oldrop2);//���ROP2��
					SelectObject(hDC,hOldBrush);//ѡ��ԭ��ˢ
					
					//�ڼ���DC�л����
					int oldmemrop2=SetROP2(hMemDC,R2_NOT);//���÷�ɫ
					HBRUSH hOldMemBrush=(HBRUSH)SelectObject(hMemDC,(HBRUSH)GetStockObject(NULL_BRUSH));//����͸����ˢ
					Rectangle(hMemDC,MyOldRect.left,MyOldRect.top,MyOldRect.right,MyOldRect.bottom);//����ԭ���ο�
					Rectangle(hMemDC,MyRect.left,MyRect.top,MyRect.right,MyRect.bottom);//�����ο�
					SetROP2(hMemDC,oldmemrop2);//���ROP2��
					SelectObject(hMemDC,hOldMemBrush);//ѡ��ԭ��ˢ

					ReleaseDC(hwnd,hDC);

					MyOldRect=MyRect;//����ԭ����

					//��ʾ��������
					char showrect[20];
					sprintf(showrect,"���λ��� 1.0 (x1:%d y1:%d x2:%d y2:%d)"\
						,MyRect.left,MyRect.top,MyRect.right,MyRect.bottom);
					SetWindowText(hwnd,showrect);
					}

					

			}
			else
			{
				/*if(IsFangSuo)
				{
					if(Point.x<MyRect.left&&Point.x>MyRect.right&&Point.y<MyRect.top&&Point.y>MyRect.bottom)
					{
						//���ü�ͷ���
						//HCURSOR hcursor=LoadCursor(MAIN_INSTANCE,MAKEINTRESOURCE(IDC_MYBIG));
						//SetClassLong(hwnd,GCL_HCURSOR,(LONG)hcursor);
				
					}
				}*/
			
			}
			return 0;
		}
		break;
	case WM_RBUTTONDOWN://����Ҽ���Ϣ
		{
			//��ʾ���Ի���
			ShowWindow(MAIN_DIALOG,TRUE);
			return 0;
		}
		break;
	case WM_CREATE://���ڴ�����Ϣ
		{
			//������ģ̬�Ի���
			MAIN_DIALOG=CreateDialogParam(MAIN_INSTANCE,MAKEINTRESOURCE(IDD_MAINDLG),hwnd,DlgProc,0);

			//���ü�ͷ���
			//HCURSOR hcursor=LoadCursor(MAIN_INSTANCE,MAKEINTRESOURCE(IDC_MYARROW));
			//SetClassLong(hwnd,GCL_HCURSOR,(LONG)hcursor);

			//��������DC
			HDC hDC=GetDC(hwnd);//�õ���ĻDC
			if(hMemDC)DeleteDC(hMemDC);//�����ڴ����DC
			hMemDC=CreateCompatibleDC(hDC);
			RECT r;
			GetClientRect(hwnd,&r);
			HBITMAP bm;//�����ڴ����λͼ,����ֻ���ǵ�ɫ����DC
			bm=CreateCompatibleBitmap(hDC,r.right-r.left,r.bottom-r.top);
			SelectObject(hMemDC,bm);
			ReleaseDC(hwnd,hDC);

			//��̬����stock.dll��̬���ӿ�
			hStockDll=LoadLibrary("stock.dll");
			if(hStockDll)
			{
				//���غ���
				StartDrawByDLL=(StockDllProc)GetProcAddress(hStockDll,"StartDrawByDLL");//�����ͼ
				GetAllGongShiFromDLL=(StockDllProc)GetProcAddress(hStockDll,"GetAllGongShi");//�õ����п�湫ʽ
			}

			return 0;
		}
		break;
	case WM_PAINT://�ػ���Ϣ
		{
			RECT r;
			GetClientRect(hwnd,&r);

			hdc=BeginPaint(hwnd,&ps);

			BitBlt(hdc,r.left,r.top,r.right-r.left,r.bottom-r.top,hMemDC,0,0,SRCCOPY);

			EndPaint(hwnd,&ps);
			return 0;
		}
		break;		
	case WM_CLOSE://�رմ�����Ϣ
		{
			
			DestroyWindow(hwnd);
			return 0;
		}
		break;
	case WM_DESTROY://���ٴ�����Ϣ
		{
			if(hStockDll)
			{
				FreeLibrary(hStockDll);
			}
			PostQuitMessage(0);
			return 0;
			
		}
		break;
		//������Ϣ����
	default: return DefWindowProc(hwnd, msg, wparam, lparam);
	
	}

}


//�������������
//-------------------------------------------------------------
int WINAPI WinMain(
  HINSTANCE hInstance,      // ��ǰʵ�����
  HINSTANCE hPrevInstance,  // ��ǰʵ�����
  LPSTR lpCmdLine,          // �����в���
  int nCmdShow              // ��ʾ״̬
)
{
	WNDCLASSEX winclass;
	HWND hwnd;
	MSG msg;

	winclass.cbSize=sizeof(WNDCLASSEX);//��ʼ���ṹ��С��ֻ��WNDCLASSEX�У�WNDCLASSû��
	winclass.style=CS_HREDRAW | CS_VREDRAW |CS_DBLCLKS | CS_OWNDC;//���ڷ��
	winclass.lpfnWndProc=WinProc;//���ڹ��̴�����
	winclass.cbClsExtra=0;//��չ���
	winclass.cbWndExtra=0;//��չ���
	winclass.hInstance=hInstance;//����ʵ�����
	//winclass.hIcon=LoadIcon(NULL,IDI_APPLICATION);//Ӧ�ó���ͼ�꣬ϵͳ�Դ�
	winclass.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_APP));//���������Ӧ�ó���ͼ��
	winclass.hCursor=LoadCursor(NULL,IDC_ARROW);//����ͷ���
	winclass.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);//���ڱ���
	winclass.lpszMenuName=NULL;//���ڲ˵�
	winclass.lpszClassName=WINDOW_CLASS_NAME;//��������
	//winclass.hIconSm=LoadIcon(NULL,IDI_APPLICATION);
	winclass.hIconSm=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_APP));

	MAIN_INSTANCE=hInstance;//���洰��ʵ�����

	//ע�ᴰ����
	if(!RegisterClassEx(&winclass))
		return 0;
	
	//��������
	if(!(hwnd=CreateWindowEx
		(NULL,//��չ���
		WINDOW_CLASS_NAME,//��������
		WINDOW_TITLE,//���ڱ���
		WS_OVERLAPPED | WS_SYSMENU | WS_VISIBLE,
		0,0,//�������Ͻ�x,yλ��
		WINDOW_WIDTH,WINDOW_HEIGHT,//���ڿ�ȣ��߶�
		NULL,//�����ھ��
		NULL,//�˵����
		hInstance,////����ʵ�����
		NULL)))//�������
		return 0;

	MAIN_HANDLE=hwnd;//���洰�ھ��
	MAIN_INSTANCE=hInstance;//���洰��ʵ�����

	INITCOMMONCONTROLSEX icex;
	RtlZeroMemory(&icex,sizeof(INITCOMMONCONTROLSEX));
	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC=ICC_WIN95_CLASSES;//ע�����пؼ�

	InitCommonControlsEx(&icex);//��ʼ����չ�ؼ�

	//��Ϣѭ��
	while(1)
	{
		if(PeekMessage(&msg,NULL,0,0,PM_REMOVE))//���ܲ���ȥ��Ϣ
		{
			if(msg.message==WM_QUIT)break;//��ΪWM_QUIT��Ϣ���˳�
			
			TranslateMessage(&msg);//ת����Ϣ
			DispatchMessage(&msg);//�ַ���Ϣ��ϵͳ
		}
		Sleep(1);
	}
	
	return msg.wParam;
}