#include "mt_midi_track.hpp"
#include "mt_midi_file_stream.hpp"

using namespace godot;

MTMidiTrack::~MTMidiTrack()
{
    for (MTMidiMsg* msg : msgs)
    {
        memdelete(msg);
    }
}

MTMidiTrack *MTMidiTrack::read_track(
    MTMidiFileStream file_stream,
    int track_id,
    Error& result)
{
    bool success = true;

    MIDIChunkHeader header(MIDIChunkHeader::HeaderType::Unknown, 0);
    result = file_stream.read_chunk_header(header);
    if (result != Error::OK)
    {
        WARN_PRINT_ED("Could not read MIDI track chunk header.");
        return nullptr;
    }

    if (header.chunk_type != MIDIChunkHeader::HeaderType::Track)
    {
        WARN_PRINT_ED(vformat("Skipping track due to unrecognized header type: %d", header.chunk_type));
        if (header.chunk_length <= file_stream.get_readable_byte_count())
        {
            PackedByteArray bit_bucket;
            result = file_stream.read_bytes(header.chunk_length, bit_bucket);
        }
        else
        {
            result == Error::ERR_FILE_EOF;
        }
        return nullptr;
    }

    if (file_stream.get_readable_byte_count() < header.chunk_length)
    {
        result = Error::ERR_FILE_EOF;
        WARN_PRINT_ED(vformat("Not enough data to read MTMidiTrack #%d, Remaining: %d, Track length: %d",
            String::num_uint64(track_id), file_stream.get_readable_byte_count(), header.chunk_length));
        return nullptr;
    }

    MTMidiTrack* track = memnew(MTMidiTrack(track_id));

    PackedByteArray buffer;
    result = file_stream.read_bytes(header.chunk_length, buffer);
    if (result != Error::OK)
    {
        WARN_PRINT_ED("Unexpected error reading track data into buffer.");
        memdelete(track);
        return nullptr;
    }

    int bytes_read = 0;
    uint64_t tick = 0;
    uint8_t running_status = 0;
    uint8_t channel_prefix = 0;
    uint8_t port_prefix = 0;
    uint32_t tick_delta;
    uint32_t delta_size;

    MTDataBuffer *data_buffer = memnew(MTDataBuffer(buffer));                    

    if (data_buffer != nullptr)
    {
        // TODO: Add type 2 support: Check for a Sequence Number Meta message, which
        // must occur before any non-zero tick deltas.

        // TODO: Add SMPTE timecode support: Check for a SMPTE Offset message, which
        // must occur before any non-zero tick deltas.

        while (success && (data_buffer->unread_count() > 0) && (bytes_read < header.chunk_length))
        {
            if (data_buffer->read_variable_length(tick_delta, delta_size) == Error::OK)
            {
                bytes_read += delta_size;
                tick += tick_delta;

                int32_t read_length;
                MTMidiMsg* readEvt = MTMidiMsg::read_msg(tick, running_status,
                                        channel_prefix, port_prefix, *data_buffer, read_length);

                if (read_length != -1)
                {
                    bytes_read += read_length;
                    track->msgs.push_back(readEvt);
                }
                else
                {
                    WARN_PRINT_ED(vformat("Unrecoverable error reading MIDI message in track %d", track_id));
                    success = false;
                }
            }
            else
            {
                WARN_PRINT_ED(vformat("Unrecoverable error reading tick delta VLV in track %d", track_id));
                success = false;
            }
        }

        memdelete(data_buffer);
    }

    if (bytes_read != header.chunk_length)
    {
        WARN_PRINT_ED(vformat("Error in track #%d : Bytes read count %d does not match chunk length %d",
            String::num_uint64(track_id), bytes_read, header.chunk_length));
        success = false;
    }

    if (!success)
    {
        result = Error::ERR_PARSE_ERROR;
        memdelete(track);
        return nullptr;
    }

    return track;
}

Error MTMidiTrack::write_events_to_stream(MTMidiFileStream file_stream)
{
    uint64_t current_tick = 0;

    Error result;
    for (MTMidiMsg* msg : msgs)
    {
        PackedByteArray data = msg->to_array(current_tick);
        if (data.size() > 0)
        {
            result = file_stream.write_bytes(data);
        }
    }
    return result;
}

int MTMidiTrack::get_length_in_bytes()
{
    int data_length = 0;
    uint64_t current_tick = 0;

    for (MTMidiMsg* msg : msgs)
    {
        data_length += msg->length_in_bytes(current_tick);
    }

    return data_length;
}

void MTMidiTrack::update_meta_data()
{
    max_note_value = -1;
    min_note_value = 128;
    channels_used.clear();
    channels_with_notes.clear();
    note_values.clear();
    track_type = TrackType::Unknown;

    for (MTMidiMsg* msg : msgs)
    {
        int32_t type = msg->get_channel_msg_type();
        switch(type)
        {
            case MTMidiMsg::ChannelMsgType::NonChannel:
                if (track_type == TrackType::Unknown)
                {
                    track_type = TrackType::Meta;
                }
                break;
            default:
                uint8_t ch = msg->get_channel();
                channels_used.insert(ch);
                track_type = ch == 9 ? TrackType::Drum : TrackType::Note;
                if (type == MTMidiMsg::ChannelMsgType::NoteOn || type == MTMidiMsg::ChannelMsgType::NoteOff)
                {
                    uint8_t noteVal = msg->get_note_value();
                    min_note_value = noteVal < min_note_value ? noteVal : min_note_value;
                    max_note_value = noteVal > max_note_value ? noteVal : max_note_value;
                    note_values.insert(noteVal);
                    channels_with_notes.insert(ch);
                }
                break;
        }
    }
}

PackedByteArray MTMidiTrack::get_note_values()
{
    PackedByteArray vals;
    if (note_values.size() > 0)
    {
        for (uint8_t val : note_values)
        {
            vals.append(val);
        }
    }
    return vals;
}
