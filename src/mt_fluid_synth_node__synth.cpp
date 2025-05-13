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

int MTFluidSynthNode::synth_create(String sf_path, bool listen_ext_input) {
    // Don't create if synth exists
    if (synth != NULL) {
        WARN_PRINT_ED("FluidSynth instance exists, unload before creating a new one");
        return -1;
    }

    if (settings == NULL) {
        WARN_PRINT_ED("FluidSynth settings have not been created");
        return -1;
    }

    // Create the synthesizer
    synth = new_fluid_synth(settings);
    if(synth == NULL)
    {
        synth_delete();
        WARN_PRINT_ED("Failed to create FluidSynth");
        return -1;
    }

    // Load the soundfont
    int sfont_id = synth_soundfont_load(sf_path, true);
    if (sfont_id == -1) {
        synth_delete();
        return -1;
    }

    /* Create the audio driver. The synthesizer starts playing as soon
       as the driver is created. */
    adriver = new_fluid_audio_driver(settings, synth);
    if (adriver == NULL)
    {
        synth_delete();
        WARN_PRINT_ED("Failed to create audio driver for FluidSynth");
        return -1;
    }

    set_process_input(listen_ext_input);

    return sfont_id;
}

int MTFluidSynthNode::synth_soundfont_load(String sf_path, bool reset) {

    /* Load a SoundFont and reset presets (so that new instruments
     * get used from the SoundFont)
     * Depending on the size of the SoundFont, this will take some time to complete...
     */
    if (synth == NULL) {
        WARN_PRINT_ED("Create a FluidSynth instance before loading a SoundFont");
        return -1;
    }

    int cur_sfont_id = fluid_synth_sfload(synth, sf_path.ascii(), reset ? 1 : 0);
    if (cur_sfont_id == FLUID_FAILED) {
        WARN_PRINT_ED(vformat("Failed to load SoundFont: %s", sf_path));
        return -1;
    }

    return cur_sfont_id;
}

int MTFluidSynthNode::synth_soundfont_unload(int sfont_id) {
    if (synth == NULL) {
        WARN_PRINT_ED("No FluidSynth instance to unload SoundFonts");
        return -1;
    }

    int result = fluid_synth_sfunload(synth, sfont_id, true);
    if (result == FLUID_FAILED) {
        WARN_PRINT_ED(vformat("Failed to unload SoundFont: %d", sfont_id));
        return -1;
    }

    return 0;
}

String MTFluidSynthNode::synth_soundfont_name(int sfont_id) {
    String name = "";

    if (synth == NULL) {
        WARN_PRINT_ED("No synth loaded, the soundfont name could not be read");
        return name;
    }

    fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(synth, sfont_id);

    if (sfont)
    {
        name = fluid_sfont_get_name(sfont);
    }
    else
    {
        WARN_PRINT_ED("The soundfont name could not be read");
    }

    return name;
}

void MTFluidSynthNode::synth_soundfont_reset_presets(int sfont_id) {
    if (synth == NULL) {
        WARN_PRINT_ED("No synth loaded, the soundfont preset list could not be reset");
    }

    fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(synth, sfont_id);

    if (sfont)
    {
        fluid_sfont_iteration_start(sfont);
    }
    else
    {
        WARN_PRINT_ED("The soundfont preset list could not be reset");
    }
}

String MTFluidSynthNode::synth_soundfont_next_preset(int sfont_id) {
    String details = "";

    if (synth == NULL) {
        WARN_PRINT_ED("No synth loaded, the soundfont preset could not be read");
        return details;
    }

    fluid_sfont_t* sfont = fluid_synth_get_sfont_by_id(synth, sfont_id);

    if (sfont)
    {
        fluid_preset_t* preset = fluid_sfont_iteration_next(sfont);
        if (preset)
        {
            int banknum = fluid_preset_get_banknum(preset);
            int instnum = fluid_preset_get_num(preset);
            details = details.num_int64(banknum) + "|" +
                details.num_int64(instnum) + " - " + fluid_preset_get_name(preset);
        }
    }
    else
    {
        WARN_PRINT_ED("The soundfont name could not be read");
    }

    return details;
}

int MTFluidSynthNode::synth_delete() {
    /* Clean up */
    set_process_input(false);
    delete_fluid_audio_driver(adriver);
    adriver = NULL;
    delete_fluid_synth(synth);
    synth = NULL;

    return 0;
}

void MTFluidSynthNode::synth_map_channel(int channel, int mapped_channel) {
    channel_map[channel] = mapped_channel;
}

