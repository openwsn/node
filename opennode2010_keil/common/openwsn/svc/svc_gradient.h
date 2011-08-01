/* svc_gradient
 * Gradient Field Routing. This can be regarded as the improvement of standard data
 * tree protocol (DTP, in svc_datatree module).
 * 
 * Different to DTP, the gradient routing service contains an neighbor nodes list
 * (which can be regarded as a simple verison of local network topology), and each 
 * nodes can has serveral parent nodes indicated by their level "property". The root
 * node has level 0, and the far nodes have large level values. 
 */

