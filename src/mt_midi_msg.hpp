#ifndef MT_MIDI_MSG_H
#define MT_MIDI_MSG_H

#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include "mt_data_buffer.hpp"

namespace godot {

class MTMidiMsg : public Node {
	GDCLASS(MTMidiMsg, Node)

private:
    static uint64_t next_msg_id;
	uint64_t id;

protected:
	static void _bind_methods();

public:
	class ChannelMsgType
	{
		public:
		static const uint8_t NoteOff = 0x80,
		NoteOn = 0x90,
		PolyKeyPressure = 0xA0,
		ControlChange = 0xB0,
		ProgramChange = 0xC0,
		ChannelPressure = 0xD0,
		PitchBend = 0xE0,
		NonChannel = 0xF0;
	};

	class NonChMsgType
	{
		public:
		static const uint8_t SysexStart = 0xF0,
		SysexContOrEsc = 0xF7,
		Meta = 0xFF;
	};

	class CCController
	{
		public:
		static const uint8_t BankSelect = 0x00,
		ModulationWheel = 0x01,
		BreathController = 0x02,
		FootController = 0x04,
		PortamentoTime = 0x05,
		DataEntryMSB = 0x06,
		ChannelVolume = 0x07,
		Balance = 0x08,
		Pan = 0x0A,
		Expression = 0x0B,
		EffectCtrl1 = 0x0C,
		EffectCtrl2 = 0x0D,
		GPC1 = 0x10,
		GPC2 = 0x11,
		GPC3 = 0x12,
		GPC4 = 0x13,
		BankSelectLSB = 0x20,
		ModulationWheelLSB = 0x21,
		BreathControllerLSB = 0x22,
		FootControllerLSB = 0x24,
		PortamentoTimeLSB = 0x25,
		DataEntryLSB = 0x26,
		ChannelVolumeLSB = 0x27,
		BalanceLSB = 0x28,
		PanLSB = 0x2A,
		ExpressionLSB = 0x2B,
		EffectCtrl1LSB = 0x2C,
		EffectCtrl2LSB = 0x2D,
		GPC1LSB = 0x30,
		GPC2LSB = 0x31,
		GPC3LSB = 0x32,
		GPC4LSB = 0x33,
		DamperPedalSustain = 0x40,
		PortamentoOnOff = 0x41,
		Sostenuto = 0x42,
		SoftPedal = 0x43,
		LegatoFootswitch = 0x44,
		Hold2 = 0x45,
		SndCtrl1_SoundVariation = 0x46,
		SndCtrl2_TimbreHarmIntensity = 0x47,
		SndCtrl3_ReleaseTime = 0x48,
		SndCtrl4_AttackTime = 0x49,
		SndCtrl5_Brightness = 0x4A,
		SndCtrl6 = 0x4B,
		SndCtrl7 = 0x4C,
		SndCtrl8 = 0x4D,
		SndCtrl9 = 0x4E,
		SndCtrl10 = 0x4F,
		GPC5 = 0x50,
		GPC6 = 0x51,
		GPC7 = 0x52,
		GPC8 = 0x53,
		PortamentoControl = 0x54,
		Efx1Depth_ExternalEfx = 0x5B,
		Efx2Depth_Tremolo = 0x5C,
		Efx3Depth_Chorus = 0x5D,
		Efx4Depth_CelesteDetune = 0x5E,
		Efx5Depth_Phaser = 0x5F,
		DataIncrement = 0x60,
		DataDecrement = 0x61,
		NonRegParamNumberLSB = 0x62,
		NonRegParamNumberMSB = 0x63,
		RegParamNumberLSB = 0x64,
		RegParamNumberMSB = 0x65,
		ChMode_AllSoundOff = 0x78,
		ChMode_ResetAllCtrls = 0x79,
		ChMode_LocalControl = 0x7A,
		ChMode_AllNotesOff = 0x7B,
		ChMode_OmniModeOff = 0x7C,
		ChMode_OmniModeOn = 0x7D,
		ChMode_MonoModeOn = 0x7E,
		ChMode_PolyModeOn = 0x7F;
	};

	class MetaMsgType
	{
		public:
		static const uint8_t SequenceNumber = 0x00,
		TextEvent = 0x01,
		CopyrightNotice = 0x02,
		SeqOrTrkName = 0x03,
		InstrumentName = 0x04,
		Lyric = 0x05,
		Marker = 0x06,
		CuePoint = 0x07,
		ChannelPrefix = 0x20,
		PortPrefix = 0x21,
		EndOfTrack = 0x2F,
		SetTempo = 0x51,
		SMPTEOffset = 0x54,
		TimeSignature = 0x58,
		KeySignature = 0x59,
		SequencerMetaEvent = 0x7F;
	};

	uint64_t tick;
	PackedByteArray msg_bytes;
	int32_t data_length;
	int32_t data_start;
	uint8_t channel_prefix;
	uint8_t port_prefix;

	MTMidiMsg();
    MTMidiMsg(uint64_t tick, uint8_t statusByte, int32_t dataLength);
    MTMidiMsg(uint64_t tick, PackedByteArray msg_as_bytes);
    //~MTMidiMsg();
    uint64_t get_id() { return id; }
    uint64_t get_tick() { return tick; }
	int32_t get_data_length() { return data_length; }
	int32_t get_data_start() { return data_start; }
	uint8_t get_channel_prefix() { return channel_prefix; }
	uint8_t get_port_prefix() { return port_prefix; }
    static bool is_status_byte(uint8_t byte) { return (byte & 0x80) != 0; };
    int32_t get_status_byte();
    int32_t get_note_value();
    int32_t get_note_velocity();
    int32_t get_channel_msg_type();
    int32_t get_channel();
    PackedByteArray get_msg_as_bytes();
    uint8_t read_data_value(int32_t index);
    PackedByteArray copy_binary_data();
    uint8_t get_meta_msg_type();
    String get_meta_msg_text();
    static MTMidiMsg *read_msg(uint64_t tick, uint8_t& running_status, uint8_t& channel_prefix, uint8_t& port_prefix, MTDataBuffer &buffer, int32_t& bytes_read);
    static MTMidiMsg *read_channel_msg(uint64_t tick, uint8_t s_byte, MTDataBuffer &buffer, int32_t& bytes_read);
    static MTMidiMsg *read_meta_msg(uint64_t tick, uint8_t s_byte, MTDataBuffer &buffer, int32_t& bytes_read);
    static MTMidiMsg *read_sysex_msg(uint64_t tick, uint8_t s_byte, MTDataBuffer &buffer, int32_t& bytes_read);
    PackedByteArray to_array(uint64_t &current_tick);
    int32_t length_in_bytes(uint64_t &current_tick);
    int32_t read_tempo();
};
}
#endif
