#ifndef MT_MIDI_FILE_STREAM_H
#define MT_MIDI_FILE_STREAM_H

#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/core/defs.hpp>

namespace godot {

class MIDIChunkHeader : RefCounted {
    public:
        enum HeaderType { File, Track, Unknown };
        static const PackedByteArray CHUNK_TYPE_FILE;
        static const PackedByteArray CHUNK_TYPE_TRACK;

        HeaderType chunk_type;
        uint32_t chunk_length;
        PackedByteArray header_data;

    MIDIChunkHeader(PackedByteArray ctype, uint32_t clength)
    {
        chunk_type = HeaderType::Unknown;
        chunk_length = clength;
        if (ctype.size() == 4)
        {
            if (ctype == CHUNK_TYPE_FILE)
            {
                chunk_type = HeaderType::File;
            }
            else if (ctype == CHUNK_TYPE_TRACK)
            {
                chunk_type = HeaderType::Track;
            }
        }
    }

    MIDIChunkHeader(HeaderType ctype, uint32_t clength)
    {
        chunk_type = ctype;
        chunk_length = clength;
        if (ctype == HeaderType::File)
        {
            header_data.resize(clength);
        }
    }

    uint16_t get_format()
    {
        if ((chunk_type == HeaderType::File) && (header_data.size() > 1))
        {
            return (header_data[0] << 8) + header_data[1];
        }
        return INT16_MAX;
    }

    bool set_format(uint16_t format)
    {
        if ((chunk_type == HeaderType::File) && (header_data.size() > 1))
        {
            header_data.set(0, format >> 8);
            header_data.set(1, format & 0xFF);
            return true;
        }
        return false;
    }

    uint16_t get_track_count()
    {
        if ((chunk_type == HeaderType::File) && (header_data.size() > 3))
        {
            return (header_data[2] << 8) + header_data[3];
        }
        return 0;
    }

    bool set_track_count(uint16_t count)
    {
        if ((chunk_type == HeaderType::File) && (header_data.size() > 3))
        {
            header_data.set(2, count >> 8);
            header_data.set(3, count & 0xFF);
            return true;
        }
        return false;
    }

    uint16_t get_division()
    {
        if ((chunk_type == HeaderType::File) && (header_data.size() > 5))
        {
            return (header_data[4] << 8) + header_data[5];
        }
        return 0;
    }

    bool set_division(uint16_t division)
    {
        if ((chunk_type == HeaderType::File) && (header_data.size() > 5))
        {
            header_data.set(4, division >> 8);
            header_data.set(5, division & 0xFF);
            return true;
        }
        return false;
    }
};

class MTMidiFileStream {
    private:
        Ref<FileAccess> file;
        uint64_t size_bytes;
        FileAccess::ModeFlags mode;
    
    public:
        static const PackedStringArray ErrorMsgs;
        MTMidiFileStream(){};
        ~MTMidiFileStream();

        Error open_to_read(String file_path);
        Error open_to_write(String file_path, bool overwrite = false);
        bool close_file();
        uint64_t get_length_bytes();
        uint64_t get_file_position();
        uint64_t get_readable_byte_count();
        bool can_read(uint64_t count);
        bool can_write();
        Error read_bytes(uint64_t count, PackedByteArray buffer);
        Error write_bytes(PackedByteArray buffer);
        Error read_uint32(uint32_t& value_read);
        Error write_uint32(uint32_t value);
        Error read_uint16(uint16_t& value_read);
        Error write_uint16(uint16_t value);
        Error read_uint8(uint8_t& value_read);
        Error write_uint8(uint8_t value);
        Error read_variable_length_value(uint32_t& value, uint32_t& read_count);
        Error write_variable_length_value(uint32_t value, uint32_t& write_count);
        static uint32_t variable_length_to_uint32(PackedByteArray value);
        static PackedByteArray uint32_to_variable_length(uint32_t value);
        static uint32_t length_as_variable_length(uint32_t value);
        Error read_chunk_header(MIDIChunkHeader header);
        Error write_chunk_header(MIDIChunkHeader header);
};
}
#endif
