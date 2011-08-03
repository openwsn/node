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
/*****************************************************************************
 * @name BinaryXML
 * @author zhangwei on 20070331
 * 
 * this module implements a BinaryXML for embedded systems.
 * since we need to read/write different type data in the packets, we hope to
 * design a flexible packet payload data structure. so we design the BinaryXML.
 * it will take the old TOpenSections instead.  
 * 
 * 
 * @history
 * @modified by xxx on 200704
 * 
 ****************************************************************************/ 

#include <string.h>
#include "rtl_binaryxml.h"

/******************************************************************************
 * Q: key idea of BinaryXML
 * R: 
 * first let's take a glance at a simplified XML. it's a tree structure. each node in the tree is as the following:
 * 
 *   {node_id, node_size,  node_parent, property_id, property_value_type, value_length, value_data }
 *
 *  node_id  1B
 *  node_parent 1B
 *  property_id < 1B
 *    property_value_type = byte, word, dword, etc.
 *    however, i think the property value type can be decided from the property_id
 *    there're some pre-defined property id in the system such 1 for time, 2 for location, etc
 *  value_length > 1 means the value_data is a array
 * 
 * we can simplify this standard tree structure as the following
 * 
 * - the node and property are same. there's no difference between node and property.
 * - node_id is unncessary. because we can use the buffer index as the id.
 * - property_value_type can be eliminated because we can duduce it from property_id
 * - value_length is unecessary because it can be deduced from node_size
 * 
 * thus we can get the final version of BinaryXML
 * 
 * - it should has a root node. even the root node keeps the size of the buffer only
 * - node format: 
 * 
 *  {node_size, node_parent, property_id, value_data}
 * 
 * - the order of each child node is not important. generally assume all the child nodes are a sequence.
 * - the root node is always 0
 * 
 * for example
 * 
 *  4, 0, TOTALSIZE, 58, 6, 0, VIBSENSOR, 43,43,32, ..............
 * 
 *****************************************************************************/

/* construct a TiBinaryXml object on the specified buffer
 * attention the construct() function will automatically create the root 
 * node on the buffer. it will occupy 4 bytes. the root node holds the 
 * information of the total length of the xml.
 * 
 * @param
 * 	xml			point to an TiBinaryXml varaible not initilized
 * 	xmldata		databuffer to hold the binary xml data. 
 * 				it is often the address point to the payload of the packet.
 * 	size		xml data buffer size
 */
TiBinaryXml * xml_construct( TiBinaryXml * xml, char * xmldata, uint8 size )
{
	xml->buf = xmldata;
	xml->capacity = size;
	xml->length = 4;
	xml->current = 4;
	xml->buf[0] = 4;
	xml->buf[1] = 0;
	xml->buf[2] = BXML_PROPERTY_TOTAL_LENGTH;
	xml->buf[3] = 4;
	
	return xml;
}

void xml_destroy( TiBinaryXml * xml )
{
	NULL;
}

/* attach the TiBinaryXml buffer to an exist buffer. the data in the buffer will 
 * not changed */
void  xml_attach( TiBinaryXml * xml, char * xmldata, uint8 len )
{
	xml->buf = xmldata;
	xml->capacity = len;
	xml->length = len;
	xml->current = 0;	
}

/* allocate a new node in the memory buffer.
 * @return
 * 	0	when failed
 * 	>0	success. it's the identifier of the node, you can manipulate this node
 * 		through this id. actually, the value is essentially the index of the node
 * 		in the buffer. so the implementation of xml_read/xml_write is highly 
 * 		efficient.
 */
uint8 xml_newnode( TiBinaryXml * xml, uint8 request_size )
{
	uint8 id;
	
	if (xml->capacity - xml->length >= request_size)
	{
		id = xml->length; 
		xml->length += request_size;
		xml->buf[id] = request_size;
	}
	else
		id = 0;

	return id;
} 

/* append a node in the buffer 
 */
uint8 xml_append( TiBinaryXml * xml, uint8 parid, uint8 property, char * data, uint8 datalen )
{
	uint8 id = xml_newnode( xml, datalen );
	if (id > 0)
	{
		xml_write( xml, id, parid, property, data, datalen );
	}
	
	return id;
}

/* update a node. the node should be already allocated by xml_newnode() or xml_append()
 * the size of the node will keep unchanged. 
 * 
 * @return
 * the bytes successfully wroten
 */
uint8 xml_write( TiBinaryXml * xml, uint8 id, uint8 parid, uint8 property, char * data, uint8 datalen )
{
	xml->buf[id] = datalen;
	xml->buf[id+1] = parid;
	xml->buf[id+2] = property;
	memmove( &(xml->buf[id+3]), data, datalen );   
	return 0;
}

/* @return
 * the bytes successfully read
 */
uint8 xml_read( TiBinaryXml * xml, uint8 id, uint8 * parid, uint8 * property, char * data, uint8 size )
{
	*parid = xml->buf[id+1];
	*property = xml->buf[id+2];
	memmove( data, &(xml->buf[id+3]), min(size,xml->buf[id]) ); 
	return 0;
}

/* remove a node from the xml tree structure
 * not implemented. 
 * i think it is not necessary for such embedded systems. but i still keep this 
 * interface here for future use.
 */
void xml_remove( TiBinaryXml * xml, uint8 id )
{
	NULL;
	return;
}

/* find the id of parent node */
uint8 xml_findparent( TiBinaryXml * xml, uint8 id )
{
	return xml->buf[id+1];
}

/* find the id of the first child
 * assume the child node must occur after its parent node 
 */
uint8 xml_findchild( TiBinaryXml * xml, uint8 id )
{
	uint8 child = id + xml->buf[id];

	if (child < xml->length)
	{
		if (xml->buf[child+1] != id)
			child = 0;
	}
	else
		child = 0;
		
	return child;
}

/* find the id of previous brother node
 * xml_findprev() is not so efficient as xml_findnext(). so you'd better avoid
 * to use it.
 */
uint8 xml_findprev( TiBinaryXml * xml, uint8 id )
{
	bool found = false;
	uint8 lastid = 0, curid = 0;
	while (!found)
	{
		lastid = curid;
		curid += xml->buf[curid];
		if (curid == id)
		{
			if (xml->buf[lastid+1] == xml->buf[id+1])
			{
				found = true;
			}
			break;
		}
	}
	
	return found ? lastid : 0;
}


/* find the id of the next brother node */
uint8 xml_findnext( TiBinaryXml * xml, uint8 id )
{
	uint8 parid = xml->buf[id+1];
	uint8 lastid, curid = id;
	bool found = false;
	while (!found)
	{
		lastid = curid;
		curid += xml->buf[id];
		if (curid >= xml->length)
			break;
			
		if (xml->buf[curid+1] == parid)
		{
			found = true;
			break;
		}
	}
	
	return found ? curid : 0;
}
