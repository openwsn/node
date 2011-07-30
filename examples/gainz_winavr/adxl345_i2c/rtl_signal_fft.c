/*
* @history
* @modified by Lu Weihui(luweihui163@163.com) and He Yaxi(yaxihe@163.com), TongJi 
*  University on 2010.07.15
*  - compiled passed. tested ok.
*/

#include  "rtl_signal_fft.h"

// 定义该宏将对结果进行归一化处理，否则不进行归一化处理，但是结果可能会溢出
#define CONFIG_UNITARY


#define PI 3.1415926535897932384626433832795


// 计算余弦用的表，放大了1024倍（256点FFT时用）
const short int m_cos_table[LENGTH/2]=
{
    1024,1024,1023,1021,1019,1016,1013,1009,1004,999,

    993,987,980,972,964,955,946,936,926,915,

    903,891,878,865,851,837,822,807,792,775,

    759,742,724,706,688,669,650,630,610,590,

    569,548,526,505,483,460,438,415,392,369,

    345,321,297,273,249,224,200,175,150,125,

    100,75,50,25,0,-25,-50,-75,-100,-125,

    -150,-175,-200,-224,-249,-273,-297,-321,-345,-369,

    -392,-415,-438,-460,-483,-505,-526,-548,-569,-590,

    -610,-630,-650,-669,-688,-706,-724,-742,-759,-775,

    -792,-807,-822,-837,-851,-865,-878,-891,-903,-915,

    -926,-936,-946,-955,-964,-972,-980,-987,-993,-999,

    -1004,-1009,-1013,-1016,-1019,-1021,-1023,-1024
};

// 计算正弦用的表，放大了1024倍（256点FFT时用）
const short int m_sin_table[LENGTH/2]=
{
    0,25,50,75,100,125,150,175,200,224,

    249,273,297,321,345,369,392,415,438,460,

    483,505,526,548,569,590,610,630,650,669,

    688,706,724,742,759,775,792,807,822,837,

    851,865,878,891,903,915,926,936,946,955,

    964,972,980,987,993,999,1004,1009,1013,1016,

    1019,1021,1023,1024,1024,1024,1023,1021,1019,1016,

    1013,1009,1004,999,993,987,980,972,964,955,

    946,936,926,915,903,891,878,865,851,837,

    822,807,792,775,759,742,724,706,688,669,

    650,630,610,590,569,548,526,505,483,460,

    438,415,392,369,345,321,297,273,249,224,

    200,175,150,125,100,75,50,25
};

// 保存位倒置的值
LEN_TYPE m_bit_reverse[LENGTH];  



