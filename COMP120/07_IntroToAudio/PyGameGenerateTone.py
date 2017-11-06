import pygame

pygame.init()
pygame.display.set_mode((800,600))
pygame.mixer.init()


done=False

while not done:
    for event in pygame.event.get():
        if event.type==pygame.QUIT:
            done=True


    pygame.display.update()

pygame.mixer.quit()
pygame.quit()