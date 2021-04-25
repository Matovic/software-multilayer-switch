#include "CiscoDiscoveryProtocol.hpp"

// Constructor from buffer. This constructor will be called while sniffing packets, whenever a PDU of this type is found.
CiscoDiscoveryProtocol::CiscoDiscoveryProtocol(const uint8_t* data, uint32_t sz)
    : buffer_(data, data + sz) 
{

}

// Clones the PDU, used when copying PDUs.
CiscoDiscoveryProtocol* CiscoDiscoveryProtocol::clone() const
{
    return new CiscoDiscoveryProtocol(*this);
}

 // Getter method for the size of PDU.
uint32_t CiscoDiscoveryProtocol::header_size() const 
{
    return buffer_.size();
}


Tins::PDU::PDUType CiscoDiscoveryProtocol::pdu_type() const
{
    return pdu_flag;
}

void CiscoDiscoveryProtocol::write_serialization(uint8_t* data, uint32_t sz) 
{
    std::memcpy(data, buffer_.data(), sz);
}

// Getter method to retrieve the buffer member.
const std::vector<uint8_t>& CiscoDiscoveryProtocol::get_buffer() const 
{
    return buffer_;
}