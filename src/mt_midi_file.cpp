#include "mt_midi_file.hpp"
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/core/memory.hpp>

using namespace godot;

void MTMidiFile::_bind_methods() {
	ClassDB::bind_method(D_METHOD("read_file", "file_path"), &MTMidiFile::read_file);
	ClassDB::bind_method(D_METHOD("write_file", "file_path", "overwrite"), &MTMidiFile::write_file);
	ClassDB::bind_method(D_METHOD("update_file_name", "file_path"), &MTMidiFile::update_file_name);
	ClassDB::bind_method(D_METHOD("build_playable_msg_list"), &MTMidiFile::build_playable_msg_list);
	ClassDB::bind_method(D_METHOD("get_last_error"), &MTMidiFile::get_last_error);
}

MTMidiFile::MTMidiFile(){}

MTMidiFile::~MTMidiFile()
{
    for (KeyValue<uint32_t, MTMidiTrack*> element : tracks)
    {
        memdelete(element.value);
    }

    //memdelete(playable_list);
}

bool MTMidiFile::read_file(String file_path)
{
    bool success = true;
    MTMidiFileStream file_stream;
    last_error = file_stream.open_to_read(file_path);
    if (last_error == Error::OK)
    {
        // Set file name
        update_file_name(file_path);

        if (process_file_header(file_stream))
        {
            for (int current_track = 0; current_track < track_count && (last_error == Error::OK); ++current_track)
            {
                MTMidiTrack *track = MTMidiTrack::read_track(file_stream, current_track, last_error);

                if (last_error == Error::OK)
                {
                    if (track->TrackMsgs().size() > 0)
                    {
                        tracks.insert(current_track, track);
                    }
                }
                else
                {
                    success = false;
                    WARN_PRINT_ED(vformat("Error reading track: %s", file_stream.ErrorMsgs[last_error]));
                }
            }
        }
        else
        {
            success = false;
            WARN_PRINT_ED(vformat("Error reading file header: %s", file_stream.ErrorMsgs[last_error]));
        }

        file_stream.close_file();
    }
    else
    {
        // Error, could not open file
        WARN_PRINT_ED(vformat("Could not open file: %s : Error - %s", file_path, file_stream.ErrorMsgs[last_error]));
        success = false;
    }
    return success;
}

bool MTMidiFile::write_file(String file_path, bool overwrite)
{
    bool success = false;

    MTMidiFileStream file_stream;
    last_error = file_stream.open_to_write(file_path, overwrite);
    if (last_error == Error::OK)
    {
        MIDIChunkHeader header(MIDIChunkHeader::HeaderType::File, 6);
        success = header.set_format(file_format) &&
                  header.set_track_count(track_count) &&
                  header.set_division(ticks_per_quarter);

        if (success)
        {
            last_error = file_stream.write_chunk_header(header);
        }

        if (success && (last_error == Error::OK))
        {
            for (KeyValue<uint32_t, MTMidiTrack*> element : tracks)
            {
                MTMidiTrack *track = element.value;
                MIDIChunkHeader track_header(MIDIChunkHeader::HeaderType::Track, track->get_length_in_bytes());

                last_error = file_stream.write_chunk_header(track_header);

                if (last_error == Error::OK)
                {
                    last_error = track->write_events_to_stream(file_stream);
                    success = last_error == Error::OK;
                }

                if (!success)
                {
                    break;
                }
            }
        }

        file_stream.close_file();

        if (success)
        {
            mark_all_tracks_saved();
        }
    }

    if (!success)
    {
        if (last_error == Error::OK)
        {
            WARN_PRINT_ED("Error setting file header data.");
        }
        else
        {
            WARN_PRINT_ED(vformat("Error writing file: %s", file_stream.ErrorMsgs[last_error]));
        }
    }

    return success;
}

