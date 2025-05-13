#ifndef MT_FLUID_SYNTH_NODE_H
#define MT_FLUID_SYNTH_NODE_H

#include <godot_cpp/classes/input_event.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/classes/node.hpp>
#include <fluidsynth.h>

namespace godot {

class MTFluidSynthNode : public Node {
	GDCLASS(MTFluidSynthNode, Node)

private:
    int channel_map[16];
    fluid_audio_driver_t *adriver;
    fluid_player_t *player;
    fluid_settings_t *settings;
    fluid_synth_t *synth;

protected:
	static void _bind_methods();

public:
	MTFluidSynthNode();
	~MTFluidSynthNode();


    // Constants
    static const uint8_t MIDI_MSG_TYPE_NOTE_OFF = 0x80;
    static const uint8_t MIDI_MSG_TYPE_NOTE_ON = 0x90;
    static const uint8_t MIDI_MSG_TYPE_POLY_KEY_PRESSURE = 0xA0;
    static const uint8_t MIDI_MSG_TYPE_CONTROL_CHANGE = 0xB0;
    static const uint8_t MIDI_MSG_TYPE_PROGRAM_CHANGE = 0xC0;
    static const uint8_t MIDI_MSG_TYPE_CHANNEL_PRESSURE = 0xD0;
    static const uint8_t MIDI_MSG_TYPE_PITCH_BEND = 0xE0;
    static const uint8_t MIDI_MSG_TYPE_SYSTEM = 0xF0;

    static const uint8_t MIDI_SYS_MSG_TYPE_SYSTEM_EXCLUSIVE = 0xF0;
    static const uint8_t MIDI_SYS_MSG_TYPE_TIME_CODE_QTR_FRAME = 0xF1;
    static const uint8_t MIDI_SYS_MSG_TYPE_SONG_POSITION_PTR = 0xF2;
    static const uint8_t MIDI_SYS_MSG_TYPE_SONG_SELECT = 0xF3;
    static const uint8_t MIDI_SYS_MSG_TYPE_TUNE_REQUEST = 0xF6;
    static const uint8_t MIDI_SYS_MSG_TYPE_END_OF_EXCLUSIVE = 0xF7;
    static const uint8_t MIDI_SYS_MSG_TYPE_TIMING_CLOCK = 0xF8;
    static const uint8_t MIDI_SYS_MSG_TYPE_START = 0xFA;
    static const uint8_t MIDI_SYS_MSG_TYPE_CONTINUE = 0xFB;
    static const uint8_t MIDI_SYS_MSG_TYPE_STOP = 0xFC;
    static const uint8_t MIDI_SYS_MSG_TYPE_ACTIVE_SENSING = 0xFE;
    static const uint8_t MIDI_SYS_MSG_TYPE_SYSTEM_RESET = 0xFF;

