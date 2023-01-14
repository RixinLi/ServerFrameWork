#include "address.h"
#include <sstream>

namespace sylar{

int Address::getFamily() const{
    return getAddr()->sa_family;
}

std::string Address::toString(){
    std::stringstream ss;
    insert(ss);
    return ss.str();
}

bool Address::operator<(const Address& rhs) const{
    socklen_t minlen = std::min(getAddrLen(),rhs.getAddrLen());
    int result = memcmp(getAddr(),rhs.getAddr(),minlen);
    if (result < 0) return true;
    else if (result > 0) return false;
    else if (minlen == getAddrLen()) return true;
    return false;
}

bool Address::operator==(const Address& rhs) const{
    return ( getAddrLen()==rhs.getAddrLen() )
            &&
            ( memcmp(getAddr(),rhs.getAddr(),getAddrLen()) == 0);
}


// IPV4

bool Address::operator!=(const Address& rhs) const{
    return !(*this == rhs);
}


IPv4Address::IPv4Address(uint32_t address = INADDR_ANY, uint32_t port = 0){

}

const sockaddr* IPv4Address::getAddr() const{

}


socklen_t IPv4Address::getAddrLen() const{

}

std::ostream& IPv4Address::insert(std::ostream& os) const {
    
}

IPAddress::ptr IPv4Address::broadcastAddress(uint32_t prefix_len) {
    
}

IPAddress::ptr IPv4Address::networdAddress(uint32_t prefix_len) {
    
}

IPAddress::ptr IPv4Address::subnetMask(uint32_t prefix_len) {
    
}

uint32_t IPv4Address::getPort() const {
    
}

void IPv4Address::setPort(uint32_t v) {
    
}

// IPV6

bool Address::operator!=(const Address& rhs) const{
    return !(*this == rhs);
}


IPv6Address::IPv6Address(uint32_t address = INADDR_ANY, uint32_t port = 0){

}

const sockaddr* IPv6Address::getAddr() const{

}


socklen_t IPv6Address::getAddrLen() const{

}

std::ostream& IPv6Address::insert(std::ostream& os) const {
    
}

IPAddress::ptr IPv6Address::broadcastAddress(uint32_t prefix_len) {
    
}
IPAddress::ptr IPv6Address::networdAddress(uint32_t prefix_len) {
    
}
IPAddress::ptr IPv6Address::subnetMask(uint32_t prefix_len) {
    
}

uint32_t IPv6Address::getPort() const {
    
}

void IPv6Address::setPort(uint32_t v) {
    
}

// UnixAddress

UnixAddress::UnixAddress(const std::string& path){
    
}

const sockaddr* UnixAddress::getAddr() const {
    
}

socklen_t UnixAddress::getAddrLen() const {
    
}

std::ostream& UnixAddress::insert(std::ostream& os) const {
    
}

// UnknownAddress

UnknownAddress::UnknownAddress(const std::string& path){

}

const sockaddr* UnknownAddress::getAddr() const {
    
}

socklen_t UnknownAddress::getAddrLen() const {
    
}

std::ostream& UnknownAddress::insert(std::ostream& os) const {
    
}

}