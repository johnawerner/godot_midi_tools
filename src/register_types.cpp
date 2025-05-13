#include "register_types.h"

#include "mt_fluid_synth_node.hpp"
#include "mt_midi_file.hpp"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_miditools_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}

	GDREGISTER_CLASS(MTFluidSynthNode);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_NOTE_OFF", MTFluidSynthNode::MIDI_MSG_TYPE_NOTE_OFF);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_NOTE_ON", MTFluidSynthNode::MIDI_MSG_TYPE_NOTE_ON);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_POLY_KEY_PRESSURE", MTFluidSynthNode::MIDI_MSG_TYPE_POLY_KEY_PRESSURE);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_CONTROL_CHANGE", MTFluidSynthNode::MIDI_MSG_TYPE_CONTROL_CHANGE);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_PROGRAM_CHANGE", MTFluidSynthNode::MIDI_MSG_TYPE_PROGRAM_CHANGE);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_CHANNEL_PRESSURE", MTFluidSynthNode::MIDI_MSG_TYPE_CHANNEL_PRESSURE);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_PITCH_BEND", MTFluidSynthNode::MIDI_MSG_TYPE_PITCH_BEND);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_MSG_TYPE_SYSTEM", MTFluidSynthNode::MIDI_MSG_TYPE_SYSTEM);

    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_SYSTEM_EXCLUSIVE", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_SYSTEM_EXCLUSIVE);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_TIME_CODE_QTR_FRAME", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_TIME_CODE_QTR_FRAME);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_SONG_POSITION_PTR", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_SONG_POSITION_PTR);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_SONG_SELECT", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_SONG_SELECT);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_TUNE_REQUEST", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_TUNE_REQUEST);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_END_OF_EXCLUSIVE", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_END_OF_EXCLUSIVE);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_TIMING_CLOCK", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_TIMING_CLOCK);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_START", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_START);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_CONTINUE", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_CONTINUE);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_STOP", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_STOP);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_ACTIVE_SENSING", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_ACTIVE_SENSING);
    ClassDB::bind_integer_constant("MTFluidSynthNode", "", "MIDI_SYS_MSG_TYPE_SYSTEM_RESET", MTFluidSynthNode::MIDI_SYS_MSG_TYPE_SYSTEM_RESET);

	GDREGISTER_CLASS(MTMidiFile);
	GDREGISTER_CLASS(MTMidiMsgList);
	GDREGISTER_CLASS(MTMidiMsg);
}

void uninitialize_miditools_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
		return;
	}
}

extern "C" {
// Initialization.
GDExtensionBool GDE_EXPORT miditools_library_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
	godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);

	init_obj.register_initializer(initialize_miditools_module);
	init_obj.register_terminator(uninitialize_miditools_module);
	init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);

	return init_obj.init();
}
}