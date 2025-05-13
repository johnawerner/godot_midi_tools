#include "mt_midi_msg.hpp"

using namespace godot;

// Initialize MTMidiMsg static message id sequence
uint64_t MTMidiMsg::next_msg_id = 0;

void MTMidiMsg::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_id"), &MTMidiMsg::get_id);
	ClassDB::bind_method(D_METHOD("get_tick"), &MTMidiMsg::get_tick);
	ClassDB::bind_method(D_METHOD("get_data_length"), &MTMidiMsg::get_data_length);
	ClassDB::bind_method(D_METHOD("get_data_start"), &MTMidiMsg::get_data_start);
	ClassDB::bind_method(D_METHOD("get_channel_prefix"), &MTMidiMsg::get_channel_prefix);
	ClassDB::bind_method(D_METHOD("get_status_byte"), &MTMidiMsg::get_status_byte);
	ClassDB::bind_method(D_METHOD("get_note_value"), &MTMidiMsg::get_note_value);
	ClassDB::bind_method(D_METHOD("get_note_velocity"), &MTMidiMsg::get_note_velocity);
	ClassDB::bind_method(D_METHOD("get_channel_msg_type"), &MTMidiMsg::get_channel_msg_type);
	ClassDB::bind_method(D_METHOD("get_channel"), &MTMidiMsg::get_channel);
	ClassDB::bind_method(D_METHOD("read_data_value", "index"), &MTMidiMsg::read_data_value);
	ClassDB::bind_method(D_METHOD("get_meta_msg_type"), &MTMidiMsg::get_meta_msg_type);
	ClassDB::bind_method(D_METHOD("get_meta_msg_text"), &MTMidiMsg::get_meta_msg_text);
    ClassDB::bind_method(D_METHOD("read_tempo"), &MTMidiMsg::read_tempo);
    ClassDB::bind_method(D_METHOD("get_msg_as_bytes"), &MTMidiMsg::get_msg_as_bytes);
}

MTMidiMsg::MTMidiMsg()
{
    id = next_msg_id++;
    tick = 0;
}

MTMidiMsg::MTMidiMsg(uint64_t tick, uint8_t statusByte, int32_t dataLength) :
    tick(tick)
{
    id = next_msg_id++;
    msg_bytes.resize(dataLength + 1);
    msg_bytes.set(0, statusByte);
}

MTMidiMsg::MTMidiMsg(uint64_t tick, PackedByteArray msg_as_bytes) :
    tick(tick)
{
    id = next_msg_id++;
    msg_bytes.append_array(msg_as_bytes);
}

int32_t MTMidiMsg::get_status_byte()
{
    if (msg_bytes.size() > 0)
    {
        return msg_bytes[0];
    }
    return -1;
}

int32_t MTMidiMsg::get_note_value()
{
    if (msg_bytes.size() > 1)
    {
        return msg_bytes[1];
    }
    return -1;
}

int32_t MTMidiMsg::get_note_velocity()
{
    if (msg_bytes.size() > 2)
    {
        return msg_bytes[2];
    }
    return -1;
}

int32_t MTMidiMsg::get_channel_msg_type()
{
    if (msg_bytes.size() > 0)
    {
        return msg_bytes[0] & 0xF0;
    }
    return -1;
}

int32_t MTMidiMsg::get_channel()
{
    if (msg_bytes.size() > 0)
    {
        return msg_bytes[0] & 0x0F;
    }
    return -1;
}

PackedByteArray MTMidiMsg::get_msg_as_bytes()
{
    return msg_bytes;
}

uint8_t MTMidiMsg::read_data_value(int32_t index)
{
    if ((dataLength > index) &&
        (msg_bytes.size() > dataStart + index))
    {
        return msg_bytes[dataStart + index];
    }
    return 0xFF;
}

PackedByteArray MTMidiMsg::copy_binary_data()
{
    return msg_bytes.duplicate();
}

uint8_t MTMidiMsg::get_meta_msg_type()
{
    if ((msg_bytes.size() > 1) &&
        (msg_bytes[0] == NonChMsgType::Meta))
    {
        return msg_bytes[1];
    }
    return 0xFF;
}

String MTMidiMsg::get_meta_msg_text()
{
    PackedByteArray buffer;

    uint8_t metaType = get_meta_msg_type();

    if ((msg_bytes.size() > 3) &&
        (metaType >= MetaMsgType::TextEvent) &&
        (metaType <= MetaMsgType::CuePoint) &&
        (dataLength > 0) &&
        (dataStart < msg_bytes.size()) &&
        (dataStart + dataLength <= msg_bytes.size()))
    {
        buffer.append_array(msg_bytes.slice(dataStart, dataStart + dataLength));
    }
    else
    {
        WARN_PRINT_ED("MTMidiMsg: Could not read Meta message text, either wrong type or no data");
    }

    return buffer.get_string_from_utf8();
}

