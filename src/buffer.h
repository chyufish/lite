#ifndef BUFFER_H
#define BUFFER_H

#include <vector>

namespace lite{

class Buffer{
public:
    static const size_t kInitLen=8192;
    explicit Buffer(size_t len=kInitLen):buffer_(len),read_index_(0),write_index_(0){}

    void Reset(){
        read_index_=0;
        write_index_=0;
    }
    size_t ReadableBytes() const{
        return write_index_-read_index_;
    }
    size_t WriteableBytes() const{
        return buffer_.size()-write_index_;
    }
    char* WriteBegin(){
        return buffer_.data()+write_index_;
    }
    const char* ReadBegin() const{
        return buffer_.data()+read_index_;
    }
    void HasWritten(size_t len){
        write_index_+=len;
    }
    void HasRead(size_t len){
        read_index_+=len;
    }
    void MakeSpace(size_t len){
        if(WriteableBytes()+read_index_<len){
            buffer_.resize(write_index_+len);
        }else{
            size_t readable=ReadableBytes();
            std::copy(buffer_.begin()+read_index_,buffer_.begin()+write_index_,
                buffer_.begin());
            read_index_=0;
            write_index_=readable;
        }
    }

    std::vector<char> buffer_;
    size_t read_index_;
    size_t write_index_;
};

}

#endif