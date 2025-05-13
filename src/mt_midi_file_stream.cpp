#include "mt_midi_file_stream.hpp"
#include <godot_cpp/variant/array.hpp>

using namespace godot;

const PackedByteArray MIDIChunkHeader::CHUNK_TYPE_FILE = {'M','T','h','d'};
const PackedByteArray MIDIChunkHeader::CHUNK_TYPE_TRACK = {'M','T','r','k'};

const PackedStringArray MTMidiFileStream::ErrorMsgs = {
	"OK",
    "FAILED",
    "ERR_UNAVAILABLE",
    "ERR_UNCONFIGURED",
	"ERR_UNAUTHORIZED",
	"ERR_PARAMETER_RANGE_ERROR",
	"ERR_OUT_OF_MEMORY",
	"ERR_FILE_NOT_FOUND",
	"ERR_FILE_BAD_DRIVE",
	"ERR_FILE_BAD_PATH",
	"ERR_FILE_NO_PERMISSION",
	"ERR_FILE_ALREADY_IN_USE",
	"ERR_FILE_CANT_OPEN",
	"ERR_FILE_CANT_WRITE",
	"ERR_FILE_CANT_READ",
	"ERR_FILE_UNRECOGNIZED",
	"ERR_FILE_CORRUPT",
	"ERR_FILE_MISSING_DEPENDENCIES",
	"ERR_FILE_EOF",
	"ERR_CANT_OPEN",
	"ERR_CANT_CREATE",
	"ERR_QUERY_FAILED",
	"ERR_ALREADY_IN_USE",
	"ERR_LOCKED",
	"ERR_TIMEOUT",
	"ERR_CANT_CONNECT",
	"ERR_CANT_RESOLVE",
	"ERR_CONNECTION_ERROR",
	"ERR_CANT_ACQUIRE_RESOURCE",
	"ERR_CANT_FORK",
	"ERR_INVALID_DATA",
	"ERR_INVALID_PARAMETER",
	"ERR_ALREADY_EXISTS",
	"ERR_DOES_NOT_EXIST",
	"ERR_DATABASE_CANT_READ",
	"ERR_DATABASE_CANT_WRITE",
	"ERR_COMPILATION_FAILED",
	"ERR_METHOD_NOT_FOUND",
	"ERR_LINK_FAILED",
	"ERR_SCRIPT_FAILED",
	"ERR_CYCLIC_LINK",
	"ERR_INVALID_DECLARATION",
	"ERR_DUPLICATE_SYMBOL",
	"ERR_PARSE_ERROR",
	"ERR_BUSY",
	"ERR_SKIP",
	"ERR_HELP",
	"ERR_BUG",
	"ERR_PRINTER_ON_FIRE"
};

MTMidiFileStream::~MTMidiFileStream()
{
    close_file();
}

Error MTMidiFileStream::open_to_read(String file_path)
{
    if (!file.is_null() && file->is_open())
    {
        return Error::ERR_FILE_ALREADY_IN_USE;
    }
    
    if (FileAccess::file_exists(file_path))
    {
        file = FileAccess::open(file_path, FileAccess::ModeFlags::READ);
        if (!file.is_null())
        {
            mode = FileAccess::ModeFlags::READ;
            size_bytes = file->get_length();
            return Error::OK;
        }

        return FileAccess::get_open_error();
    }
    return Error::ERR_FILE_NOT_FOUND;
}

Error MTMidiFileStream::open_to_write(String file_path, bool overwrite)
{
    if (!file.is_null() && file->is_open())
    {
        return Error::ERR_FILE_ALREADY_IN_USE;
    }

    if (FileAccess::file_exists(file_path) && !overwrite)
    {
        return Error::ERR_ALREADY_EXISTS;
    }

    file = FileAccess::open(file_path, FileAccess::ModeFlags::WRITE);
    if (!file.is_null())
    {
        mode = FileAccess::ModeFlags::WRITE;
        size_bytes = 0;
        return Error::OK;
    }

    return FileAccess::get_open_error();
}