MTMidiMsg *MTMidiMsg::read_msg(
    uint64_t tick,
    uint8_t& running_status,
    uint8_t& channel_prefix,
    uint8_t& port_prefix,
    MTDataBuffer buffer,
    int32_t& bytes_read)
{
    bytes_read = 0;

    uint8_t status_byte = running_status;
    if ((buffer.unread_count() > 0) && (is_status_byte(buffer.peek())))
    {
        buffer.read_byte(status_byte);
        ++bytes_read;
    }

    int32_t read_count;
    switch (status_byte & 0xF0)
    {
        case ChannelMsgType::NoteOff:
        case ChannelMsgType::NoteOn:
        case ChannelMsgType::PolyKeyPressure:
        case ChannelMsgType::ControlChange:
        case ChannelMsgType::ProgramChange:
        case ChannelMsgType::ChannelPressure:
        case ChannelMsgType::PitchBend:
            {
                // Store running status
                running_status = status_byte;
                MTMidiMsg* msg = read_channel_msg(tick, status_byte, buffer, read_count);
                bytes_read = read_count > -1 ? bytes_read + read_count : read_count;
                if (msg != nullptr)
                {
                    channel_prefix = msg->get_channel();
                    msg->portPrefix = port_prefix;
                }
                return msg;
            }
            break;
        case ChannelMsgType::NonChannel:
            switch (status_byte)
            {
                case NonChMsgType::SysexStart: // Sysex Msg Start event
                case NonChMsgType::SysexContOrEsc: // Sysex Msg Continue or Escape event
                    {
                        MTMidiMsg* msg = read_sysex_msg(tick, status_byte, buffer, read_count);
                        if (msg != nullptr)
                        {
                            msg->portPrefix = port_prefix;
                        }
                        bytes_read = read_count > -1 ? bytes_read + read_count : read_count;
                        return msg;
                    }
                    break;
                case NonChMsgType::Meta: // Meta event
                    {
                        MTMidiMsg* msg = read_meta_msg(tick, status_byte, buffer, read_count);
                        bytes_read = read_count > -1 ? bytes_read + read_count : read_count;
                        if (msg != nullptr)
                        {
                            if (msg->get_meta_msg_type() == MetaMsgType::ChannelPrefix)
                            {
                                channel_prefix = msg->read_data_value(0);
                            }

                            if (msg->get_meta_msg_type() == MetaMsgType::PortPrefix)
                            {
                                port_prefix = msg->read_data_value(0);
                            }

                            msg->channelPrefix = channel_prefix;
                            msg->portPrefix = port_prefix;
                        }
                        return msg;
                    }
                    break;
                default: // Unknown event
                    WARN_PRINT_ED("Unrecognized event type: " + status_byte);
                    break;
            }
            break;
    }
    return nullptr;
}

MTMidiMsg *MTMidiMsg::read_channel_msg(
    uint64_t ticks,
    uint8_t status_byte,
    MTDataBuffer buffer,
    int32_t& bytes_read)
{
    bytes_read = 0;

    uint8_t db1 = 0;
    uint8_t db2 = UINT8_MAX;
    switch (status_byte & 0xF0)
    {
        case ChannelMsgType::NonChannel:
            WARN_PRINT_ED("Found unrecognized message in ReadChannelMsg");
            bytes_read = -1;
            break;
        case ChannelMsgType::NoteOff:
        case ChannelMsgType::NoteOn:
            if ((buffer.read_byte(db1) == Error::OK) && (buffer.read_byte(db2) == Error::OK))
            {
                db1 &= 0x7F;
                db2 &= 0x7F;
                bytes_read = 2;
            }
            else
            {
                WARN_PRINT_ED("Not enough data available to read Note msg");
                bytes_read = -1;
            }
            break;
        case ChannelMsgType::ProgramChange:
        case ChannelMsgType::ChannelPressure:
            if (buffer.read_byte(db1) == Error::OK)
            {
                bytes_read = 1;
            }
            else
            {
                WARN_PRINT_ED("Not enough data to read single-byte MTMidiMsg");
                bytes_read = -1;
            }
            break;
        default:
            if ((buffer.read_byte(db1) == Error::OK) && (buffer.read_byte(db2) == Error::OK))
            {
                bytes_read = 2;
            }
            else
            {
                WARN_PRINT_ED("Not enough data to read double-byte FSMidiMsg");
                bytes_read = -1;
            }
            break;
    }

    if (bytes_read > 0)
    {
        MTMidiMsg *new_msg = memnew(MTMidiMsg(ticks, status_byte, bytes_read));
        if (bytes_read == 2)
        {
            new_msg->msg_bytes[1] = db1;
            new_msg->msg_bytes[2] = db2;
        }
        else if (bytes_read == 1)
        {
            new_msg->msg_bytes[1] = db1;
        }
        return new_msg;
    }

    return nullptr;
}

