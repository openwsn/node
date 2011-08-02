
/** 
 * 802.11 Frame Format
 *
 * All 802.11 frames are composed by the following components:
 *
 *  +----------+-------------+----------------------------------------+-----+
 *  | Preamble | PLCP Header |                 MAC Data               | CRC |
 *  +----------+-------------+----------------------------------------+-----+
 * 
 * 
 * MAC Data
 *  +------------------+----------------+----------+----------+----------+-------------+----------+-------------------+-------+
 *  | Frame Control 2B | Duration/ID 2B | Addr1 6B | Addr2 6B | Addr3 6B | Sequence 2B | Addr4 6B | Frame Body 0-2312 | CRC 4 |
 *  +------------------+----------------+----------+----------+----------+-------------+----------+-------------------+-------+
 *
 *
 * RTS Frame Format
 * 
 *  +------------------+-------------+-------+-------+--------+
 *  + Frame Control 2B | Duration 2B | RA 6B | TA 6B | CRC 4B | 
 *  +------------------+-------------+-------+-------+--------+
 *
 *  RA: receiver address
 *  TA: who transmit the RTS frame
 *  duration: in microseconds, required to transmit the next Data or Management frame,
 *      plus one CTS frame, plus one ACK frame, plus three SIFS intervals.
 * 
 * 
 * CTS Frame Format
 * 
 *  +------------------+-------------+-------+--------+
 *  + Frame Control 2B | Duration 2B | RA 6B | CRC 4B | 
 *  +------------------+-------------+-------+--------+
 * 
 *
 * ACK Frame Format
 * 
 *  +------------------+-------------+-------+--------+
 *  + Frame Control 2B | Duration 2B | RA 6B | CRC 4B | 
 *  +------------------+-------------+-------+--------+
 *
 */
