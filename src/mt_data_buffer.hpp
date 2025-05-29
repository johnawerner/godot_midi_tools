#ifndef MT_DATA_BUFFER_H
#define MT_DATA_BUFFER_H

#include <godot_cpp/core/object.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include "mt_midi_file_stream.hpp"

namespace godot {

class MTDataBuffer : public Object {
    private:
    PackedByteArray data;
    uint64_t index;

    public:
    MTDataBuffer(PackedByteArray data) : data(data), index(0) {}
    uint64_t size() { return data.size(); }
    bool can_read(uint64_t count) { return count <= data.size() - index; }
    uint64_t unread_count() { return data.size() - index; }
    uint8_t peek() { return index < data.size() ? data[index] : 0; }

    Error read_byte(uint8_t &byte_read)
    {
        if (can_read(1))
        {
            byte_read = data[index++];
            return Error::OK;
        }
        return Error::ERR_UNAVAILABLE;
    }

    PackedByteArray read_bytes(uint64_t count, Error &result)
    {
        PackedByteArray tmp_buf;
        if (can_read(count))
        {
            result = Error::OK;
            tmp_buf.append_array(data.slice(index, index + count));
            index += count;
        }
        else
        {
            result = Error::ERR_UNAVAILABLE;
        }
        return tmp_buf;
    }

    Error read_variable_length(uint32_t &value, uint32_t &read_count)
    {
        bool more_data = true;
        PackedByteArray buffer;
        uint8_t data;

        value = 0;
        read_count = 0;
    
        Error result = Error::OK;
        while ((result == Error::OK) && more_data)
        {
            result = read_byte(data);
            if (result == Error::OK)
            {
                read_count++;
                buffer.append(data & 0x7F);
                more_data = data & 0x80;
            }
        }
    
        if ((result == Error::OK) && (read_count > 0) && (read_count <= 4))
        {
            value = MTMidiFileStream::variable_length_to_uint32(buffer);
        }
        else
        {
            result = Error::ERR_PARSE_ERROR;
        }
            
        return result;
    }
};

}
#endif
