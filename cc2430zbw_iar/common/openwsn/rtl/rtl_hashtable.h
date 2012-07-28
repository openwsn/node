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
/* Reference
 * 学习STL map, STL set之数据结构基础, 2005, http://stl.winterxy.com/html/000039.html
 * 详细解说STL hash_map系列, http://www.stlchina.org/twiki/bin/view.pl/Main/STLDetailHashMap
 */

hashtable, map, set需要一个hash接口
key = uintx or (buffer pointer, len)

nhash hash table with uintx key
mhash mutibyte hash   (buffer pointer + len, such as string)



hashtable_useinterface( key2index, findempty, find )

hash_key2index_xxxtype
hash_findempty
hash_find



我的需要是一个hashtable or  hashmap
uintx id ==> item contents

如果要求经常在中间插入删除，list效率高于vector
如果要求快速查找，vector更优秀
如果要求插入删除比较平衡，则heap和tree比较好
但不管怎么说，我们总归都是需要这样一个数据结构[id, {content}, state, other informaiton such as parent/child]




hash_isempty
hash_findempty
hash_find

[meta information, item state information, item content]
可以考虑基于vector实现，让vector管理内存





this hash table assume uintx is the key type
{key, value} pair

key = int

vector的升级版
只要增加如下两个函数即可
vec_findindex( key )
vec_find(key)
vec_keytoindex(uintx key, uintx idx) = hash function

vec_findemptybykey

vec_mapkey2int

我原先的vector限定了只能顺序添加，其实是个list。这个限制对vector而言是不是不合理？因为vector本质上是代替array并提供一系列更方便的方法
现在要非顺序访问和操作

但这样的话，如何才能知道某个item是否已经被使用呢？
没办法

增加数据结构list

list
添加和释放都是O(1)，基于数组的实现。这与vector是不同的
但是insert和delete和find很慢
为了改进，增加TiHashVector

vec_isavailable(item *)
vec_findempty_uintxkey(key)
vec_find_uintx(key)
vec_findempty_charkey(key)
vec_find_uintxkey(key)

vec_hashput
vec_hashget
vec_hashfind
如果把vector作为一个hashtable来用，那么item中的第一个字节我们规定必须用来表示state
vec_hashfind( key )
vec_hashfindempty( index )
vec_hashput
vec_hashget
vec_hashitems

问题是是否empty，我怎么知道

hash_map/set



