
/*******************************************************************************
 * Network Byte Order
 ******************************************************************************/

/* Network order problem is encountered when transmit word or qword variables. It's
 * a very fundmental problem in network programing or porting to other platforms. 
 * 
 * According to the network byte order, the highest byte should be sent first. So
 * it's essentially a "Big Endian" design. Attention some protocol or hardware design
 * may not comply with this regulations. 
 *
 * In order to avoid the Little Endian/Big Endian problem in communications, and 
 * also helping the developer to write platform-independent programs, we define the 
 * following macros to help coding. These macros can often be found in a lot of 
 * powerful C language developing environment. 
 *
 *      htons()--"Host to Network Short"
 *      htonl()--"Host to Network Long"
 *      ntohs()--"Network to Host Short"
 *      ntohl()--"Network to Host Long"
 */

/*
#if defined(CONFIG_BIG_ENDIAN) && !defined(CONFIG_LITTLE_ENDIAN)
	#define htons(A) (A)
	#define htonl(A) (A)
	#define ntohs(A) (A)
	#define ntohl(A) (A) 
#elif defined(CONFIG_LITTLE_ENDIAN) && !defined(CONFIG_BIG_ENDIAN)
	#define htons(A) ((((uint16)(A) & 0xff00) >> 8) | \
					   (((uint16)(A) & 0x00ff) << 8))
	#define htonl(A) ((((uint32)(A) & 0xff000000) >> 24) | \
					   (((uint32)(A) & 0x00ff0000) >> 8) | \
					   (((uint32)(A) & 0x0000ff00) << 8) | \
					   (((uint32)(A) & 0x000000ff) << 24))

	#define ntohs htons
	#define ntohl htohl
#else
	//#error "Either BIG_ENDIAN or LITTLE_ENDIAN must be #defined, but not both." 
#endif
*/