bool MTMidiFileStream::close_file()
{
    if (!file.is_null() && file->is_open())
    {
        file->close();
        file->unreference();
        return true;
    }
    return false;
}

uint64_t MTMidiFileStream::get_length_bytes()
{
    if (!file.is_null() && file->is_open())
    {
        return size_bytes;
    }
    return 0;
}

uint64_t MTMidiFileStream::get_file_position()
{
    if (!file.is_null() && file->is_open())
    {
        return file->get_position();
    }
    return 0;
}

uint64_t MTMidiFileStream::get_readable_byte_count()
{
    if (!file.is_null() && file->is_open() && (mode == FileAccess::ModeFlags::READ))
    {
        return size_bytes - file->get_position();
    }
    return 0;
}

bool MTMidiFileStream::can_read(uint64_t count)
{
    if (!file.is_null() && file->is_open() && (mode == FileAccess::ModeFlags::READ))
    {
        return (size_bytes - file->get_position()) >= count;
    }
    return false;
}

bool MTMidiFileStream::can_write()
{
    if (!file.is_null() && file->is_open() && (mode == FileAccess::ModeFlags::WRITE))
    {
        return true;
    }
    return false;
}

Error MTMidiFileStream::read_bytes(uint64_t count, PackedByteArray buffer)
{
    if (can_read(count))
    {
        PackedByteArray buf = file->get_buffer(count);
        if (buf.size() == count)
        {
            buffer.append_array(buf);
            return Error::OK;
        }
        return file->get_error();
    }
    return Error::ERR_FILE_CANT_READ;
}

Error MTMidiFileStream::write_bytes(PackedByteArray buffer)
{
    if (can_write())
    {
        if (file->store_buffer(buffer))
        {
            size_bytes += buffer.size();
            return Error::OK;
        }

        return file->get_error();
    }
    return Error::ERR_FILE_CANT_WRITE;
}

Error MTMidiFileStream::read_uint32(uint32_t& value_read)
{
    PackedByteArray data;
    Error result = read_bytes(4, data);
    if (result == Error::OK)
    {
        value_read = (data[0] << 24) | 
                     (data[1] << 16) | 
                     (data[2] << 8) |
                     data[3];
    }
    return result;
}

Error MTMidiFileStream::write_uint32(uint32_t value)
{
    if (can_write())
    {
        PackedByteArray data;

        for (int i = 0; i < 4; ++i)
        {
            data.append(value & 0xFF);
            value = value >> 8;
        }

        data.reverse();

        if (file->store_buffer(data))
        {
            size_bytes += 4;
            return Error::OK;
        }

        return file->get_error();
    }
    return Error::ERR_FILE_CANT_WRITE;
}

Error MTMidiFileStream::read_uint16(uint16_t& value_read)
{
    PackedByteArray data;
    Error result = read_bytes(2, data);
    if (result == Error::OK)
    {
        value_read = (data[0] << 8) | 
                     data[1];
    }
    return result;
}

Error MTMidiFileStream::write_uint16(uint16_t value)
{
    if (can_write())
    {
        PackedByteArray data;
        for (int i = 0; i < 2; ++i)
        {
            data.append(value & 0xFF);
            value = value >> 8;
        }

        data.reverse();

        if (file->store_buffer(data))
        {
            size_bytes += 2;
            return Error::OK;
        }

        return file->get_error();
    }
    return Error::ERR_FILE_CANT_WRITE;
}

Error MTMidiFileStream::read_uint8(uint8_t& value_read)
{
    if (can_read(1))
    {
        value_read = file->get_8();
        return Error::OK;
    }
    return Error::ERR_FILE_CANT_READ;
}

Error MTMidiFileStream::write_uint8(uint8_t value)
{
    if (can_write())
    {
        if (file->store_8(value))
        {
            size_bytes++;
            return Error::OK;
        }

        return file->get_error();
    }
    return Error::ERR_FILE_CANT_WRITE;
}

