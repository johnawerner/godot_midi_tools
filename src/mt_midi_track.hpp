#ifndef MT_MIDI_TRACK_H
#define MT_MIDI_TRACK_H

#include <godot_cpp/templates/hash_set.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include "mt_midi_msg.hpp"


namespace godot {

class MTMidiTrack : public Object {
	GDCLASS(MTMidiTrack, Object)

private:
    List<MTMidiMsg*> msgs;
public:
    enum TrackType { Unknown = 0, Note = 1, Drum = 2, Meta = 3 };
	int32_t track_id;
	int32_t max_note_value = -1;
	int32_t min_note_value = 128;
	bool contains_unsaved_edits = false;
	HashSet<uint8_t> channels_used;
	HashSet<uint8_t> channels_with_notes;
	HashSet<uint8_t> note_values;
	TrackType track_type = TrackType::Unknown;

	List<MTMidiMsg*> TrackMsgs() { return msgs;}
    void TrackMsgs(List<MTMidiMsg*> value) { msgs = value; }

	MTMidiTrack(uint64_t id) : track_id(id) {}
    ~MTMidiTrack();
    static MTMidiTrack* read_track(MTMidiFileStream file_stream, int track_id, Error& result);
    Error write_events_to_stream(MTMidiFileStream file_stream);
    int32_t get_length_in_bytes();
    void update_meta_data();
    PackedByteArray get_note_values();
};
}
#endif
