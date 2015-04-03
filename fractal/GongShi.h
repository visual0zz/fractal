#include "iostream.h"
#include "ctype.h"
#include "stdio.h"
#include "math.h"
#include "stdlib.h"
#include "stack"
#include "vector"
#include "string"
#include "windows.h"

using std::string;
using std::stack;
using std::vector;


#define E 2.71828182845904//��Ȼ����
#define PI 3.14159265358979//Բ����

//����(���������)
#define ATTR_NUM 1//��ֵ
#define ATTR_SIGN 2//�����
#define ATTR_VAL 3//����

//���ȼ�(�����)
#define PRIORITY_ADD 1//���������
#define PRIORITY_SUB 1
#define PRIORITY_MUL 2
#define PRIORITY_DIV 2

//���Ǻ���
#define PRIORITY_SIN 2
#define PRIORITY_COS 2
#define PRIORITY_SINH 2
#define PRIORITY_COSH 2
#define PRIORITY_TAN 2
#define PRIORITY_TANH 2
#define PRIORITY_COT 2
#define PRIORITY_COTH 2
#define PRIORITY_SEC 2
#define PRIORITY_CSC 2
#define PRIORITY_SECH 2
#define PRIORITY_CSCH 2
#define PRIORITY_EXP 2

#define PRIORITY_ASIN 2
#define PRIORITY_ACOS 2
#define PRIORITY_ATAN 2
#define PRIORITY_ACOT 2

#define PRIORITY_LN 2//��Ȼ����
#define PRIORITY_LOG 2//��������
#define PRIORITY_MI 3//������
#define PRIORITY_LEFT 10//������
#define PRIORITY_RIGHT 10//������

#define PRIORITY_FU 11//����
#define PRIORITY_ZHENG 11//����

//���˳��(�����)
#define COMBINE_LEFT 1//����
#define COMBINE_RIGHT 2//�ҽ��

//��ʽ��
template<class T>
struct TOKEN
{
    int attr;//����(���������)
    struct 
    {
        int priority;//���ȼ�(�����)
        int count;//countԪ�����
        int combine;//���˳��
    }sign;
    T num;//��ֵ
    string str;//��ʽÿ���ַ���
};

