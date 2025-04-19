
/**
 * @zhkeyes 2025/4/6
 * �Զ�����������
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
    //�򻺳����������
    void append(const char* buf, size_t bufLen);
    //�ӻ�����������
    void retrieve(std::string& outputbuf, size_t bufLen = 0);
    size_t retrieve(char* buf, size_t bufLen);
    //�鿴���ݣ����ó���
    size_t peek(char* buf, size_t bufLen);

    void erase(size_t bufLen = 0);

    //Ѱ��\n
    size_t  findLF();

    //��黺������ǰ���ж����ַ�����
    int remaining();

    bool isEmpty();

    void clear();
private:
    //TODO:������ʱ�õ�std::string, ֮���Ǹ���
    std::string m_internalBuf;
};

#endif