#include "mt_midi_msg_list.hpp"

using namespace godot;

void MTMidiMsgList::_bind_methods()
{
	ClassDB::bind_method(D_METHOD("count"), &MTMidiMsgList::count);
	ClassDB::bind_method(D_METHOD("find_index", "msg"), &MTMidiMsgList::find_index);
	ClassDB::bind_method(D_METHOD("get_at", "index"), &MTMidiMsgList::get_at);
	ClassDB::bind_method(D_METHOD("reset_iterator"), &MTMidiMsgList::reset_iterator);
	ClassDB::bind_method(D_METHOD("iterate_to", "msg"), &MTMidiMsgList::iterate_to);
	ClassDB::bind_method(D_METHOD("current_msg"), &MTMidiMsgList::current_msg);
	ClassDB::bind_method(D_METHOD("next"), &MTMidiMsgList::next);
}

void MTMidiMsgList::delete_msgs()
{
	for (MTMidiMsg *msg : msg_list)
	{
		memdelete(msg);
	}
}

/// @brief Returns the message count
/// @return int, message count
int MTMidiMsgList::count() { return msg_list.size(); }

/// @brief Appends a message to the list
/// Adds a MTMidiMsg at the end of the internal list
/// A protected method only useable by the MTMidiTrack friend class
/// @param msg Pointer to MTMidiMsg to add
void MTMidiMsgList::push_back(MTMidiMsg* msg){}

/// @brief Adds a message in tick order, before any messages with the same tick
/// Adds a message in ascending order, using the MTMidiMsg 'tick' attribute
/// The message is inserted before any messages with the same 'tick' value
/// A protected method only useable by the MTMidiTrack friend class
/// @param msg  Pointer to MTMidiMsg to add
void MTMidiMsgList::insert_before_equal_tick(MTMidiMsg* msg){}

/// @brief Adds a message in tick order, after any messages with the same tick
/// Adds a message in ascending order, using the MTMidiMsg 'tick' attribute
/// The message is inserted after any messages with the same 'tick' value
/// A protected method only useable by the MTMidiTrack friend class
/// @param msg  Pointer to MTMidiMsg to add
void MTMidiMsgList::insert_after_equal_tick(MTMidiMsg* msg){}

/// @brief Removes the message
/// Removes the given message, returning a pointer to the message on success
/// A protected method only useable by the MTMidiTrack friend class
/// @param msg Pointer to MTMidiMsg to remove
/// @return Pointer to removed MTMidiMsg on success, else a nullptr
MTMidiMsg* MTMidiMsgList::remove(MTMidiMsg* msg){ return nullptr; }

/// @brief Returns the zero-based index of the given message
/// @param msg Pointer to MTMidiMsg to find
/// @return int, index of MTMidiMsg pointer if found, else -1
int MTMidiMsgList::find_index(MTMidiMsg* msg){ return -1; }

/// @brief Returns the message at the given zero-based index
/// @param index int, zero-based index of desired message
/// @return Pointer to MTMidiMsg if index in bounds, else a nullptr
MTMidiMsg* MTMidiMsgList::get_at(int index){ return nullptr; }

/// @brief Removes the message at the given zero-based index
/// @param index int, zero-based index of message to be removed
/// @return Pointer to MTMidiMsg if index in bounds, else a nullptr
MTMidiMsg* MTMidiMsgList::remove_at(int index){ return nullptr; }

/// @brief Resets the internal iterator to the beginning of the list
void MTMidiMsgList::reset_iterator(){}

/// @brief Places the internal iterator at the given message
/// If the list contains the given message, the iterator will be
/// placed at the position of the message.
/// If the message is not found in the list, the iterator is unchanged.
/// @param msg Pointer to MTMidiMsg to find
void MTMidiMsgList::iterate_to(MTMidiMsg *msg){}

/// @brief Returns the message at the current position of the internal iterator
/// @return Pointer to MTMidiMsg if the iterator is valid, else a nullptr
MTMidiMsg* MTMidiMsgList::current_msg(){ return nullptr; }

/// @brief Advances the internal iterator and returns the message
/// @return Pointer to MTMidiMsg if the iterator is valid, else a nullptr
MTMidiMsg* MTMidiMsgList::next(){ return nullptr; }