int MTFluidSynthNode::synth_setup_channel(int channel, int sfont_id, int bank_num, int program, int reverb, int chorus,
    int volume, int pan, int expression) {
    
    if (synth == NULL) {
        WARN_PRINT_ED("Create a FluidSynth instance before channel setup");
        return -1;
    }
    
    // Reset all controllers
    fluid_synth_cc(synth, channel, MIDI_CC_TYPE_CHMODE_RESET_ALL_CTRLS, 0);

    fluid_synth_program_select(synth, channel, sfont_id, bank_num, program);
    fluid_synth_cc(synth, channel, MIDI_CC_TYPE_EFX1DEPTH_EXTERNAL_EFX, reverb);
    fluid_synth_cc(synth, channel, MIDI_CC_TYPE_EFX3DEPTH_CHORUS, chorus);
    fluid_synth_cc(synth, channel, MIDI_CC_TYPE_CHANNEL_VOLUME, volume);
    fluid_synth_cc(synth, channel, MIDI_CC_TYPE_PAN, pan);
    fluid_synth_cc(synth, channel, MIDI_CC_TYPE_EXPRESSION, expression);

    return 0;
}

int MTFluidSynthNode::synth_set_interpolation(int method) {
    fluid_interp interp_method = FLUID_INTERP_DEFAULT;
    switch(method) {
        case 0:
            interp_method = FLUID_INTERP_NONE;
            break;
        case 1:
            interp_method = FLUID_INTERP_LINEAR;
            break;
        case 2:
            interp_method = FLUID_INTERP_4THORDER;
            break;
        case 3:
            interp_method = FLUID_INTERP_7THORDER;
            break;
    }

    if (fluid_synth_set_interp_method(synth, -1, interp_method) == FLUID_FAILED) {
        WARN_PRINT_ED("Failed to set interpolation method");
        return -1;
    }

    return 0;
}

int MTFluidSynthNode::synth_render_file(String midi_file, String output_file, String sf_path,
                                int interpolation, double sample_rate, String bit_depth,
                                String file_type)
{
    if (settings == NULL) {
        WARN_PRINT_ED("FluidSynth settings have not been created");
        return -1;
    }

    fluid_settings_t* tmp_settings = new_fluid_settings();
    
    if (settings_copy(settings, tmp_settings) == 0) {

        // specify the file to store the audio to
        // make sure you compiled fluidsynth with libsndfile to get a real wave file
        // otherwise this file will only contain raw s16 stereo PCM
        fluid_settings_setstr(tmp_settings, "audio.file.name", output_file.ascii());

        // use number of samples processed as timing source, rather than the system timer
        fluid_settings_setstr(tmp_settings, "player.timing-source", "sample");
        
        // since this is a non-realtime scenario, there is no need to pin the sample data
        fluid_settings_setint(tmp_settings, "synth.lock-memory", 0);

        // Set the sample rate for rendering, valid values 8000.0 - 96000.0
        if ((sample_rate >= 8000.0) && (sample_rate <= 96000.0))
        {
            fluid_settings_setnum(tmp_settings, "synth.sample-rate", sample_rate);
        }

        // Set the file format, i.e. storage type for sample data
        // Valid values:
        //    'double' = 64 bit floating point
        //    'float' = 32 bit floating point,
        //    's16' = 16 bit signed PCM,
        //    's24' = 24 bit signed PCM,
        //    's32' = 32 bit signed PCM,
        //    's8' = 8 bit signed PCM and
        //    'u8' = 8 bit unsigned PCM.
        fluid_settings_setstr(tmp_settings, "audio.file.format", bit_depth.ascii());

        fluid_settings_setstr(tmp_settings, "audio.file.type", file_type.ascii());

        fluid_synth_t* tmp_synth = new_fluid_synth(tmp_settings);
        
        fluid_synth_sfload(tmp_synth, sf_path.ascii(), true);

        fluid_interp interp_method = FLUID_INTERP_DEFAULT;
        switch(interpolation) {
            case 0:
                interp_method = FLUID_INTERP_NONE;
                break;
            case 1:
                interp_method = FLUID_INTERP_LINEAR;
                break;
            case 2:
                interp_method = FLUID_INTERP_4THORDER;
                break;
            case 3:
                interp_method = FLUID_INTERP_7THORDER;
                break;
        }

        if (fluid_synth_set_interp_method(tmp_synth, -1, interp_method) == FLUID_FAILED) {
            WARN_PRINT_ED("Failed to set interpolation method");
            delete_fluid_synth(tmp_synth);
            delete_fluid_settings(tmp_settings);
            return -1;
        }
        
        fluid_player_t *player = new_fluid_player(tmp_synth);
        fluid_player_add(player, midi_file.ascii());
        fluid_player_play(player);
        
        fluid_file_renderer_t* renderer = new_fluid_file_renderer(tmp_synth);
        
        while (fluid_player_get_status(player) == FLUID_PLAYER_PLAYING)
        {
            if (int rtn = fluid_file_renderer_process_block(renderer) != FLUID_OK)
            {
                break;
            }
        }
        
        // just for sure: stop the playback explicitly and wait until finished
        fluid_player_stop(player);
        fluid_player_join(player);
        
        delete_fluid_file_renderer(renderer);
        delete_fluid_player(player);
        delete_fluid_synth(tmp_synth);
    }
    else {
        WARN_PRINT_ED("Failed to create settings copy");
        delete_fluid_settings(tmp_settings);
        return -1;
    }

    delete_fluid_settings(tmp_settings);
    
    return 0;
}


