#pragma once

#include <string>

class Filter
{
public:
    Filter();

	std::string
		filter_src_ip_add_,									// filter based on source IP address
		filter_dst_ip_add_,									// filter based on destination IP address
		filter_src_mac_add_,								// filter based on source MAC address
		filter_dst_mac_add_;								// filter based on source MAC address
	bool b_http_, b_icmp_, b_tcp_, b_udp_, b_ip_, b_arp_;	// filter based on network protocol
	bool b_filter_protocol_;
	bool b_filter_out;										// filter based on IN or OUT direction, false is for IN and true is for OUT
	bool b_filter_deny;										// filter based on user permission, false is for permit and true is for deny
};

