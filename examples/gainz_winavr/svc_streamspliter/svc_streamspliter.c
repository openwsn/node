#include "../common/rtl/rtl_configall.h"
#include <string.h>
#include "../common/hal/hal_debugio.h"
#include "../common/rtl/rtl_foundation.h"
#include "svc_streamspliter.h"

返回值就是output buffer中数据的长度
uint16 split_rxhandle( TiStreamSpliter * split, char * input, uint16 len, char * output, uint16 size )
{
	uint16 i = 0;
	uint16 pac_head = 0;
	uint16 pac_end = 0;

	=> please change the variable name from option to state
	=> 虽然简单,但这里也是小状态机的思想

	switch (split->state)
	{
	case SPLITER_STATE_WAITFOR_START:
	case SPLITER_STATE_WAITFOR_END:
	}

	=> 没细看这段程序,但基本的套路,用switch+当前状态判断的思想是正确的

	switch (split->option)
	{
	case 0x00://如果从未收到过PAC_START_FLAG
		for(i=0;i<len_in;i++)//抛弃收到的字节直到得到PAC_START_FLAG；
		{
			if(input[i]==PAC_START_FLAG)
			{
				split->option = 0x01;
				pac_head = i+1;
				break;
			}
		}

		for(i=pac_head;i<len_in;i++)//如果收到PAC_START_FLAG，则寻找此字节流中是否存在PAC_EDN_FLAG；
		{
			if(input[i]==PAC_END_FLAG)
			{
				split->option = 0x00;
				pac_end = i-1;
				break;
			}
		}
		if(pac_head==0)//没有收到过PAC_START_FLAG，跳出

		else if(pac_head<pac_end)//收到PAC_START_FLAG和PAC_END_FLAG
		{
			memmove(output, &input[pac_head], pac_end-pac_head+1);//直接取出packet
			memmove(input, &input[pac_end+1], len_in-pac_end);//将剩余字节迁移，方便迭代
			split->textpac_len = pac_end-pac_head+1;
			// 重点：
			// 当同时收到PAC_START_FLAG和PAC_END_FLAG，剩余的字节流仍需处理。
			// 故迭代调用split_rxhandle函数，现在设计output对象只有一个，
			// 则旧包会被新包覆盖
			// 此处用split结构的ret成员来保证返回的是最新的包大小，
			// 但似乎不妥的是处理output对象时必须同时修改ret。

			split_rxhandle( split, input, len_in-pac_end-1, output, len_out );		
		}
		else //收到过PAC_START_FLAG但是没有PAC_END_FLAG,全部字节流加入队列
		{
			memmove(split->buf, &input[pac_head], len_in-pac_head);//待修改
		}

		return split->ret;
		break;

	case 0x01://如果收到过PAC_START_FLAG，却没收到PAC_END_FLAG
		for(i=0;i<len_in;i++)//直接寻找PAC_END_FLAG，
		{
			if(input[i]==PAC_END_FLAG)
			{
				split->option = 0x00;
				pac_end = i-1;
				break;
			}
		}
		if(pac_end==0)//如果没有找到
			memmove(split->buf, &input[0], len_in);//全部加入队列
		else 
		{
			memmove(split->buf, &input[0], pac_end+1);//部分加入队列
			memmove(output, split->buf, queue_end);//取出packet
			split->textpac_len = queue_end;
			split_rxhandle( split, input, len_in-pac_end-1, output, len_out );		
						
		}

		return split->textpac_len;
		break;
	
	
	}
}



uint16 split_txhandle( TiStreamSpliter * split, char * input, uint16 len, char * output, uint16 size )
{
	只要在input前后拼上start/stop字符然后move到output中去即可
}
