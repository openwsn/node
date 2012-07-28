/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2010 zhangwei(TongJi University)
 *
 * OpenWSN is a free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 or (at your option) any later version.
 *
 * OpenWSN is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 59 Temple
 * Place, Suite 330, Boston, MA 02111-1307 USA.
 *
 * For non-opensource or commercial applications, please choose commercial license.
 * Refer to OpenWSN site http://code.google.com/p/openwsn/ for more detail.
 *
 * For other questions, you can contact the author through email openwsn#gmail.com
 * or the mailing address: Dr. Wei Zhang, Dept. of Control, Dianxin Hall, TongJi
 * University, 4800 Caoan Road, Shanghai, China. Zip: 201804
 *
 ******************************************************************************/

/******************************************************************************
 * rtl_stochastic
 * This module realizes some statistic functions.
 *
 * @author sunqiang(TongJi University) on 20091123
 *	- first created.
 *****************************************************************************/ 

#include "rtl_stochastic.h"
#include <math.h>

static double _normal_01_peiji();
static double _normal_01_bili();

/******************************************************************************
 * higher level stochastic distributions
 *****************************************************************************/ 
/* 得到[x1,x2)区间随机数
 */
double rand_uniform(double x1, double x2)
{
	return rand_double() * (x2 - x1) + x1;
}

/* For internal use.
 * 配极法产生正态分布, 一次产生两个独立变量, 只取一个即可
 * Reference: Knuth: Art of Computer Programming, Vol2, Page 107
 */
double _normal_01_peiji()
{	
	double v1,v2,s;
	double x1,x2;
	do {
		v1=rand_double()*2.0-1.0;
		v2=rand_double()*2.0-1.0;
		s= v1*v1+v2*v2;
	} while(s>=1.0);
	if (s==0)
		x1=x2=0.0;
	else{
		x1= v1 * sqrt(-2.0 * log(s)/s);
		x2= v2 * sqrt(-2.0 * log(s)/s);
	}
	return x1;
}

/* For internal use.
 * 比例法产生正态分布
 * Reference: Knuth: Art of Computer Programming, Vol2, Page 114
 */
double _normal_01_bili()
{
	const double M_exp1= exp(1.0);
	double u,v,x,x2;
	const double c1=sqrt(8.0/M_exp1);
	const double c2=exp(0.25)*4;
	const double c3=exp(-1.35)*4;
	while(1){
		do{
			u=rand_double();
		}while(u==0.0);
		v=rand_double();
		x=c1*(v-0.5)/u;
		x2=x*x;
		if (x2<=5.0-c2*u)
			break;
		if (x2>=c3/u+1.4)
			continue;
		if (x2<=-4.8*log(u))
			break;
	}
	return x;
}

/* 产生期望为avg，方差为sigma的正态分布
 */
double rand_normal(double avg, double sigma)
{
	return _normal_01_bili() * sigma + avg;
}

/* 期望为lamda的Possion分布，方差也是lamda
 */
int rand_poisson(double lamda)
{
	double expl= exp(-lamda);
	double sum=1.0;
	int n=0;
	do {
		sum*=rand_double();
		++n;
	} while(sum>expl);
	return n-1;
}

/* 三角分布, 密度函数为三角形, 左下x坐标为a, 右下x坐标为b, 且假设a<0<b;
 * 高度为 2/(a-b).
 * 分布函数为 d(x)= ((1/a)*x*x-2*x+a)/(a-b);when x<=0; ((1/b)*x*x-2*x+a)/(a-b); when x>=0
 * d(a)=0; d(0)=a/(a-b); d(b)=1;
 * 期望avg_theory=(a+b)/3.0;
 * 方差std_theory=(a*a+b*b-a*b)/18.0
 */
double rand_triangle(double a, double b)
{
	double y=rand_double(), x=0;
	if (y<a/(a-b)){
		x=a*(1-sqrt(y*(1-b/a)));
	}
	else{
		x=b*(1-sqrt((1-y)*(1-a/b)));
	}
	return x;
}