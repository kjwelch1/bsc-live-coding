import wave
import math
import struct

SAMPLE_WIDTH=2
SAMPLE_RATE=44100.0
BIT_DEPTH=2.0
CHANNELS=2

def generate_sine_wave(frequency, sample_rate, sample_length, volume):
    values = []
    for i in range(0, sample_length):
        value = math.sin(2 * math.pi * frequency * (i / sample_rate)) * (volume * BIT_DEPTH)

        for j in xrange(0, CHANNELS):
            values.append(value)

    return values


def save_wave_file(filename, wav_data, sample_rate):
    packed_values=[]
    for i in range(0,len(wav_data)):
        packed_value = struct.pack('h', wav_data[i])
        packed_values.append(packed_value)

    noise_out = wave.open(filename, 'w')
    noise_out.setparams((CHANNELS, SAMPLE_WIDTH, sample_rate, 0, 'NONE', 'not compressed'))
    value_str = ''.join((str(n) for n in packed_values))
    noise_out.writeframes(value_str)
    noise_out.close()


tone_values_one=generate_sine_wave(4000.0,SAMPLE_RATE,132000,1000.0)
save_wave_file('Tone1.wav',tone_values_one,SAMPLE_RATE)