/********************************************************************
函数功能：对指定长度的二进制数求对应的位倒置值。
入口参数：N：要求倒序的值。L：bit数。
返    回：N的位倒置值。
备    注：无。
********************************************************************/
LEN_TYPE _do_bit_reverse(LEN_TYPE N, LEN_TYPE L)
{
    LEN_TYPE Temp1,Temp2;
    LEN_TYPE i;
    for(i=0;i<L/2;i++)
    {
        Temp1=0;
        Temp2=0;
        if(N&(1<<i))
            Temp1=1;
        if(N&(1<<(L-1-i)))
            Temp2=1;
        N&=~(1<<i);
        N&=~(1<<(L-1-i));
        N|=(Temp2<<i);
        N|=(Temp1<<(L-1-i));
    }
    return N;
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：初始化位倒置值数组，供FFT运算时查表用。
入口参数：无。
返    回：无。
备    注：结果保存在全局变量m_bit_reverse数组中。
********************************************************************/
void InitBitRev(void)
{
    LEN_TYPE i;
    for(i=0;i<LENGTH;i++) //求位反转
    {
        m_bit_reverse[i]=_do_bit_reverse(i,bL);
    }
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：对输入数据进行倒序处理。
入口参数：pIn：要处理的数据缓冲区地址。
返    回：无。
备    注：在调用FftExe之前应该调用该函数对数据进行倒序处理。
********************************************************************/
void FftInput(IN_TYPE *pIn)
{
    LEN_TYPE i;
    IN_TYPE Temp;
    for(i=0;i<LENGTH;i++)
    {
        if(m_bit_reverse[i]>i)  //如果倒位序比原序大，则交换
        {
            Temp=pIn[i];
            pIn[i]=pIn[m_bit_reverse[i]];
            pIn[m_bit_reverse[i]]=Temp;
        }
    }
}
/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：进行FFT运算。
入口参数：pIn：输入数据的缓冲区；pRe：保存实部的缓冲区；pIm：保存虚部的缓冲区。
返    回：无。
备    注：在调用该函数之前应该调用FftInput函数对数据进行倒序处理。
********************************************************************/
void FftExe(IN_TYPE *pIn, OUT_TYPE *pRe, OUT_TYPE *pIm)
{
    LEN_TYPE i,j;
    LEN_TYPE BlockSize;
    OUT_TYPE tr,ti;
    LEN_TYPE OffSet1,OffSet2;


    LEN_TYPE OffSet0;


    long c,s;

    //先计算2点的
    for(j=0;j<LENGTH;j+=2)
    {
        tr=pIn[j+1];
        pRe[j+1]=(pIn[j]-tr);
        pIm[j+1]=0;
        pRe[j]=(pIn[j]+tr);
        pIm[j]=0;
    }

    for(BlockSize=4;BlockSize<=LENGTH;BlockSize<<=1) //再一层层计算
    {
        for(j=0;j<LENGTH;j+=BlockSize)
        {
            for(i=0;i<BlockSize/2;i++)
            {

                OffSet0=LENGTH/BlockSize*i;
                c=m_cos_table[OffSet0];
                s=m_sin_table[OffSet0];

                OffSet1=i+j;
                OffSet2=OffSet1+BlockSize/2;
                tr=(OUT_TYPE)((c*pRe[OffSet2]+s*pIm[OffSet2])/1024);
                ti=(OUT_TYPE)((c*pIm[OffSet2]-s*pRe[OffSet2])/1024);
#ifdef CONFIG_UNITARY  //如果要对结果归一化处理，则每次运算要除以2
                pRe[OffSet2]=(pRe[OffSet1]-tr)/2;
                pIm[OffSet2]=(pIm[OffSet1]-ti)/2;
                pRe[OffSet1]=(pRe[OffSet1]+tr)/2;
                pIm[OffSet1]=(pIm[OffSet1]+ti)/2;
#else
                pRe[OffSet2]=(pRe[OffSet1]-tr);
                pIm[OffSet2]=(pIm[OffSet1]-ti);
                pRe[OffSet1]=(pRe[OffSet1]+tr);
                pIm[OffSet1]=(pIm[OffSet1]+ti);
#endif
            }
        }
    }
#ifdef CONFIG_UNITARY
    pRe[0]/=2;
    pIm[0]/=2;
#endif
}

/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：求复数的模。
入口参数：x：复数实部；y：复数实部；wamp：模。
返    回：无。
备    注：无。
********************************************************************/

void getamp(OUT_TYPE x[],OUT_TYPE y[],OUT_TYPE wamp[],short int len)
{
    int i;
    for(i=0;i<len;i++)
    {
        wamp[i]=(OUT_TYPE)sqrt(x[i]*x[i]+y[i]*y[i]);//<<2;//开方后放大
    }

}

/////////////////////////End of function/////////////////////////////


/********************************************************************
函数功能：求数组中最大元素下标。
入口参数：x：数组；len：数组中元素个数。
返    回：数组中最大元素下标。
备    注：无。
********************************************************************/

short int getimax(OUT_TYPE x[],short int len)
{
    short int imax=0,len2=len/2,i;
    for(i=0;i<len2;i++)  if(x[i]>x[imax]) imax=i;

    return imax;
}

/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：滤波器。
入口参数：in：要滤波的数组；np：数组中元素个数；cutoff：截止频率；
timebase：采样频率。
返    回：无。
备    注：无。
********************************************************************/

void RCFilter(IN_TYPE * in, short int np,float cutoff,float timebase)
{	
    //in[] contains the data to be filtered
    //np is number of data points(=dimension of in[])
    //cutoff is cut-off frequency in Hz
    //timebase is sample interval in seconds


    short i;
    float a,b,y;

    a=2-cos(2*PI*cutoff*timebase);

    if(a!=1)
    {
        a=a-sqrt(a*a-1);
        b=1-a;
        y=b*in[0];
        for(i=0;i<np;i++) in[i]=y=b*in[i]+a*y;
        y*=b;
        for(i=np-1;i>=0;i--) in[i]=y=b*in[i]+a*y;
    }

}

/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：得到FFT后，频谱序号对应的频率。
入口参数：t：采样频率；n：采样点数；i：频谱序号。
返    回：频谱序号对应的频率。
备    注：无。
********************************************************************/

float getfre(float t,short int n,short int i)
{
    return i/(t*n);
}

/////////////////////////End of function/////////////////////////////

/********************************************************************
函数功能：（主程序）测试FFT程序。
入口参数：无。
返    回：无。
备    注：无。
********************************************************************/
/*
#define T 0.05 //采样周期


void main(void)
{

int i=0,k=0;

IN_TYPE InBuffer1[LENGTH];

OUT_TYPE Re[LENGTH];
OUT_TYPE Im[LENGTH];

short int imax;

for(i=0;i<LENGTH;i++)
{
InBuffer1[i]=(IN_TYPE)(900*sin(i));
}

//RCFilter(InBuffer1,LENGTH,1,T);

InitBitRev();  //初始化FFT用的位反转用的表格
FftInput(InBuffer1);       //位倒序
FftExe(InBuffer1,Re,Im);   //做FFT运算


getamp(Re,Im,Re,LENGTH);

for(i=0;i<LENGTH;i++) cout<<Re[i]<<setw(10);//输出FFT后频谱
imax=getimax(Re,LENGTH);

float fre;
fre=getfre(T,LENGTH,imax);

cout<<endl<<imax<<endl;//输出最大值序号
cout<<fre<<endl; //输出FFT后最大值对应频率
}

*/
