#include "bytearray.h"

namespace sylar{

ByteArray::Node::Node(size_t s)
:ptr(new char[s]), size(s), next(nullptr)
{}

ByteArray::Node::Node()
:ptr(nullptr),size(0),next(nullptr)
{}

ByteArray::Node::~Node(){
if(ptr) delete[] ptr;
next = nullptr;
}


ByteArray::ByteArray(size_t base_size)
:m_baseSize(base_size),
 m_position(0),
 m_capacity(base_size),
 m_size(0),
 m_root(new Node(base_size)),
 m_cur(m_root),
 m_endian(SYLAR_BIG_ENDIAN)
{
}

ByteArray::~ByteArray(){
    Node* tmp = m_root;
    while(tmp){
        m_cur = tmp;
        tmp = tmp->next;
        delete m_cur;
    }
}

bool ByteArray::isLittleEndian()const{
    return m_endian == SYLAR_LITTLE_ENDIAN;
}

void ByteArray::setIsLittleEndian(bool val){
    if (val){
        m_endian = SYLAR_LITTLE_ENDIAN;
    }else{
        m_endian = SYLAR_BIG_ENDIAN;
    }
}


void ByteArray::writeFint8  (const int8_t& value){
    write(&value,sizeof(value));
}

void ByteArray::writeFuint8 (const uint8_t& value){
    write(&value,sizeof(value));
}
void ByteArray::writeFint16 (const int16_t& value){
    write(&value,sizeof(value));
}
void ByteArray::writeFuint16(const uint16_t& value){
    write(&value,sizeof(value));
}
void ByteArray::writeFint32 (const int32_t& value){
    write(&value,sizeof(value));
}
void ByteArray::writeFuint32(const uint32_t& value){
    write(&value,sizeof(value));
}
void ByteArray::writeFint64 (const int64_t& value){
    write(&value,sizeof(value));
}
void ByteArray::writeFuint64(const uint64_t& value);
void ByteArray::writeInt32  (const int32_t& value);
void ByteArray::writeUint32 (const uint32_t& value);
void ByteArray::writeInt64  (const int64_t& value);
void ByteArray::writeUint64 (const uint64_t& value);
void ByteArray::writeFloat  (const float& value);
void ByteArray::writeDouble (const double& value);
void ByteArray::writeStringF16(const std::string& value);
void ByteArray::writeStringF32(const std::string& value);
void ByteArray::writeStringF64(const std::string& value);
void ByteArray::writeStringVint(const std::string& value);
void ByteArray::writeStringWithoutLength(const std::string& value);
int8_t   ByteArray::readFint8();
uint8_t  ByteArray::readFuint8();
int16_t  ByteArray::readFint16();
uint16_t ByteArray::readFuint16();
int32_t  ByteArray::readFint32();
uint32_t ByteArray::readFuint32();
int64_t  ByteArray::readFint64();
uint64_t ByteArray::readFuint64();
int32_t  ByteArray::readInt32();
uint32_t ByteArray::readUint32();
int64_t  ByteArray::readInt64();
uint64_t ByteArray::readUint64();
float    ByteArray::readFloat();
double   ByteArray::readDouble();
std::string ByteArray::readStringF16();
std::string ByteArray::readStringF32();
std::string ByteArray::readStringF64();
std::string ByteArray::readStringVint();
void ByteArray::clear();
void ByteArray::write(const void* buf, size_t size);
void ByteArray::read(char* buf,size_t size);
void ByteArray::setPosition(size_t v);
bool ByteArray::writeToFile(const std::string& name) const;
void ByteArray::readFromfile(const std::string& name);




}