import pygame
import random
import numpy

def generate_pure_tone():
    """Generate pure tone NB. This is very short sample!"""
    generated_samples = numpy.array([[1, 1], [2, 2], [3, 3]], dtype="int8")
    return generated_samples


pygame.init()
pygame.display.set_mode((800,600))
pygame.mixer.init()

#Generate initial samples from function
samples=generate_random_tone()
#Make a PyGame Sound from the sample array
current_sound=pygame.sndarray.make_sound(samples)

done=False

while not done:
    for event in pygame.event.get():
        if event.type==pygame.QUIT:
            done=True

        if event.type==pygame.KEYDOWN:
            if event.key==pygame.K_p:
                #Play the sound repeating
                current_sound.play(-1)

    pygame.display.update()

pygame.mixer.quit()
pygame.quit()