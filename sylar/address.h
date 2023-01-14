#ifndef __SYLAR_ADDRESS_H__
#define __SYLAR_ADDRESS_H__

#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <iostream>

namespace sylar{


class Address{

public:
    typedef std::shared_ptr<Address> ptr;
    virtual ~Address(){}

    int getFamily() const;

    virtual const sockaddr* getAddr() const = 0;
    virtual socklen_t getAddrLen() const = 0;
    virtual std::ostream& insert(std::ostream& os) const = 0;

    std::string toString();

    bool operator<(const Address& rhs) const;
    bool operator==(const Address& rhs) const;
    bool operator!=(const Address& rhs) const;

};

class IPAddress: public Address{
public:

    typedef std::shared_ptr<IPAddress> ptr;

    virtual IPAddress::ptr broadcastAddress(uint32_t prefix_len) = 0;
    virtual IPAddress::ptr networdAddress(uint32_t prefix_len) = 0;
    virtual IPAddress::ptr subnetMask(uint32_t prefix_len) = 0;

    virtual uint32_t getPort() const = 0;
    virtual void setPort(uint32_t v) = 0;

};

class IPv4Address : public IPAddress{

public:
    typedef std::shared_ptr<IPv4Address> ptr;

    IPv4Address(uint32_t address = INADDR_ANY, uint32_t port = 0);

    const sockaddr* getAddr() const override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
    IPAddress::ptr networdAddress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

    uint32_t getPort() const override;
    void setPort(uint32_t v) override;

private:
    sockaddr_in m_addr;

};

class IPv6Address : public IPAddress{

public:
    typedef std::shared_ptr<IPv6Address> ptr;

    IPv6Address();
    IPv6Address(const char* address, uint32_t port = 0)

    const sockaddr* getAddr() const override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

    IPAddress::ptr broadcastAddress(uint32_t prefix_len) override;
    IPAddress::ptr networdAddress(uint32_t prefix_len) override;
    IPAddress::ptr subnetMask(uint32_t prefix_len) override;

    uint32_t getPort() const override;
    void setPort(uint32_t v) override;

private:
    sockaddr_in6 m_addr;

};

class UnixAddress: public Address{

public:
    typedef std::shared_ptr<UnixAddress> ptr;

    UnixAddress();
    UnixAddress(const std::string& path);

    const sockaddr* getAddr() const override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

private:
    struct sockaddr_un m_addr;
    socklen_t m_length;
};

class UnknownAddress: public Address{

public:
    typedef std::shared_ptr<UnknownAddress> ptr;


    UnknownAddress(int family);

    const sockaddr* getAddr() const override;
    socklen_t getAddrLen() const override;
    std::ostream& insert(std::ostream& os) const override;

private:
    sockaddr m_addr;
};

}

#endif
