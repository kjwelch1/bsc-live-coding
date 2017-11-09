import pygame
import wave

def increase_volume(current_sound_samples,change_amount):
    """Increave the volume (aka amplitude) of the current sound sample by the value passed in"""
    for sample in current_sound_samples:
        sample*=change_amount


def save_sound(filename,sound,sample_freq,num_channels):
    """Save the sound to a given wav filename, with a frequency and a number of channels (1=mono, 2=stero)"""
    #open a wave file in write ('w') mode, this will create the file
    file=wave.open(filename,'w')
    #set the framerate aka sample frequency
    file.setframerate(sample_freq)
    #set the number of the channels
    file.setnchannels(num_channels)
    #the size of the one sample in bytes
    file.setsampwidth(2)
    #write the actual sound to the file, notice the call to get_raw
    file.writeframesraw(sound.get_raw())
    file.close()

pygame.init()
pygame.display.set_mode((800,600))
pygame.mixer.init()

#load the sound from file
sound=pygame.mixer.Sound('jump.wav')
#retrieve the samples, if we change the samples we will change the
#sound
samples=pygame.sndarray.samples(sound)

done=False

while not done:
    for event in pygame.event.get():
        if event.type==pygame.QUIT:
            done=True
        if event.type==pygame.KEYDOWN:
            if event.key==pygame.K_SPACE:
                sound.play()
            if event.key==pygame.K_UP:
                #double the sound
                increase_volume(samples,2)
            if event.key==pygame.K_s:
                #save the sound with the specfied values, these values
                #are the same as the standard initialisation of the mixer
                #these should be stored as variables
                save_sound("test.wav",sound,22050,2)

    pygame.display.update()

pygame.mixer.quit()
pygame.quit()