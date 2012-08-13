/*******************************************************************************
 * This file is part of OpenWSN, the Open Wireless Sensor Network Platform.
 *
 * Copyright (C) 2005-2020 zhangwei(TongJi University)
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

/*******************************************************************************
 * hal_event
 *
 * @author zhangwei on 2012.08.02
 * - first version.
 *  
 ******************************************************************************/ 
 
#include "../hal_configall.h"
#include <string.h>
#include "../hal_foundation.h"
#include "../hal_event.h"

TiEvent g_hal_event;

TiEvent * hal_initevent( TiEventId id, void * objectfrom, void * objectto, TiFunEventHandler handler )
{
    g_hal_event.id = id;
    g_hal_event.objectfrom = objectfrom;
    g_hal_event.objectto = objectto;
    g_hal_event.handler = handler;
    
    return &(g_hal_event);
}