    static const uint8_t MIDI_CC_TYPE_BANKSELECT = 0x00;
    static const uint8_t MIDI_CC_TYPE_MODULATION_WHEEL = 0x01;
    static const uint8_t MIDI_CC_TYPE_BREATH_CONTROLLER = 0x02;
    static const uint8_t MIDI_CC_TYPE_FOOT_CONTROLLER = 0x04;
    static const uint8_t MIDI_CC_TYPE_PORTAMENTO_TIME = 0x05;
    static const uint8_t MIDI_CC_TYPE_DATA_ENTRY_MSB = 0x06;
    static const uint8_t MIDI_CC_TYPE_CHANNEL_VOLUME = 0x07;
    static const uint8_t MIDI_CC_TYPE_BALANCE = 0x08;
    static const uint8_t MIDI_CC_TYPE_PAN = 0x0A;
    static const uint8_t MIDI_CC_TYPE_EXPRESSION = 0x0B;
    static const uint8_t MIDI_CC_TYPE_EFFECT_CTRL1 = 0x0C;
    static const uint8_t MIDI_CC_TYPE_EFFECT_CTRL2 = 0x0D;
    static const uint8_t MIDI_CC_TYPE_GPC1 = 0x10;
    static const uint8_t MIDI_CC_TYPE_GPC2 = 0x11;
    static const uint8_t MIDI_CC_TYPE_GPC3 = 0x12;
    static const uint8_t MIDI_CC_TYPE_GPC4 = 0x13;
    static const uint8_t MIDI_CC_TYPE_BANK_SELECT_LSB = 0x20;
    static const uint8_t MIDI_CC_TYPE_MODULATION_WHEEL_LSB = 0x21;
    static const uint8_t MIDI_CC_TYPE_BREATH_CONTROLLER_LSB = 0x22;
    static const uint8_t MIDI_CC_TYPE_FOOT_CONTROLLER_LSB = 0x24;
    static const uint8_t MIDI_CC_TYPE_PORTAMENTO_TIME_LSB = 0x25;
    static const uint8_t MIDI_CC_TYPE_DATA_ENTRY_LSB = 0x26;
    static const uint8_t MIDI_CC_TYPE_CHANNEL_VOLUME_LSB = 0x27;
    static const uint8_t MIDI_CC_TYPE_BALANCE_LSB = 0x28;
    static const uint8_t MIDI_CC_TYPE_PAN_LSB = 0x2A;
    static const uint8_t MIDI_CC_TYPE_EXPRESSION_LSB = 0x2B;
    static const uint8_t MIDI_CC_TYPE_EFFECT_CTRL1_LSB = 0x2C;
    static const uint8_t MIDI_CC_TYPE_EFFECT_CTRL2_LSB = 0x2D;
    static const uint8_t MIDI_CC_TYPE_GPC1LSB = 0x30;
    static const uint8_t MIDI_CC_TYPE_GPC2LSB = 0x31;
    static const uint8_t MIDI_CC_TYPE_GPC3LSB = 0x32;
    static const uint8_t MIDI_CC_TYPE_GPC4LSB = 0x33;
    static const uint8_t MIDI_CC_TYPE_DAMPER_PEDAL_SUSTAIN = 0x40;
    static const uint8_t MIDI_CC_TYPE_PORTAMENTO_ON_OFF = 0x41;
    static const uint8_t MIDI_CC_TYPE_SOSTENUTO = 0x42;
    static const uint8_t MIDI_CC_TYPE_SOFT_PEDAL = 0x43;
    static const uint8_t MIDI_CC_TYPE_LEGATO_FOOT_SWITCH = 0x44;
    static const uint8_t MIDI_CC_TYPE_HOLD2 = 0x45;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL1_SOUND_VARIATION = 0x46;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL2_TIMBRE_HARM_INTENSITY = 0x47;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL3_RELEASE_TIME = 0x48;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL4_ATTACK_TIME = 0x49;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL5_BRIGHTNESS = 0x4A;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL6 = 0x4B;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL7 = 0x4C;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL8 = 0x4D;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL9 = 0x4E;
    static const uint8_t MIDI_CC_TYPE_SND_CTRL10 = 0x4F;
    static const uint8_t MIDI_CC_TYPE_GPC5 = 0x50;
    static const uint8_t MIDI_CC_TYPE_GPC6 = 0x51;
    static const uint8_t MIDI_CC_TYPE_GPC7 = 0x52;
    static const uint8_t MIDI_CC_TYPE_GPC8 = 0x53;
    static const uint8_t MIDI_CC_TYPE_PORTAMENTO_CONTROL = 0x54;
    static const uint8_t MIDI_CC_TYPE_EFX1DEPTH_EXTERNAL_EFX = 0x5B;
    static const uint8_t MIDI_CC_TYPE_EFX2DEPTH_TREMOLO = 0x5C;
    static const uint8_t MIDI_CC_TYPE_EFX3DEPTH_CHORUS = 0x5D;
    static const uint8_t MIDI_CC_TYPE_EFX4DEPTH_CELESTE_DETUNE = 0x5E;
    static const uint8_t MIDI_CC_TYPE_EFX5DEPTH_PHASER = 0x5F;
    static const uint8_t MIDI_CC_TYPE_DATA_INCREMENT = 0x60;
    static const uint8_t MIDI_CC_TYPE_DATA_DECREMENT = 0x61;
    static const uint8_t MIDI_CC_TYPE_NON_REG_PARAM_NUMBER_LSB = 0x62;
    static const uint8_t MIDI_CC_TYPE_NON_REG_PARAM_NUMBER_MSB = 0x63;
    static const uint8_t MIDI_CC_TYPE_REG_PARAM_NUMBER_LSB = 0x64;
    static const uint8_t MIDI_CC_TYPE_REG_PARAM_NUMBER_MSB = 0x65;
    static const uint8_t MIDI_CC_TYPE_CHMODE_ALL_SOUND_OFF = 0x78;
    static const uint8_t MIDI_CC_TYPE_CHMODE_RESET_ALL_CTRLS = 0x79;
    static const uint8_t MIDI_CC_TYPE_CHMODE_LOCAL_CONTROL = 0x7A;
    static const uint8_t MIDI_CC_TYPE_CHMODE_ALL_NOTES_OFF = 0x7B;
    static const uint8_t MIDI_CC_TYPE_CHMODE_OMNI_MODE_OFF = 0x7C;
    static const uint8_t MIDI_CC_TYPE_CHMODE_OMNI_MODE_ON = 0x7D;
    static const uint8_t MIDI_CC_TYPE_CHMODE_MONO_MODE_ON = 0x7E;
    static const uint8_t MIDI_CC_TYPE_CHMODE_POLY_MODE_ON = 0x7F;

