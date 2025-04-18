#pragma once
/**
 * @zhkeyes 2025/4/6
 * 自定义缓冲类
 */
#include <string>

class ByteBuffer
{
public:
    ByteBuffer() = default;
    ~ByteBuffer() = default;


    ByteBuffer(const ByteBuffer& rhs);
    ByteBuffer& operator=(const ByteBuffer& rhs);

    ByteBuffer(ByteBuffer&& rhs) noexcept;
    ByteBuffer& operator=(ByteBuffer&& rhs) noexcept;
public:
    operator const char* ();

public:
    void append(const char* buf, size_t bufLen);
    void retrieve(std::string& outputbuf, size_t bufLen = 0);
    size_t retrieve(char* buf, size_t bufLen);
    size_t peek(char* buf, size_t bufLen);


    void erase(size_t bufLen = 0);

    //寻找\n
    size_t  findLF();

    int remaining();

    bool isEmpty();

    void clear();
private:
    std::string m_internalBuf;
};