bool MTMidiFile::process_file_header(MTMidiFileStream fileStream)
{
    bool success = true;
    MIDIChunkHeader chunk_header(MIDIChunkHeader::HeaderType::Unknown, 0);
    last_error = fileStream.read_chunk_header(chunk_header);
    if ((last_error == Error::OK) && (chunk_header.chunk_type == MIDIChunkHeader::HeaderType::File))
    {
        if (chunk_header.chunk_length != 6)
        {
            WARN_PRINT_ED(vformat("Unexpected file header length: %d", chunk_header.chunk_length));
            success = false;
        }

        if (success)
        {
            uint16_t format = chunk_header.get_format();
            uint16_t division = chunk_header.get_division();
            if ((format != 0) && (format != 1))
            {
                WARN_PRINT_ED(vformat("Unexpected file format: %d", file_format));
                success = false;
            }

            // Read division type and values
            if (((division >> 8) & 0x80) == 0)
            {
                // Division is Ticks per Quarter Note
                ticks_per_quarter = division;
                // Set tick length to the default tempo 120 bpm
                usecs_per_tick = 500000.0 / ticks_per_quarter;
            }
            else
            {
                // Division is SMPTE time code
                WARN_PRINT_ED("File uses SMPTE time code.");
                smpte_format = (division >> 8) & 0xFF;
                ticks_per_frame = division & 0xFF;
                usecs_per_tick = 1000000.0 / abs(smpte_format) / ticks_per_frame;
            }
        }
    }
    else
    {
        // Error, unrecognized file header
        WARN_PRINT_ED(vformat("Error reading MIDI file header: %s", fileStream.ErrorMsgs[last_error]));
        success = false;
    }
    return success;
}

void MTMidiFile::update_file_name(String file_path)
{
    file_path_full = file_path;
    file_name = file_path.get_file();
}

void MTMidiFile::mark_all_tracks_saved()
{
    for (KeyValue<uint32_t, MTMidiTrack*> element : tracks)
    {
        MTMidiTrack* track = element.value;
        track->contains_unsaved_edits = false;
    }
}

MTMidiMsgList* MTMidiFile::build_playable_msg_list()
{
/*    bool success = true;
    List<MTMidiMsg*> msgList;
    // Iterator for end comparison for all List<MTMidiMsg> iterators,
    // just an iterator with element pointer = nullptr
    List<MTMidiMsg*>::Iterator end;
    if (tracks.size() > 0)
    {
        float totalMsgCount = 0;
        float processedMsgCount = 0;

        List<List<MTMidiMsg*>::Iterator> track_iters;
        for (KeyValue<uint32_t, MTMidiTrack*> element : tracks)
        {
            MTMidiTrack track = *element.value;
            end = track.TrackMsgs().end();
            if (track.TrackMsgs().size() > 0)
            {
                track_iters.push_back(track.TrackMsgs().begin());
                totalMsgCount += track.TrackMsgs().size();
            }
        }

        while ((processedMsgCount < totalMsgCount) && (track_iters.size() > 0))
        {
            ++processedMsgCount;

            uint32_t current_track = 0;
            for (int x = 1; x < track_iters.size(); ++x)
            {
                List<MTMidiMsg*>::Iterator current = track_iters.get(current_track);
                List<MTMidiMsg*>::Iterator check = track_iters.get(x);
                if ((*check)->tick < (*current)->tick)
                {
                    current_track = x;
                }
            }
            List<MTMidiMsg*>::Iterator next = track_iters.get(current_track);
            MTMidiMsg* currentMsg = *next;
            ++next;

            msgList.push_back(currentMsg);

            if (next == end)
            {
                track_iters.erase(next);

                // Check that the last event in the track was an EndOfTrack event
                if (currentMsg->get_status_byte() == MTMidiMsg::NonChMsgType::Meta)
                {
                    if (currentMsg->get_meta_msg_type() != MTMidiMsg::MetaMsgType::EndOfTrack)
                    {
                        WARN_PRINT_ED("Track ended with Meta event that was not EndOfTrack: Type: " + currentMsg->get_meta_msg_type());
                        success = false;
                    }
                }
                else
                {
                    WARN_PRINT_ED("Track ended without Meta EndOfTrack event - Last event type: " + currentMsg->get_status_byte());
                    success = false;
                }
            }
        }
    }
    else
    {
        success = false;
        WARN_PRINT_ED("No tracks in file");
    }

    if (!success)
    {
        msgList.clear();
    }
*/
    return memnew(MTMidiMsgList());
}
