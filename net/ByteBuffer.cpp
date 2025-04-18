#include "ByteBuffer.h"

#include <string.h>

ByteBuffer::ByteBuffer(const ByteBuffer& rhs) :
    m_internalBuf(rhs.m_internalBuf)
{

}

ByteBuffer& ByteBuffer::operator=(const ByteBuffer& rhs)
{
    // TODO: insert return statement here
    if (this != &rhs)
    {
        m_internalBuf = rhs.m_internalBuf;
    }
    return *this;
}

ByteBuffer::ByteBuffer(ByteBuffer&& rhs) noexcept :
    m_internalBuf(std::move(rhs.m_internalBuf))
{

}

ByteBuffer& ByteBuffer::operator=(ByteBuffer&& rhs) noexcept
{
    // TODO: insert return statement here
    if (this != &rhs)
    {
        m_internalBuf = std::move(rhs.m_internalBuf);
    }
    return *this;
}

ByteBuffer::operator const char* ()
{
    return m_internalBuf.c_str();
}


void ByteBuffer::append(const char* buf, size_t bufLen)
{
    m_internalBuf.append(buf, bufLen);
}

void ByteBuffer::retrieve(std::string& outputbuf, size_t bufLen/*=0*/)
{
    if (bufLen == 0 || bufLen >= outputbuf.length())
    {
        outputbuf = std::move(m_internalBuf);
        return;
    }

    outputbuf = m_internalBuf.substr(0, bufLen);
    outputbuf.erase(0, bufLen);
}

size_t ByteBuffer::retrieve(char* buf, size_t bufLen)
{
    if (bufLen == 0)
        return 0;

    if (bufLen >= m_internalBuf.length())
        bufLen = m_internalBuf.length();

    memcpy(buf, m_internalBuf.c_str(), bufLen);

    return bufLen;
}

size_t ByteBuffer::peek(char* buf, size_t bufLen)
{
    if (bufLen == 0)
        return 0;

    if (bufLen >= m_internalBuf.length())
        bufLen = m_internalBuf.length();

    memcpy(buf, m_internalBuf.c_str(), bufLen);
    m_internalBuf.erase(0, bufLen);

    return bufLen;

}

void ByteBuffer::erase(size_t bufLen /*=0*/)
{
    if (bufLen == 0 || bufLen >= m_internalBuf.length())
    {
        m_internalBuf.clear();
        return;
    }

    m_internalBuf.erase(0, bufLen);
}

size_t ByteBuffer::findLF()
{
    return m_internalBuf.find("\n");
}

int ByteBuffer::remaining()
{
    return  m_internalBuf.length();
}

bool ByteBuffer::isEmpty()
{
    return m_internalBuf.empty();
}

void ByteBuffer::clear()
{
    m_internalBuf.clear();
}
