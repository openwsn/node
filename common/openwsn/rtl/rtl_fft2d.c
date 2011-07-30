
2D-FFT (C code)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>



#define intsize sizeof(int)
#define complexsize sizeof(complex)
#define PI 3.1415926

int *a,*b;
int nLen,init_nLen,mLen,init_mLen,N,M;
FILE *dataFile;

typedef struct{
    float real;
    float image;
}complex;

complex *A,*A_In,*W;

complex Add(complex, complex);
complex Sub(complex, complex);
complex Mul(complex, complex);
int calculate_M(int);
void reverse(int,int);
void readData();
void fft(int,int);
void printResult();


int main()
{
    int i,j;

    readData();

    A = (complex *)malloc(complexsize*nLen);
    reverse(nLen,N);
    for(i=0; i<mLen; i++)
    {
        for(j=0; j<nLen; j++)
        {
            A[j].real = A_In[i*nLen+b[j]].real;
            A[j].image = A_In[i*nLen+b[j]].image;
        }
        
        fft(nLen,N);
        for(j=0; j<nLen; j++)
        {
            A_In[i*nLen+j].real = A[j].real;
            A_In[i*nLen+j].image = A[j].image;
        }
    }
    free(A);
 
    A = (complex *)malloc(complexsize*mLen);
    reverse(mLen,M);
    for(i=0; i<nLen; i++)
    {
        for(j=0; j<mLen; j++)
        {
            A[j].real = A_In[b[j]*nLen+i].real;
            A[j].image = A_In[b[j]*nLen+i].image;
        }

        fft(mLen,M);
        for(j=0; j<mLen; j++)
        {
            A_In[j*nLen+i].real = A[j].real;
            A_In[j*nLen+i].image = A[j].image;
        }
    }
    free(A);

    printResult();
 
    return 0;
}


void readData()
{
     int i,j;

     dataFile = fopen("dataIn.txt","r");
     fscanf(dataFile,"%d %d",&init_mLen,&init_nLen);
     M = calculate_M(init_mLen);
     N = calculate_M(init_nLen);
     nLen = (int)pow(2,N);
     mLen = (int)pow(2,M);
    
     A_In = (complex *)malloc(complexsize*nLen*mLen);

     for(i=0; i<init_mLen; i++)
     {
         for(j=0; j<init_nLen; j++)
         {
             fscanf(dataFile,"%f",&A_In[i*nLen+j].real);
             A_In[i*nLen+j].image = 0.0;
         }
     }
     fclose(dataFile);

     for(i=0; i<mLen; i++)
     {
         for(j=init_nLen; j<nLen; j++)
         {
             A_In[i*nLen+j].real = 0.0;
             A_In[i*nLen+j].image = 0.0;
         }
     }

     for(i=init_mLen; i<mLen; i++)
     {
         for(j=0; j<init_nLen; j++)
         {
             A_In[i*nLen+j].real = 0.0;
             A_In[i*nLen+j].image = 0.0;
         }
     }

     printf("Reading initial datas:\n");
     for(i=0; i<init_mLen; i++)
     {
         for(j=0; j<init_nLen; j++)
         {
             if(A_In[i*nLen+j].image < 0)
             { 
                 printf("%f%fi\t",A_In[i*nLen+j].real,A_In[i*nLen+j].image);
             }
             else
             {
                 printf("%f+%fi\t",A_In[i*nLen+j].real,A_In[i*nLen+j].image);
             }
         }
         printf("\n");
     }
  
     printf("\n");
  
     printf("Reading formal datas:\n");
     for(i=0; i<mLen; i++)
     {
         for(j=0; j<nLen; j++)
         {
             if(A_In[i*nLen+j].image < 0)
             { 
                 printf("%f%fi\t",A_In[i*nLen+j].real,A_In[i*nLen+j].image);
             }
             else
             {
                 printf("%f+%fi\t",A_In[i*nLen+j].real,A_In[i*nLen+j].image);
             }
         }
         printf("\n");
     }
}


void fft(int fft_nLen, int fft_M)
{
     int i;
     int lev,dist,p,t;
     complex B;

     W = (complex *)malloc(complexsize*fft_nLen/2);

     for(lev=1; lev<=fft_M; lev++)
     {
         dist = (int)pow(2,lev-1);
         for(t=0; t<dist; t++)
         {
             p = t*(int)pow(2,fft_M-lev);
             W[p].real = (float)cos(2*PI*p/fft_nLen);
             W[p].image = (float)(-1*sin(2*PI*p/fft_nLen));
             for(i=t; i<fft_nLen; i=i+(int)pow(2,lev))
             {
                 B = Add(A[i],Mul(A[i+dist],W[p]));
                 A[i+dist] = Sub(A[i],Mul(A[i+dist],W[p]));
                 A[i].real = B.real;
                 A[i].image = B.image;
             }
         }
     }

     free(W);
}