Error MTMidiFileStream::read_variable_length_value(uint32_t& value, uint32_t& read_count)
{
    bool more_data = true;
    PackedByteArray buffer;
    uint8_t data;

    read_count = 0;

    while (more_data)
    {
        Error result = read_uint8(data);
        if (result == Error::OK)
        {
            read_count++;
            buffer.append(data & 0x7F);
            more_data = data & 0x80;
        }
        else
        {
            return result;
        }
    }

    if ((read_count > 0) && (read_count <= 4))
    {
        value = variable_length_to_uint32(buffer);
        return Error::OK;
    }
    
    return Error::ERR_PARSE_ERROR;
}

Error MTMidiFileStream::write_variable_length_value(uint32_t value, uint32_t& write_count)
{
    if (can_write())
    {
        PackedByteArray buffer = uint32_to_variable_length(value);

        if (file->store_buffer(buffer))
        {
            write_count = buffer.size();
            size_bytes += write_count;
            return Error::OK;
        }

        return file->get_error();
    }
    return Error::ERR_FILE_CANT_WRITE;
}

uint32_t MTMidiFileStream::variable_length_to_uint32(PackedByteArray value)
{
    uint32_t return_value = 0;
    for (uint8_t cur_byte : value)
    {
        return_value <<= 7;
        return_value += cur_byte & 0x7F;
    }
    return return_value;
}

PackedByteArray MTMidiFileStream::uint32_to_variable_length(uint32_t value)
{
    PackedByteArray var_len_value;
    do {
        uint8_t cur_byte = value & 0x7F;
        if (var_len_value.size() > 0)
        {
            cur_byte |= 0x80;
        }
        var_len_value.append(cur_byte);
        value >>= 7;
    } while (value > 0);

    var_len_value.reverse();

    return var_len_value;
}

uint32_t MTMidiFileStream::length_as_variable_length(uint32_t value)
{
    if (value < 128) return 1;
    if (value < 16384) return 2;
    if (value < 2097152) return 3;
    if (value < 268435456) return 4;
    return 5;
}

Error MTMidiFileStream::read_chunk_header(MIDIChunkHeader header)
{
    if (can_read(8))
    {
        PackedByteArray type_array;
        uint32_t chunk_length = 0;
        Error result = read_bytes(4, type_array);
        if (result == Error::OK)
        {
            result = read_uint32(chunk_length);
        }
        else
        {
            return result;
        }

        if (result == Error::OK)
        {
            header.chunk_length = chunk_length;
            if (type_array == MIDIChunkHeader::CHUNK_TYPE_FILE)
            {
                header.chunk_type = MIDIChunkHeader::HeaderType::File;
                return read_bytes(chunk_length, header.header_data);
            }
            else if (type_array == MIDIChunkHeader::CHUNK_TYPE_TRACK)
            {
                header.chunk_type = MIDIChunkHeader::HeaderType::Track;
            }
            else
            {
                header.chunk_type = MIDIChunkHeader::HeaderType::Unknown;
            }
        }
        return result;
    }
    return Error::ERR_FILE_CANT_READ;
}

Error MTMidiFileStream::write_chunk_header(MIDIChunkHeader header)
{
    if (header.chunk_type == MIDIChunkHeader::HeaderType::Unknown)
    {
        return Error::ERR_INVALID_DATA;
    }

    if (can_write())
    {
        PackedByteArray buffer = (header.chunk_type == MIDIChunkHeader::HeaderType::File) ?
                                 MIDIChunkHeader::CHUNK_TYPE_FILE : MIDIChunkHeader::CHUNK_TYPE_TRACK;

        Error result = write_bytes(buffer);

        if (result != Error::OK)
        {
            return result;
        }

        size_bytes += 4;
        result = write_uint32(header.chunk_length);

        if (result != Error::OK)
        {
            return result;
        }

        size_bytes += 4;

        if (header.chunk_type == MIDIChunkHeader::HeaderType::File)
        {
            result = write_bytes(header.header_data);
            if (result == Error::OK)
            {
                size_bytes += header.header_data.size();
            }
        }

        return result;
    }
    return Error::ERR_FILE_CANT_WRITE;
}