MTMidiMsg *MTMidiMsg::read_meta_msg(
    uint64_t ticks,
    uint8_t status_byte,
    MTDataBuffer buffer,
    int32_t& bytes_read)
{
    bytes_read = 0;

    if (status_byte != NonChMsgType::Meta)
    {
        WARN_PRINT_ED("Unrecognized event type while reading MTMidiMsg: " + status_byte );
        return nullptr;
    }

    uint8_t meta_type;
    if (buffer.read_byte(meta_type) == Error::OK)
    {
        ++bytes_read;

        uint32_t vlLength;
        uint32_t vlValue;
        if (buffer.read_variable_length(vlValue, vlLength) == Error::OK)
        {
            bytes_read += vlLength;

            Error result;
            PackedByteArray tmp_buffer = buffer.read_bytes(vlValue, result);
            if (result == Error::OK)
            {
                bytes_read += vlValue;
                MTMidiMsg *msg = memnew(MTMidiMsg(ticks, status_byte, 0));
                msg->msg_bytes.append(meta_type);
                PackedByteArray tmp = MTMidiFileStream::uint32_to_variable_length(vlValue);
                msg->msg_bytes.append_array(tmp);
                msg->msg_bytes.append_array(tmp_buffer);
                msg->dataLength = vlValue;
                msg->dataStart = 2 + tmp.size();
                return msg;
            }
        }
    }

    WARN_PRINT_ED("Ran out of data while reading MTMidiMsg");
    bytes_read = -1;
    return nullptr;
}

MTMidiMsg *MTMidiMsg::read_sysex_msg(
    uint64_t ticks,
    uint8_t status_byte,
    MTDataBuffer buffer,
    int32_t& bytes_read)
{
    bytes_read = 0;

    if ((status_byte != NonChMsgType::SysexStart) &&
        (status_byte != NonChMsgType::SysexContOrEsc))
    {
        WARN_PRINT_ED("Unrecognized event type while reading FSMidiMsg: " + status_byte);
        bytes_read = -1;
        return nullptr;
    }

    uint32_t vlLength;
    uint32_t vlValue;
    if (buffer.read_variable_length(vlValue, vlLength) == Error::OK)
    {
        bytes_read += vlLength;

        Error result;
        PackedByteArray tmp_buffer = buffer.read_bytes(vlValue, result);
        if (result == Error::OK)
        {
            bytes_read += vlValue;
            MTMidiMsg *msg = memnew(MTMidiMsg(ticks, status_byte, 0));
            PackedByteArray tmp = MTMidiFileStream::uint32_to_variable_length(vlValue);
            msg->msg_bytes.append_array(tmp);
            msg->msg_bytes.append_array(tmp_buffer);
            msg->dataLength = vlValue;
            msg->dataStart = 1 + tmp.size();
            return msg;
        }
    }

    WARN_PRINT_ED("Not enough data while reading MTMidiMsg");
    bytes_read = -1;
    return nullptr;
}

PackedByteArray MTMidiMsg::to_array(uint64_t &currentTick)
{
    PackedByteArray data;
    data.append_array(MTMidiFileStream::uint32_to_variable_length(tick - currentTick));
    data.append_array(msg_bytes);
    currentTick = tick;
    return data;
}

int32_t MTMidiMsg::length_in_bytes(uint64_t &currentTick)
{
    int32_t length = MTMidiFileStream::length_as_variable_length(tick - currentTick);
    length += msg_bytes.size();
    currentTick = tick;
    return length;
}

int32_t MTMidiMsg::read_tempo()
{
    int32_t tempo = 500000;
    if ((msg_bytes.size() == 6) &&
        (msg_bytes[0] == NonChMsgType::Meta) &&
        (msg_bytes[1] == MetaMsgType::SetTempo))
    {
        tempo = (msg_bytes[3] << 16) + (msg_bytes[4] << 8) + msg_bytes[5];
    }
    else
    {
        WARN_PRINT_ED("Attempt to read tempo from wrong message type");
    }
    return tempo;
}
