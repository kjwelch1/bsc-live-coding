import pygame

pygame.init()
pygame.display.set_mode((800,600))
pygame.mixer.init()

pygame.mixer.music.load('bensound-sunny.mp3')
sound=pygame.mixer.Sound('jump.wav')

done=False

while not done:
    for event in pygame.event.get():
        if event.type==pygame.QUIT:
            done=True
        if event.type==pygame.KEYDOWN:
            if event.key==pygame.K_p:
                pygame.mixer.music.play()
            if event.key==pygame.K_SPACE:
                sound.play()

    pygame.display.update()

pygame.mixer.quit()
pygame.quit()