#ifndef MT_MIDI_MSG_LIST_H
#define MT_MIDI_MSG_LIST_H

#include "mt_midi_msg.hpp"
#include "mt_midi_track.hpp"

namespace godot {

class MTMidiMsgList : public Object {
	GDCLASS(MTMidiMsgList, Object)

    friend MTMidiTrack;

private:
    List<MTMidiMsg*> msg_list;
    List<MTMidiMsg*>::Iterator msg_iter;

protected:
	static void _bind_methods();
    void delete_msgs();
    void push_back(MTMidiMsg* msg);
    void insert_before_equal_tick(MTMidiMsg* msg);
    void insert_after_equal_tick(MTMidiMsg* msg);
    MTMidiMsg* remove(MTMidiMsg* msg);
    MTMidiMsg* remove_at(int index);

public:
    int count();

    int find_index(MTMidiMsg* msg);
    MTMidiMsg* get_at(int index);

    void reset_iterator();
    void iterate_to(MTMidiMsg* msg);
    MTMidiMsg* current_msg();
    MTMidiMsg* next();
};

}
#endif
