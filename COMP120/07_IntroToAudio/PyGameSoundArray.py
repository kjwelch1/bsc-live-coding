import pygame
import wave

def increase_volume(current_sound_samples):
    for sample in current_sound_samples:
        sample*= 2

def save_sound(filename,sound,sample_freq,num_channels):
    file=wave.open(filename,'w')
    file.setframerate(sample_freq)
    file.setnchannels(num_channels)
    file.setsampwidth(2)
    file.writeframesraw(sound.get_raw())
    file.close()

pygame.init()
pygame.display.set_mode((800,600))
pygame.mixer.init()

sound=pygame.mixer.Sound('jump.wav')
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
                increase_volume(samples)
            if event.key==pygame.K_s:
                save_sound("test.wav",sound,22050,2)

    pygame.display.update()

pygame.mixer.quit()
pygame.quit()