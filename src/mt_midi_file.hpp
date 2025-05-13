#ifndef MT_MIDI_FILE_H
#define MT_MIDI_FILE_H

#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/array.hpp>
#include "mt_midi_file_stream.hpp"
#include "mt_midi_track.hpp"
#include "mt_midi_msg.hpp"
#include "mt_midi_msg_list.hpp"

namespace godot {

class MTMidiFile : public RefCounted {
    GDCLASS(MTMidiFile, RefCounted)

    MTMidiMsgList *playable_list;
    
    protected:
	static void _bind_methods();
    bool process_file_header(MTMidiFileStream file_stream);
    void mark_all_tracks_saved();

    public:
    HashMap<uint32_t, MTMidiTrack*> tracks;
    uint16_t ticks_per_quarter = 384;
    double usecs_per_tick;
    uint16_t file_format = 1;
    uint16_t track_count = 0;
    int8_t smpte_format = 0;
    uint8_t ticks_per_frame = 0;
    String file_path_full;
    String file_name;
    Error last_error;

    MTMidiFile();
    ~MTMidiFile();

    bool read_file(String file_path);
    bool write_file(String file_path, bool overwrite);
    void update_file_name(String file_path);
    MTMidiMsgList* build_playable_msg_list();
    Error get_last_error() { return last_error; }
};
}
#endif