int MTFluidSynthNode::synth_play_messages(int msg_count, PackedInt32Array indices, PackedByteArray data)
{
    if (synth)
    {
        int data_length = data.size();

        for (int i = 0; i < msg_count; ++i)
        {
            int cur_index = indices[i];

            if (cur_index < data_length)
            {
                int type = data[cur_index] & 0xF0;
                int channel = data[cur_index] & 0x0F;

                switch (type)
                {
                    case MIDI_MSG_TYPE_NOTE_OFF:
                        if (cur_index + 1 < data_length)
                        {
                            fluid_synth_noteoff(synth, channel, data[cur_index + 1]);
                        }
                        break;
                    case MIDI_MSG_TYPE_NOTE_ON:
                        if (cur_index + 2 < data_length)
                        {
                            // NOTE: Velocity is filtered to remove highest bit,
                            // due to invalid format introduced by Cubase
                            fluid_synth_noteon(synth, channel, data[cur_index + 1],
                                data[cur_index + 2] & 0x7F);
                        }
                        break;
                    case MIDI_MSG_TYPE_POLY_KEY_PRESSURE:
                        if (cur_index + 2 < data_length)
                        {
                            fluid_synth_key_pressure(synth, channel, data[cur_index + 1],
                                data[cur_index + 2]);
                        }
                        break;
                    case MIDI_MSG_TYPE_CHANNEL_PRESSURE:
                        if (cur_index + 1 < data_length)
                        {
                            fluid_synth_channel_pressure(synth, channel, data[cur_index + 1]);
                        }
                        break;
                    case MIDI_MSG_TYPE_CONTROL_CHANGE:
                        if (cur_index + 2 < data_length)
                        {
                            fluid_synth_cc(synth, channel, data[cur_index + 1],
                                data[cur_index + 2]);
                        }
                        break;
                    case MIDI_MSG_TYPE_PITCH_BEND:
                        if (cur_index + 2 < data_length)
                        {
                            int pitch = (data[cur_index + 2] << 7) + data[cur_index + 1];
                            fluid_synth_pitch_bend(synth, channel, pitch);
                        }
                        break;
                    case MIDI_MSG_TYPE_PROGRAM_CHANGE:
                        if (cur_index + 1 < data_length)
                        {
                            fluid_synth_program_change(synth, channel, data[cur_index + 1]);
                        }
                        break;
                    case MIDI_MSG_TYPE_SYSTEM:
                        // TODO: Add code to handle system messages
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else
    {
        WARN_PRINT_ED("No synth available, could not play events");
        return -1;
    }

    return 0;
}


int MTFluidSynthNode::synth_system_reset()
{
    if (!synth)
    {
        WARN_PRINT_ED("FluidSynth No synth available to reset");
        return -1;
    }
    return fluid_synth_system_reset(synth);
}


void MTFluidSynthNode::synth_listen_ext_input(bool listen)
{
    if (!synth)
    {
        WARN_PRINT_ED("FluidSynth No synth available");
    }
    set_process_input(listen);
}


void MTFluidSynthNode::_input(const Ref<InputEvent> &event) {
    InputEventMIDI* midi_event;
    if ((midi_event = dynamic_cast<InputEventMIDI*>(*event)) != nullptr ) {
        int channel = channel_map[midi_event->get_channel()];
        switch(midi_event->get_message()) {
            case MIDI_MESSAGE_NOTE_OFF:
                fluid_synth_noteoff(synth, channel, midi_event->get_pitch());
                break;
            case MIDI_MESSAGE_NOTE_ON:
                fluid_synth_noteon(synth, channel, midi_event->get_pitch(),
                    midi_event->get_velocity());
                break;
            case MIDI_MESSAGE_AFTERTOUCH:
                fluid_synth_key_pressure(synth, channel, midi_event->get_pitch(),
                    midi_event->get_pressure());
                break;
            case MIDI_MESSAGE_CHANNEL_PRESSURE:
                fluid_synth_channel_pressure(synth, channel, midi_event->get_pressure());
                break;
            case MIDI_MESSAGE_CONTROL_CHANGE:
                fluid_synth_cc(synth, channel, midi_event->get_controller_number(),
                    midi_event->get_controller_value());
                break;
            case MIDI_MESSAGE_PITCH_BEND:
                fluid_synth_pitch_bend(synth, channel, midi_event->get_pitch());
                break;
            case MIDI_MESSAGE_PROGRAM_CHANGE:
                fluid_synth_program_change(synth, channel, midi_event->get_instrument());
                break;
            case MIDI_MESSAGE_SYSTEM_RESET:
                fluid_synth_system_reset(synth);
            default:
                break;
        }
    }
}