void printResult()
{
     int i,j;
 
     printf("Output results:\n");
     for(i=0; i<mLen; i++)
     {
         for(j=0; j<nLen; j++)
         {
             if(A_In[i*nLen+j].image < 0)
             {
                 printf("%f%fi\t",A_In[i*nLen+j].real,A_In[i*nLen+j].image);
             }
             else
             {
                 printf("%f+%fi\t",A_In[i*nLen+j].real,A_In[i*nLen+j].image);
             }
         }
         printf("\n");
     }
 
     free(A_In);
}

int calculate_M(int len)
{
    int i;
    int k;
 
    i = 0;
    k = 1;
    while(k < len)
    {
        k = k*2;
        i++;
    }
 
    return i;
}

void reverse(int len, int M)
{
    int i,j;
 
    a = (int *)malloc(intsize*M);
    b = (int *)malloc(intsize*len);
 
    for(i=0; i<M; i++)
    {
        a[i] = 0;
    }
 
    b[0] = 0;
    for(i=1; i<len; i++)
    {
        j = 0;
        while(a[j] != 0)
        {
            a[j] = 0;
            j++;
        }
  
        a[j] = 1;
        b[i] = 0;
        for(j=0; j<M; j++)
        {
            b[i] = b[i]+a[j]*(int)pow(2,M-1-j);
        }
    }
}

complex Add(complex c1, complex c2)
{
    complex c;
    c.real = c1.real+c2.real;
    c.image = c1.image+c2.image;
    return c;
}

complex Sub(complex c1, complex c2)
{
    complex c;
    c.real = c1.real-c2.real;
    c.image = c1.image-c2.image;
    return c;
}

complex Mul(complex c1, complex c2)
{
    complex c;
    c.real = c1.real*c2.real-c1.image*c2.image;
    c.image = c1.real*c2.image+c2.real*c1.image;
    return c;
}

本文引用地址： http://www.sciencenet.cn/m/user_content.aspx?id=292817








求救高手，关于八点FFT分割 C语言 的详细程序 ，最好附有步骤说明，感谢！

对我有用[0]丢个板砖[0]引用举报管理TOP 回复次数：2


summerfly_2004
(小鸟霏霏)
等　级：
#1楼 得分：0回复于：2008-06-16 18:41:28
看懂fft流程应该就没问题的呀，8点也正好是例子给的吧
对我有用[0]丢个板砖[0]引用举报管理TOP
精华推荐：求一个计算数字和的算法

Erorr
(小瑞)
等　级：
#2楼 得分：0回复于：2008-06-27 00:08:52
分数太少了！ 
C/C++ code

#include <iostream.h>
#include "math.h"

#define PI 3.14159265358976323846
#define NN 2048

struct complex                                                 //data form                 
{
    float Re;
    float Im;
};

void FFT(complex x[],complex y[],int N);                 

void main()
{
    int i,N;
    complex x[NN],y[NN];
    cout<<"how many point FFT do you want to do:(N=2,4,8,16...)"<<endl;
    cin>>N;
    cout<<"please input the data:"<<endl;
    
        for(i=0;i<N;i++)                                        //input
        {
            cout<<"Rm("<<i<<"):";
            cin>>x[i].Re;
            cout<<"Im("<<i<<"):";
            cin>>x[i].Im;
        }
        FFT(x,y,N);
        for(i=0;i<N;i++)
            cout<<"X["<<i<<"]=("<<x[i].Re<<")+j("<<x[i].Im<<")"<<endl;
}

void FFT(complex x[],complex y[],int N)                        //FFT
{
    int i,j,k,m=0;
    int p,q,s;
    double angle;
    complex W;
    complex c[NN];

    for(i=0,j=0;i<N-1;i++)                                    //hehe
    {
        if(i<j)
        {
            c[i]=x[i];
            x[i]=x[j];
            x[j]=c[i];
        }
        k=N/2;
        while(k<=j)
        {
            j=j-k;
            k=k/2;
        }
        j=j+k;
    }
    
    for(m=0;m<(log(N)/log(2));m++)                     //level 0,1,2...
    {
        for(k=0;k<pow(2,((log(N)/log(2))-m-1));k++)    //group ...4,2,1
        {
            for(s=0;s<pow(2,m);s++)                    //point  0,1,3...
            {
                p=k*int(pow(2,m+1))+s;
                angle=-PI*2*s*pow(2,((log(N)/log(2))-m-1))/N;
                W.Re=float (cos(angle));
                W.Im=float (sin(angle));
                q=p+int(pow(2,m));
                y[p].Re=x[p].Re+W.Re*x[q].Re-x[q].Im*W.Im;
                y[p].Im=x[p].Im+W.Im*x[q].Re+x[q].Im*W.Re;
                y[q].Re=x[p].Re-(W.Re*x[q].Re-x[q].Im*W.Im);
                y[q].Im=x[p].Im-(W.Im*x[q].Re+x[q].Im*W.Re);
            }
        }
        for(i=0;i<N;i++)
            x[i]=y[i];
    }

}
http://topic.csdn.net/u/20080613/22/bbffa6cb-b760-4bab-9b1b-0854a994d2f6.html