    // Settings
    /**
     * @brief Create a settings object.
     * 
     * @return int Returns 0 on success.
     */
    int settings_create();

    /**
     * @brief Change an integer setting.
     * 
     * @param setting Setting name.
     * @param value New value.
     * @return int Returns 0 on success.
     */
    int settings_change_int(String setting, int value);
    
    /**
     * @brief Change a float/double setting.
     * 
     * @param setting Setting name.
     * @param value New value.
     * @return int Returns 0 on success.
     */
    int settings_change_dbl(String setting, double value);
    
    /**
     * @brief Change a String setting.
     * 
     * @param setting Setting name.
     * @param value New value.
     * @return int Returns 0 on success.
     */
    int settings_change_str(String setting, String value);

    double settings_get_dbl(String setting);
    int settings_get_int(String setting);
    String settings_get_str(String setting);

    /**
     * @brief Deletes the internal settings structure.
     * 
     * @return int Returns 0 on success.
     */
    int settings_delete();

    int settings_copy(fluid_settings_t *original, fluid_settings_t *copy);
    String settings_as_json_str();
    int settings_save(String file_name);
    int settings_load_from_json(String json_str);
    int settings_load(String file_name);


    // Synth
    /**
     * @brief Creates a new Fluid Synth instance and loads a SoundFont.
     * 
     * @param sf_path Path to the SoundFont to be loaded.
     * @param listen_ext_input Should the synth respond to external MIDI input.
     * @return int Returns Soundfont id on success, -1 on failure.
     */
    int synth_create(String sf_path, bool listen_ext_input);

    /**
     * @brief Renders the specified MIDI file as a .wav file, using
     *        the current settings.
     * 
     * @param midi_file Path to MIDI file to be rendered.
     * @param output_file Path to output file, can use file extension to determine type.
     * @param sf_path Path to SoundFont to be used for rendering.
     * @param interpolation Type of interpolation to use, 4th Order is default.
     *                      (0 = None, 1 = Linear, 2 = 4th Order, 3 = 7th Order)
     * @return int Returns 0 on success.
     */
    int synth_render_file(String midi_file, String output_file, String sf_path,
        int interpolation, double sample_rate, String bit_depth, String file_type);
    int synth_soundfont_load(String sf_path, bool reset);
    int synth_soundfont_unload(int sfont_id);
    String synth_soundfont_name(int sfont_id);
    void synth_soundfont_reset_presets(int sfont_id);
    String synth_soundfont_next_preset(int sfont_id);
    int synth_delete();
    void synth_map_channel(int channel, int mapped_channel);
    int synth_setup_channel(int channel, int sfont_id, int bank_num, int program, int reverb, int chorus,
        int volume = 100, int pan = 64, int expression = 127);
    int synth_set_interpolation(int method);
    int synth_play_messages(int msg_count, PackedInt32Array indices, PackedByteArray data);
    int synth_system_reset();
    void synth_listen_ext_input(bool listen);
    void _input(const Ref<InputEvent> &event) override;


    // Player
    int player_create();
    int player_delete();
    int player_load_midi(String file_path);
    int player_play(int loop_count = 0);
    int player_seek(int tick);
    int player_stop();
};

}

#endif