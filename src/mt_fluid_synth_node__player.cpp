#include "mt_fluid_synth_node.hpp"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/error_macros.hpp>
#include <godot_cpp/classes/input_event_midi.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/classes/json.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <iostream>

using namespace godot;

int MTFluidSynthNode::player_create() {
    if (player != NULL) {
        WARN_PRINT_ED("FluidSynth player already exists");
        return -1;
    }

    player = new_fluid_player(synth);

    if (player == NULL) {
        WARN_PRINT_ED("Creating FluidSynth player failed");
        return -1;
    }

    return 0;
}

int MTFluidSynthNode::player_delete() {
    delete_fluid_player(player);
    player = NULL;
    return 0;
}

int MTFluidSynthNode::player_load_midi(String file_path) {

    if (fluid_player_add(player, file_path.ascii()) == FLUID_FAILED){
        WARN_PRINT_ED("FluidSynth player failed to load MIDI file");
        return -1;
    }

    return 0;
}

int MTFluidSynthNode::player_play(int loop_count) {
    if (player != NULL) {
        if (loop_count >= -1) {
            fluid_player_set_loop(player, loop_count);
        }

        if (fluid_player_play(player) == FLUID_FAILED) {
            WARN_PRINT_ED("FluidSynth player failed to play");
            return -1;
        }
    }
    return 0;
}

int MTFluidSynthNode::player_seek(int tick) {
    if (player != NULL) {
        if (fluid_player_seek(player, tick) == FLUID_FAILED) {
            WARN_PRINT_ED("FluidSynth player failed to seek in file");
            return -1;
        }
    }
    return 0;
}

int MTFluidSynthNode::player_stop() {
    if (player != NULL) {
        fluid_player_stop(player);
    }
    return 0;
}
