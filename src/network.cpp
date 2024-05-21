/*
 *  Copyright 2024 Belegkarnil
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 *  associated documentation files (the “Software”), to deal in the Software without restriction,
 *  including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
 *  so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or substantial
 *  portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
 *  OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include "network.hpp"


Network::Network(const char * iface){
    size_t len = strlen(iface);
    this->iface = (char*)malloc(len+1);
    strlcpy(this->iface,iface,len+1);

    this->net_socket = nl_socket_alloc();
}

Network::~Network(){
    free(this->iface);
    nl_socket_free(this->net_socket);
}

void Network::measure(){
    const struct rtnl_link ** net_link = &(this->rtnl_link);
    rtnl_link_get_kernel(this->net_socket, 0, this->iface, net_link);

    info->rx_bytes		= rtnl_link_get_stat(*net_link, RTNL_LINK_RX_BYTES);
    info->tx_bytes		= rtnl_link_get_stat(*net_link, RTNL_LINK_TX_BYTES);
    info->rx_packets	= rtnl_link_get_stat(*net_link, RTNL_LINK_RX_PACKETS);
    info->tx_packets	= rtnl_link_get_stat(*net_link, RTNL_LINK_TX_PACKETS);
    info->rx_errors	    = rtnl_link_get_stat(*net_link, RTNL_LINK_RX_ERRORS);
    info->tx_errors	    = rtnl_link_get_stat(*net_link, RTNL_LINK_TX_ERRORS);
    info->rx_dropped	= rtnl_link_get_stat(*net_link, RTNL_LINK_RX_DROPPED);
    info->tx_dropped	= rtnl_link_get_stat(*net_link, RTNL_LINK_TX_DROPPED);
    info->collisions	= rtnl_link_get_stat(*net_link, RTNL_LINK_COLLISIONS);

    /*
    RTNL_LINK_RX_COMPRESSED,
    RTNL_LINK_TX_COMPRESSED,
    RTNL_LINK_RX_FIFO_ERR,
    RTNL_LINK_TX_FIFO_ERR,
    RTNL_LINK_RX_LEN_ERR,
    RTNL_LINK_RX_OVER_ERR,
    RTNL_LINK_RX_CRC_ERR,
    RTNL_LINK_RX_FRAME_ERR,
    RTNL_LINK_RX_MISSED_ERR,
    RTNL_LINK_TX_ABORT_ERR,
    RTNL_LINK_TX_CARRIER_ERR,
    RTNL_LINK_TX_HBEAT_ERR,
    RTNL_LINK_TX_WIN_ERR,
    RTNL_LINK_MULTICAST,
    */
    rtnl_link_put(*net_link);
}

void outputHeader(FILE *restrict stream){
	fprintf(stream,"%s","net_inbytes,net_outbytes,net_inpackets,net_outpackets,in_errors,out_errors,in_dropped,out_dropped,net_collisions");
}

void output(FILE *restrict stream){
	fprintf(stream,
	    //9 fields
		"%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu,%lu",
		this->rx_bytes,
		this->tx_bytes,
		this->rx_packets,
		this->tx_packets,
		this->rx_errors,
		this->tx_errors,
		this->rx_dropped,
		this->tx_dropped,
		this->collisions
	);
}