//�ֽ⹫ʽ,gongshi:�ַ�����ʽ str_array:�ֽ��Ĺ�ʽ vals:ָ�������ַ���
template<class T>
int fenjie(char *gongshi,vector<TOKEN<T> > &str_array,vector<TOKEN<T> > &vals)
{
    char *p=gongshi;
    string s,str=gongshi;
    int t=0;
    if(t=str.find_first_of('='))p+=(t+1);

    if(*p=='+')//����ͷ��Ϊ+��Ϊ����
    {
        TOKEN<T> token;
        token.str="+";
        token.attr=ATTR_SIGN;
        token.sign.priority=PRIORITY_ZHENG;
        token.sign.count=1;
        token.sign.combine=COMBINE_LEFT;
        str_array.push_back(token);
        p++;
    }
    if(*p=='-')//����ͷ��Ϊ-��Ϊ����
    {
        TOKEN<T> token;
        token.str="-";
        token.attr=ATTR_SIGN;
        token.sign.priority=PRIORITY_FU;
        token.sign.count=1;
        token.sign.combine=COMBINE_LEFT;
        str_array.push_back(token);
        p++;
    }
    while(*p)
    {
        TOKEN<T> token;
        if(*p=='+')
        {
            if(*(p+1)=='-'||*(p+1)=='+')//�����滹��+��-��Ϊ����
            {
                token.str="+";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_ZHENG;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p++;
                continue;
            }
            int b=0;
            for(int i=0;i<vals.size();i++)//�жϺ�һ���ַ����Ƿ�Ϊ����
            {
                if(*(p+1+vals[i].str.length()-1))
                {
                    s.assign(p+1,vals[i].str.length());
                    if(s.compare(vals[i].str)==0)
                    {
                        b=1;
                        break;
                    }
                }
                
            }
            if(isdigit(*(p+1))||b)//����һ���ַ����Ǳ���������
            {
                b=0;
                for(int i=0;i<vals.size();i++)//�ж�ǰһ���ִ��Ƿ�Ϊ����
                {
                    if(*(p+vals[i].str.length()-2))
                    {
                        s.assign(p-1,vals[i].str.length());
                        if(s.compare(vals[i].str)==0)
                        {
                            b=1;
                            break;
                        }
                    }
                
                }
                //����һ���ַ����Ǳ�����������ǰһ���ִ���Ϊ���������ֺ������ţ���Ϊ����
                if(!isdigit(*(p-1))&&!b&&*(p-1)!=')')
                {
                    token.str="+";
                    token.attr=ATTR_SIGN;
                    token.sign.priority=PRIORITY_ZHENG;
                    token.sign.count=1;
                    token.sign.combine=COMBINE_LEFT;
                    str_array.push_back(token);
                    p++;
                }
                else//����Ϊ�Ӻ�
                {
                    token.str="+";
                    token.attr=ATTR_SIGN;
                    token.sign.priority=PRIORITY_ADD;
                    token.sign.count=2;
                    token.sign.combine=COMBINE_RIGHT;
                    str_array.push_back(token);
                    p++;

                }
            
            }
            else
            {
                token.str="+";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_ADD;
                token.sign.count=2;
                token.sign.combine=COMBINE_RIGHT;
                str_array.push_back(token);
                p++;
            }
            continue;
        }
        if(*p=='-')
        {
            if(*(p+1)=='-'||*(p+1)=='+')//�����滹��+��-��Ϊ����
            {
                token.str="-";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_FU;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p++;
                continue;
            }
            int b=0;
            for(int i=0;i<vals.size();i++)//�жϺ�һ���ַ����Ƿ�Ϊ����
            {
                if(*(p+1+vals[i].str.length()-1))
                {
                    s.assign(p+1,vals[i].str.length());
                    if(s.compare(vals[i].str)==0)
                    {
                        b=1;
                        break;
                    }
                }
                
            }
            if(isdigit(*(p+1))||b)//����һ���ַ����Ǳ���������
            {
                b=0;
                for(int i=0;i<vals.size();i++)//�ж�ǰһ���ִ��Ƿ�Ϊ����
                {
                    if(*(p+vals[i].str.length()-2))
                    {
                        s.assign(p-1,vals[i].str.length());
                        if(s.compare(vals[i].str)==0)
                        {
                            b=1;
                            break;
                        }
                    }
                
                }
                //����һ���ַ����Ǳ�����������ǰһ���ִ���Ϊ���������ֺ������ţ���Ϊ����
                if(!isdigit(*(p-1))&&!b&&*(p-1)!=')')
                {
                    token.str="-";
                    token.attr=ATTR_SIGN;
                    token.sign.priority=PRIORITY_FU;
                    token.sign.count=1;
                    token.sign.combine=COMBINE_LEFT;
                    str_array.push_back(token);
                    p++;
                }
                else//����Ϊ����
                {
                    token.str="-";
                    token.attr=ATTR_SIGN;
                    token.sign.priority=PRIORITY_SUB;
                    token.sign.count=2;
                    token.sign.combine=COMBINE_RIGHT;
                    str_array.push_back(token);
                    p++;

                }
            
            }
            else
            {
                token.str="-";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_SUB;
                token.sign.count=2;
                token.sign.combine=COMBINE_RIGHT;
                str_array.push_back(token);
                p++;
            }
            continue;
            
        }
        if(*p=='*')
        {
            token.str="*";
            token.attr=ATTR_SIGN;
            token.sign.priority=PRIORITY_MUL;
            token.sign.count=2;
            token.sign.combine=COMBINE_RIGHT;
            str_array.push_back(token);
            p++;
            continue;
            
        }
        if(*p=='/')
        {
            token.str="/";
            token.attr=ATTR_SIGN;
            token.sign.priority=PRIORITY_DIV;
            token.sign.count=2;
            token.sign.combine=COMBINE_RIGHT;
            str_array.push_back(token);
            p++;
            continue;
            
        }
        if(*p=='^')
        {
            token.str="^";
            token.attr=ATTR_SIGN;
            token.sign.priority=PRIORITY_MI;
            token.sign.count=2;
            token.sign.combine=COMBINE_LEFT;//����
            str_array.push_back(token);
            p++;
            continue;
            
        }
        if(*p=='(')
        {
            token.str="(";
            token.attr=ATTR_SIGN;
            token.sign.priority=PRIORITY_LEFT;
            token.sign.count=0;
            token.sign.combine=COMBINE_LEFT;
            str_array.push_back(token);
            p++;
            continue;
            
        }
        if(*p==')')
        {
            token.str=")";
            token.attr=ATTR_SIGN;
            token.sign.priority=PRIORITY_RIGHT;
            token.sign.count=0;
            token.sign.combine=COMBINE_RIGHT;
            str_array.push_back(token);
            p++;
            continue;
            
        }
        if(*(p+3))
        {
            s.assign(p,4);
            if(s.compare("sinh")==0)
            {
                token.str="sinh";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_SINH;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("cosh")==0)
            {
                token.str="cosh";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_COSH;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("tanh")==0)
            {
                token.str="tanh";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_TANH;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("coth")==0)
            {
                token.str="coth";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_COTH;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("sech")==0)
            {
                token.str="sech";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_SECH;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("csch")==0)
            {
                token.str="csch";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_CSCH;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("asin")==0)
            {
                token.str="asin";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_ASIN;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("acos")==0)
            {
                token.str="acos";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_ACOS;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("atan")==0)
            {
                token.str="atan";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_ATAN;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else if(s.compare("acot")==0)
            {
                token.str="acot";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_ACOT;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=4;
                continue;
                
            }
            else ;
            
        }
        if(*(p+2))
        {
            s.assign(p,3);
            if(s.compare("sin")==0)
            {
                token.str="sin";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_SIN;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else if(s.compare("cos")==0)
            {
                token.str="cos";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_COS;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else if(s.compare("tan")==0)
            {
                token.str="tan";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_TAN;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else if(s.compare("cot")==0)
            {
                token.str="cot";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_COT;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else if(s.compare("sec")==0)
            {
                token.str="sec";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_SEC;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else if(s.compare("csc")==0)
            {
                token.str="csc";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_CSC;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else if(s.compare("log")==0)
            {
                token.str="log";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_LOG;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else if(s.compare("exp")==0)
            {
                token.str="exp";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_EXP;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=3;
                continue;
                
            }
            else ;
            
        }
        if(*(p+1))
        {
            s.assign(p,2);
            if(s.compare("ln")==0)
            {
                token.str="ln";
                token.attr=ATTR_SIGN;
                token.sign.priority=PRIORITY_LN;
                token.sign.count=1;
                token.sign.combine=COMBINE_LEFT;
                str_array.push_back(token);
                p+=2;
                continue;
                
            }
            else ;
            
        }
        if(isdigit(*p))//��Ϊ��ֵ
        {
            string str_num;
            while(1)
            {
                str_num+=*p;
                p++;
                if(!isdigit(*p)&&*p!='.')break;
                
            }
            char *st;
            token.str=str_num;
            token.attr=ATTR_NUM;
            token.num=(double)strtod(str_num.c_str(),&st);//���ַ���תΪdoubleֵ
            str_array.push_back(token);
            continue;
        }
        if(isalpha(*p))//��Ϊ�����ַ���
        {
            for(int i=0;i<vals.size();i++)
            {
                if(*(p+vals[i].str.length()-1))
                {
                    s.assign(p,vals[i].str.length());
                    if(s.compare(vals[i].str)==0)
                    {
                        token.str=vals[i].str;
                        token.attr=ATTR_VAL;
                        vals[i].attr=ATTR_VAL;//Ϊ�Ժ�������Ż�
                        token.num=0;
                        str_array.push_back(token);
                        p+=vals[i].str.length();
                        break;
                    }
                }
                
            }
            continue;
        }
        if(isspace(*p))//��Ϊ�ո�
        {
            p++;
            continue;
        }
        return 0;//û��ƥ�����,����ʧ��
        
 }
 if(str_array.size()==0)return 0;//����ʧ��
 
 return 1;
}
//תΪ��׺���ʽ str_array:�ֽ��Ĺ�ʽ,������ת�����ʽ��
template<class T>
int after_gongshi(vector<TOKEN<T> > &str_array)
{
    stack<TOKEN<T> > str_stack;//��ջ
    vector<TOKEN<T> > temp_array;//��ʱ��ʽ����
    int i;
    TOKEN<T> token;
    for(i=0;i<str_array.size();i++)
    {
        
        if(str_array[i].attr==ATTR_SIGN)//��Ϊ�����
        {
            if(str_array[i].str.compare("(")==0)//��Ϊ������ֱ��ѹ��
            {
                str_stack.push(str_array[i]);
                continue;
            }
            if(str_array[i].str.compare(")")==0)//�������ţ����������ź�������������
            {
                
                while(1)
                {
                    if(str_stack.size()>0)
                    {
                        token=str_stack.top();
                        if(token.str.compare("(")==0)
                        {
                            str_stack.pop();
                            break;
                        }
                        else
                        {
                            temp_array.push_back(token);
                            str_stack.pop();
                            
                        }
                    }
                    else break;
                    
                }
                continue;
            }
            if(str_stack.size()==0)//����ջΪ�գ�ֱ��ѹ��
            {
                str_stack.push(str_array[i]);
                continue;
            }
            while(str_stack.size()>0)//����ջ��Ϊ��
            {
                token=str_stack.top();
                if(token.str.compare("(")!=0)//�Ҷ�ջ����Ϊ������
                {
                    //�������ȼ��ж��Ƿ���ջ
                    if(str_array[i].sign.priority>token.sign.priority)//�����ڶ�ջ����������ȼ���ֱ��ѹ��
                    {
                        str_stack.push(str_array[i]);
                        break;
                        
                    }
                    else if(str_array[i].sign.priority<token.sign.priority)//��С�ڶ�ջ����������ȼ�
                    {
                        //�����������ȼ�������������
                        temp_array.push_back(token);
                        str_stack.pop();
                        
                    }
                    else//�����ȼ���ͬ������ݽ�Ϸ����ж�
                    {
                        if(str_array[i].sign.combine==COMBINE_RIGHT)//��Ϊ�ҽ��
                        {
                            temp_array.push_back(token);//������ջ
                            str_stack.pop();
                        }
                        else if(str_array[i].sign.combine==COMBINE_LEFT)//��Ϊ����
                        {
                            str_stack.push(str_array[i]);//����ѹ���ջ
                            break;
                        
                        }
                        else return 0;
                    }
                }
                else //����ջ��Ϊ�����ţ�ֱ��ѹ��
                {
                    str_stack.push(str_array[i]);
                    break;
                    
                }
            }
            if(str_stack.size()==0)//�������������ȼ�������������������ջǡΪ������ѹ��
            {
                str_stack.push(str_array[i]);
            }
            continue;
            
        }
        else if(str_array[i].attr==ATTR_NUM)//��Ϊ��ֵ��ֱ��ѹ��
        {
            temp_array.push_back(str_array[i]);
            
        }
        else if(str_array[i].attr==ATTR_VAL)//��Ϊ������ֱ��ѹ��
        {
            temp_array.push_back(str_array[i]);
        }
        else return 0;
        
    }
    if(i==str_array.size())//������󣬵�������
    {
        while(1)
        {
            if(str_stack.size()>0)
            {
                token=str_stack.top();
                if(token.str.compare("(")==0)
                {
                    str_stack.pop();
                }
                else
                {
                    temp_array.push_back(token);
                    str_stack.pop();
                    
                }
            }
            else break;
            
        }
        
    }
    //��������
    str_array.empty();
    str_array.resize(0);
    for(i=0;i<temp_array.size();i++)
    {
        str_array.push_back(temp_array[i]);

    }
    return 1;
}
//����������vals�е�ֵ���滻��ʽ��������ʽ����
template<class T>
int fuzhi_gongshi(vector<TOKEN<T> > &str_array,vector<TOKEN<T> > vals)
{
    int i,j;
    for(i=0;i<str_array.size();i++)
    {
        if(str_array[i].attr==ATTR_VAL)
        {
            for(j=0;j<vals.size();j++)
            {
                if(vals[j].str.compare(str_array[i].str)==0)
                {
                    str_array[i].num=vals[j].num;
                    break;
                }
            
            }
        }
    }
    return 1;
}

//����str_array��ջ��ʽ����ֵ,���ݱ�������vals�е�ֵ����,resultΪ����ֵ
template<class T>
int compute_gongshi(vector<TOKEN<T> > &str_array,T &result)
{
    stack<TOKEN<T> > str_stack;//��ʱ��ջ
    int i=0;
    for(i=0;i<str_array.size();i++)
    {
        if(str_array[i].attr==ATTR_NUM)
        {
            str_stack.push(str_array[i]);
        
        }
        else if(str_array[i].attr==ATTR_VAL)
        {
            str_stack.push(str_array[i]);
        
        }
        else if(str_array[i].attr==ATTR_SIGN)
        {
            if(str_array[i].sign.count==2)
            {
                TOKEN<T> num1,num2,num;
                if(str_stack.size()>=2)//��������
                {
                    num1=str_stack.top();
                    str_stack.pop();
                    num2=str_stack.top();
                    str_stack.pop();

                    num.attr=ATTR_NUM;
                    try//���������쳣
                    {
                        if(str_array[i].str=="+")
                            num.num=num2.num+num1.num;
                        else if(str_array[i].str=="-")
                            num.num=num2.num-num1.num;
                        else if(str_array[i].str=="*")
                            num.num=num2.num*num1.num;
                        else if(str_array[i].str=="/")
                            num.num=num2.num/num1.num;
                        else if(str_array[i].str=="^")
                            num.num=pow(num2.num,num1.num);
                        else return 0;
                        
                    }
                    catch (...)
                    {
                        return 0;
                    }
                    str_stack.push(num);
                    continue;
                }
                else if(str_stack.size()==1)
                {
                    continue;
                }
                else return 0;

            }
            else if(str_array[i].sign.count==1)
            {
                TOKEN<T> num;
                if(str_stack.size()>=1)
                {
                    num=str_stack.top();
                    str_stack.pop();
                }
                else return 0;
                num.attr=ATTR_NUM;
                try
                {
                    if(str_array[i].str=="+")//����
                        num.num=num.num;
                    else if(str_array[i].str=="-")//����
                        num.num=-1*num.num;
                    else if(str_array[i].str=="sin")
                        num.num=sin(num.num);
                    else if(str_array[i].str=="cos")
                        num.num=cos(num.num);
                    else if(str_array[i].str=="tan")
                        num.num=sin(num.num)/cos(num.num);
                    else if(str_array[i].str=="cot")
                        num.num=cos(num.num)/sin(num.num);
                    /*else if(str_array[i].str=="asin")
                        num.num=asin(num.num);
                    else if(str_array[i].str=="acos")
                        num.num=acos(num.num);
                    else if(str_array[i].str=="atan")
                        num.num=atan(num.num);
                    else if(str_array[i].str=="acot")
                        num.num=acot(num.num);*/
                    else if(str_array[i].str=="sec")
                        num.num=1/cos(num.num);
                    else if(str_array[i].str=="csc")
                        num.num=1/sin(num.num);
                    else if(str_array[i].str=="sinh")
                        num.num=sinh(num.num);
                    else if(str_array[i].str=="cosh")
                        num.num=cosh(num.num);
                    else if(str_array[i].str=="tanh")
                        num.num=tanh(num.num);
                    else if(str_array[i].str=="coth")
                        num.num=1/tanh(num.num);
                    else if(str_array[i].str=="sech")
                        num.num=1/cosh(num.num);
                    else if(str_array[i].str=="csch")
                        num.num=1/sinh(num.num);
                    else if(str_array[i].str=="log")
                        num.num=log(num.num)/log(10);
                    else if(str_array[i].str=="ln")
                        num.num=log(num.num)/log(E);
                    else if(str_array[i].str=="exp")
                        num.num=exp(num.num);
                    else return 0;
                        
                }
                catch (...)
                {
                    return 0;
                }
                    
                str_stack.push(num);
                continue;
            
            }
            else return 0;
        
        }
        else return 0;
    
    }
    if(str_stack.size()==1)
    {
        result=str_stack.top().num;
        str_stack.pop();
    }
    else return 0;

    return 1;

}
//��ʽת��
template<class T>
int GongShi_Change(char *gongshi,vector<TOKEN<T> > &vals,vector<TOKEN<T> > &str_array)
{
    if(!fenjie(gongshi,str_array,vals))
        return 0;
    if(!after_gongshi(str_array))
        return 0;

    return 1;
}
//���㹫ʽ
template<class T>
int GongShi_Compute(vector<TOKEN<T> > &str_array,vector<TOKEN<T> > vals,T &result)
{
    /*if(!fuzhi_gongshi(str_array,vals))
        return 0;
    if(!compute_gongshi(str_array,result))
        return 0;*/
    //��ʽ������ֵ
    int i,j;
    //��ʽ����
    stack<TOKEN<T> > str_stack;//��ʱ��ջ
    for(i=0;i<str_array.size();i++)
    {
        if(str_array[i].attr==ATTR_NUM)
        {
            str_stack.push(str_array[i]);
            
        }
        else if(str_array[i].attr==ATTR_VAL)
        {
            //��ʽ������ֵ
            for(j=0;j<vals.size();j++)
            {
                if(vals[j].str.compare(str_array[i].str)==0)
                {
                    str_stack.push(vals[j]);//�Ż���ֱ��ѹ��ñ���,ʡȥ������ֵ
                    //str_array[i].num=vals[j].num;
                    break;
                }
            
            }
            //str_stack.push(str_array[i]);
        }
        else if(str_array[i].attr==ATTR_SIGN)
        {
            if(str_array[i].sign.count==2)
            {
                TOKEN<T> num1,num2,num;
                if(str_stack.size()>=2)//��������
                {
                    num1=str_stack.top();
                    str_stack.pop();
                    num2=str_stack.top();
                    str_stack.pop();

                    num.attr=ATTR_NUM;
            
                    if(str_array[i].str=="+")
                        num.num=num2.num+num1.num;
                    else if(str_array[i].str=="-")
                        num.num=num2.num-num1.num;
                    else if(str_array[i].str=="*")
                        num.num=num2.num*num1.num;
                    else if(str_array[i].str=="/")
                        num.num=num2.num/num1.num;
                    else if(str_array[i].str=="^")
                        num.num=pow(num2.num,num1.num);
                    else return 0;
        
                    str_stack.push(num);
                    continue;
                }
                else if(str_stack.size()==1)
                {
                    continue;
                }
                else return 0;

            }
            else if(str_array[i].sign.count==1)
            {
                TOKEN<T> num;
                if(str_stack.size()>=1)
                {
                    num=str_stack.top();
                    str_stack.pop();
                }
                else return 0;
                num.attr=ATTR_NUM;
            
                if(str_array[i].str=="+")//����
                    num.num=num.num;
                else if(str_array[i].str=="-")//����
                    num.num=-1*num.num;
                else if(str_array[i].str=="sin")
                    num.num=sin(num.num);
                else if(str_array[i].str=="cos")
                    num.num=cos(num.num);
                else if(str_array[i].str=="tan")
                    num.num=sin(num.num)/cos(num.num);
                else if(str_array[i].str=="cot")
                    num.num=cos(num.num)/sin(num.num);
                else if(str_array[i].str=="sec")
                    num.num=1/cos(num.num);
                else if(str_array[i].str=="csc")
                    num.num=1/sin(num.num);
                else if(str_array[i].str=="sinh")
                    num.num=sinh(num.num);
                else if(str_array[i].str=="cosh")
                    num.num=cosh(num.num);
                else if(str_array[i].str=="tanh")
                    num.num=tanh(num.num);
                else if(str_array[i].str=="coth")
                    num.num=1/tanh(num.num);
                else if(str_array[i].str=="sech")
                    num.num=1/cosh(num.num);
                else if(str_array[i].str=="csch")
                    num.num=1/sinh(num.num);
                else if(str_array[i].str=="log")
                    num.num=log(num.num)/log(10);
                else if(str_array[i].str=="ln")
                    num.num=log(num.num)/log(E);
                else if(str_array[i].str=="exp")
                    num.num=exp(num.num);
                else return 0;
            
                str_stack.push(num);
                continue;
            
            }
            else return 0;
        
        }
        else return 0;
    
    }
    if(str_stack.size()==1)
    {
        result=str_stack.top().num;
        //str_stack.pop();
    }
    else return 0;

    return 1;
}
