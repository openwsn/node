#define CONFIG_INTERFACE_IMPLEMENTATION(intf_header, intf_type, impl_header, impl_type) \\
#include #intf_header#		\\
#include #impl_header#		\\
#define intf_type impl_type

#define CONFIG_INTERFACE_FUNCTION(trans_send, cc2420_send)
#define CONFIG_INTERFACE_FUNCTION(trans_recv, cc2420_recv)
#define CONFIG_INTERFACE_FUNCTION(trans_evolve, cc2420_evolve)
