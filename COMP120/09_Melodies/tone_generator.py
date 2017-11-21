import wave
import math
import struct

SAMPLE_WIDTH=2
SAMPLE_RATE=44100.0
BIT_DEPTH=2.0
CHANNELS=2


def combine_tones(tone_one,tone_two,sample_length):
    values = []
    for i in range(0, sample_length):
        values.append(tone_one[i]+tone_two[i])
    return values


def generate_sine_wave(frequency, sample_rate, sample_length, volume):
    values = []
    for i in range(0, sample_length):
        value = math.sin(2 * math.pi * frequency * (i / sample_rate)) * (volume * BIT_DEPTH)
        for j in xrange(0, CHANNELS):
            values.append(value)

    return values


def generate_square_wave(frequency, sample_rate, sample_length, volume):
    values = []
    for i in range(0, sample_length):
        value = 4*(math.sin(frequency * (i / sample_rate)) * (volume * BIT_DEPTH))/math.pi

        for j in xrange(0, CHANNELS):
            values.append(value)

    return values


def generate_saw_tooth_wave(frequency, sample_rate, sample_length, volume):
    values = []
    for i in range(0, sample_length):
        value = 2*(math.sin(frequency * (i / sample_rate)) * (volume * BIT_DEPTH))/-math.pi

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


def make_chord(sound1,sound2,sound3,sample_length):
    values=[]
    for i in range(0,sample_length):
        values.append(sound1[i])
        if i>4000:
            values.append(sound1[i]+sound2[i])
        if i>8000:
            values.append(sound1[i]+sound2[i]+sound3[i])

    return values


def echo(sound1,sound2,delay,sample_length):
    values=[]
    for i in range(0,sample_length):
        values.append(sound1[i])
        if i>delay:
            echo=sound1[i]*0.6
            values.append(echo+sound1[i])
    return values

tone_values_one=generate_sine_wave(4000.0,SAMPLE_RATE,132000,1000.0)
tone_values_two=generate_sine_wave(400.0,SAMPLE_RATE,132000,500.0)
square_tone_values=generate_square_wave(4000.0,SAMPLE_RATE,132000,2000.0)
saw_tone_values=generate_saw_tooth_wave(4000.0,SAMPLE_RATE,132000,2000.0)
chord_values=make_chord(tone_values_one,tone_values_two,square_tone_values,132000)
echo_values=echo(tone_values_one,tone_values_two,40000,132000)


save_wave_file('Tone1.wav',tone_values_one,SAMPLE_RATE)
save_wave_file('Tone2.wav',tone_values_two,SAMPLE_RATE)
save_wave_file('Square1.wav',square_tone_values,SAMPLE_RATE)
save_wave_file('Saw1.wav',saw_tone_values,SAMPLE_RATE)
save_wave_file('Chord.wav',chord_values,SAMPLE_RATE)
save_wave_file('Echo.wav',echo_values,SAMPLE_RATE)

combined_tone=combine_tones(tone_values_one,tone_values_two,132000)
save_wave_file('CombinedTone.wav',combined_tone,SAMPLE_RATE)

