//模块名称：rtl_textcodec(ANSI C, no depends)

#define CONFIG_SPLIT_BUFFER_SIZE 500
#define CONFIG_TEXTPAC_SIZE 300

#define PAC_START_FLAG 0x01
#define PAC_END_FLAG 0x02

#define SPLITER_STATE_WAITFOR_START 0
#define SPLITER_STATE_WAITFOR_END 0

typedef struct{
	uint8 state;
	char splitbuf[CONFIG_SPLIT_BUFFER_SIZE];
	uint8 len;
	uint8 option;
	uint8 textpac_len;
	char  textpac[CONFIG_TEXTPAC_SIZE]
}TiSteamSpliter;

// RX stream spliting.
// server（socket server or uart server）只要收到数据，
// 就可以把调用split_rxhandle处理之。
// Parameter-
// input: 	server（socket server or uart server）收到的数据
// len_in: 	传入数据的长度
// output:	完整的packet, 已经剔出Packet的START和END字符
// len_out: 	输出缓冲区大小，output buffer必须大到可以放下最长的packet
// return:	0 when not found a packet; >0 data length inside output buffer
uint16 split_rxhandle( TiStreamSpliter * split, char * input, uint16 len_in, char * output, uint16 len_out );

// TX stream assemble
// Parameter-
// input: 传入的packet，应是一个完整的packet
// output: 加装了START/END标记字符之后的packet
uint16 split_txhandle( TiStreamSpliter * split, char * input, uint16 len, char * output, uint16 size );
