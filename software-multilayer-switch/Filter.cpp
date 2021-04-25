#include "Filter.hpp"

Filter::Filter()
	: b_filter_protocol_ { false }, b_http_{ false }, b_icmp_{ false }, 
	b_tcp_{ false }, b_udp_{ false }, b_ip_{ false }, b_arp_{ false }
{

}