import wave
import math
import struct

SAMPLE_WIDTH=2
SAMPLE_RATE=44100.0
BIT_DEPTH=2.0
CHANNELS=2


def combine_tones(tone_one,tone_two,sample_length):
    values=[]
    for i in range(0, sample_length):
        values.append(tone_one[i]+tone_two[i])
    return values


def generate_sine_wave(frequency, sample_rate, sample_length, volume):
    values=[]
    for i in range(0, sample_length):
        value = math.sin(2 * math.pi * frequency * (i / sample_rate)) * (volume * BIT_DEPTH)
        packed_value = struct.pack('h', value)

        for j in xrange(0, CHANNELS):
            values.append(packed_value)

    return values


def save_wave_file(filename, wav_data, sample_rate):
    noise_out = wave.open(filename, 'w')
    noise_out.setparams((CHANNELS, SAMPLE_WIDTH, sample_rate, 0, 'NONE', 'not compressed'))
    value_str = ''.join((str(n) for n in wav_data))
    noise_out.writeframes(value_str)
    noise_out.close()


tone_values_one=generate_sine_wave(4000.0,SAMPLE_RATE,132000,1000.0)
tone_values_two=generate_sine_wave(400.0,SAMPLE_RATE,132000,500.0)
save_wave_file('Tone1.wav',tone_values_one,SAMPLE_RATE)
save_wave_file('Tone2.wav',tone_values_two,SAMPLE_RATE)

combined_tone=combine_tones(tone_values_one,tone_values_two,132000)
save_wave_file('CombinedTone.wav',combined_tone,SAMPLE_RATE)

