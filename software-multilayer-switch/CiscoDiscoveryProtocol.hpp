#pragma once

#include <tins/tins.h>
#include <vector>

/*
 * CiscoDiscoveryProtocol PDU class, which behaves very similarly to Tins::RawPDU.
 */
class CiscoDiscoveryProtocol : public Tins::PDU 
{
public:
    /*
     * Unique protocol identifier. For user-defined PDUs, it must use values greater or equal to PDU::USER_DEFINED_PDU;
     */
    static const Tins::PDU::PDUType pdu_flag = PDU::USER_DEFINED_PDU;
    
    int 
        time_to_live = 30,
        local_port_id,
        port_id;
    std::string 
        device_id,
        addresses;

    /*
     * Constructor from buffer. This constructor will be called while
     * sniffing packets, whenever a PDU of this type is found.
     *
     * The "data" parameter points to a buffer of length "sz".
     */
    CiscoDiscoveryProtocol(const uint8_t* data, uint32_t sz);

    // Clones the PDU, used when copying PDUs.
    CiscoDiscoveryProtocol* clone() const;

    // Getter method for the size of PDU.
    uint32_t header_size() const;

    /*
     * This method must return pdu_flag.
     */
    Tins::PDU::PDUType pdu_type() const;

    /*
     * Serializes the PDU. The serialization output should be written
     * to the buffer pointed to by "data", which is of size "sz". The
     * "sz" parameter will be equal to the value returned by
     * CiscoDiscoveryProtocol::header_size.
     */
    void write_serialization(uint8_t* data, uint32_t sz);

    // Getter method to retrieve the buffer member.
    const std::vector<uint8_t>& get_buffer() const;

private:
    std::vector<uint8_t> buffer_;
};

// Assign value to the pdu_flag.
//const Tins::PDU::PDUType CiscoDiscoveryProtocol::pdu_flag = PDU::USER_DEFINED_PDU;
