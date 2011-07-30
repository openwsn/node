bible860427@163.comhttp://nesct.sourceforge.net/tinyaodv.html

Example Application: TinyAODV

Node Configuration
Code Generation
Updating Makefile
Build
Run
This package provides an implementation of a reduced AODV routing protocol. It supports many RFC features, but 
takes into account the reduced capabilities of Wireless Sensor Network platforms operating over TinyOS.

NST-AODV follows in part the main functionality described on HSN TinyAODV implementation. For that reason some of the modules 
have a similar structure as the one used in the implementation mentioned before.


/*
 * @reference
 * - AODV at UCSB, http://moment.cs.ucsb.edu/AODV/aodv.html;
 * - Ad hoc On-Demand Distance Vector (AODV) Routing, http://www.ietf.org/rfc/rfc3561.txt
 * - Ad hoc On-Demand Distance Vector Routing, http://en.wikipedia.org/wiki/Ad_hoc_On-Demand_Distance_Vector_Routing
 */
 