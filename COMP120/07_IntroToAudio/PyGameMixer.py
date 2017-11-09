import pygame

pygame.init()
pygame.display.set_mode((800,600))
pygame.mixer.init()

#Load in the music file
pygame.mixer.music.load('bensound-sunny.mp3')
#Load the Sound from file, notice that this returns a sound
#which we store for later use
jump_sound=pygame.mixer.Sound('jump.wav')

done=False

while not done:
    for event in pygame.event.get():
        if event.type==pygame.QUIT:
            done=True
        if event.type==pygame.KEYDOWN:
            if event.key==pygame.K_p:
                #this plays the music which has been loaded by
                #the mixer, you should note that the play function
                #takes in multiple parameters which control playback
                pygame.mixer.music.play()
            if event.key==pygame.K_SPACE:
                #playing the jump sound, again the play function
                #can take in multiple pararmeters
                jump_sound.play()

    pygame.display.update()

#quit the pygame mixer
pygame.mixer.quit()
pygame.quit()