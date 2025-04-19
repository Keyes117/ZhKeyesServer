
/**
 * @zhkeyes 2025/4/6
 * 自定义区缓冲类
 */
#ifndef NET_BYTEBUFFER_H_
#define NET_BYTEBUFFER_H_

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
    //向缓冲区添加数据
    void append(const char* buf, size_t bufLen);
    //从缓冲区拿数据
    void retrieve(std::string& outputbuf, size_t bufLen = 0);
    size_t retrieve(char* buf, size_t bufLen);
    //查看数据，不拿出来
    size_t peek(char* buf, size_t bufLen);

    void erase(size_t bufLen = 0);

    //寻找\n
    size_t  findLF();

    //检查缓冲区当前还有多少字符数据
    int remaining();

    bool isEmpty();

    void clear();
private:
    //TODO:这里暂时用的std::string, 之后考虑更换
    std::string m_internalBuf;
};

#endif