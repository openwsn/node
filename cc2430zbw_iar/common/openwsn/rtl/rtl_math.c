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

#include <math.h>

/* Reference
 * [x] C语言程序设计的常用算法,
 *     http://hi.baidu.com/zkheartboy/blog/item/26dd17e9fd9e203bb90e2d15.html
 *     (很全！，包括数值，排序，查找等)
 */

/*
struct Point {float x, y;};
struct Line {Point p0, p1;};

// Line Intersection
// http://www.mydev.info/snippets.html
//
int IntersectLine(const Line& line0, const Line& line1, Point& pnt)
{
	const Point& a = line0.p0; // AB = A + r*(B-A), r in [0,1]
	const Point& b = line0.p1;
	const Point& c = line1.p0; // CD = C + s*(D-C), s in [0,1]
	const Point& d = line1.p1;
	Point e0 = {b.x-a.x, b.y-a.y};
	Point e1 = {d.x-c.x, d.y-c.y};
	Point e2 = {a.x-c.x, a.y-c.y};

	// XA + r*(XB-XA) = XC + s*(XD-XC)
	// YA + r*(YB-YA) = YC + s*(YD-YC)
	// r = ((YA-YC)(XD-XC)-(XA-XC)(YD-YC))/((XB-XA)(YD-YC)-(YB-YA)(XD-XC))
	// s = ((YA-YC)(XB-XA)-(XA-XC)(YB-YA))/((XB-XA)(YD-YC)-(YB-YA)(XD-XC))
	float denom   = e0.x*e1.y - e0.y*e1.x;
	float numer_r = e2.y*e1.x - e2.x*e1.y;
	float numer_s = e2.y*e0.x - e2.x*e0.y;

	const float TOL = 0.0001f;
	bool parallel = ::fabsf(denom)<TOL;
	if (parallel)
	{
        bool coincident = (::fabsf(numer_r)<TOL) && (::fabsf(numer_s)<TOL);
        return coincident ? +1 : -1;
    }
    else
    {
        numer_r/= denom;
        numer_s/= denom;
        pnt.x = a.x + numer_r*e0.x; // XI = XA + r*(XB-XA)
        pnt.y = a.y + numer_r*e0.y; // YI = YA + r*(YB-YA)
		bool within_line0 = (numer_r>-TOL) && (numer_r<1+TOL);
		bool within_line1 = (numer_s>-TOL) && (numer_s<1+TOL);
		return within_line0 && within_line1 ? 0 : +2;
	}
	// returns: -1 parallel, +1 parallel and coincident,
	// 0 intersection exists, +2 line segments do not intersect
}
*/

/*
struct Point {float x, y;};
struct Triangle {Point p0, p1, p2;};

// Point in Triangle
// http://www.mydev.info/snippets.html
//
bool PointInTriangle(const Point& p, const Triangle& tri, float& u, float& v)
{
    const Point& a = tri.p0;
    const Point& b = tri.p1;
    const Point& c = tri.p2;
    Point e0 = {p.x-a.x, p.y-a.y}; // AP
    Point e1 = {b.x-a.x, b.y-a.y}; // AB
    Point e2 = {c.x-a.x, c.y-a.y}; // AC

    const float TOL = 0.0001f;
    if (::fabsf(e1.x)<TOL) // AB vertical
    {
        u = e0.x/e2.x;
        if (u<-TOL || u>1+TOL) return false;
        if (::fabsf(e1.y)<TOL) return false; // AB horizontal
        v = (e0.y - e2.y*u)/e1.y;
    }
    else
    {
        float d = e2.y*e1.x - e2.x*e1.y; // area
        if (::fabsf(d)<TOL) return false; // collapsed triangle
        u = (e0.y*e1.x - e0.x*e1.y)/d;
        if (u<-TOL || u>1+TOL) return false;
        v = (e0.x - e2.x*u)/e1.x;
    }
    return (v>-TOL && u+v<1+TOL);

    // u and v are the barycentric coordinates:
    // p.x = (1-u-v)*a.x + v*b.x + u*c.x,
    // p.y = (1-u-v)*a.y + v*b.y + u*c.y, u>=0, v>=0, u+v<=1
    // compute interpolated vertex attributes:
    // attrib_p = (1-u-v)*attrib_a + v*attrib_b + u*attrib_c
}

*/
