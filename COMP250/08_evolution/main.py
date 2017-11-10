import pygame
import random

import map
import pathfinding


TILE_SIZE = 25
MAP_WIDTH = 20
MAP_HEIGHT = 20

POPULATION_SIZE = 20
TOURNAMENT_SIZE = 5
ELITISM = POPULATION_SIZE / 10
MUTATION = 0.5


def wait_key():
    while True:
        event = pygame.event.wait()
        if event.type == pygame.QUIT:
            raise KeyboardInterrupt()
        elif event.type == pygame.KEYDOWN:
            return


def draw_map_and_path(screen, the_map, path):
    screen.fill((255, 255, 255))

    the_map.draw(screen)
    for i in xrange(1, len(path)):
        pygame.draw.line(screen, (0, 0, 0), path[i - 1].pos, path[i].pos, 6)

    pygame.display.flip()


def get_path(the_map):
    path = pathfinding.a_star_search(the_map, None)
    if path is None:
        path = []

    pathfinding.pull_string(the_map, path)

    return path


def calculate_fitness(the_map):
    path = get_path(the_map)
    return pathfinding.get_path_length(path)


def tournament_select(population):
    tournament = []
    for i in xrange(TOURNAMENT_SIZE):
        tournament.append(random.choice(population))

    population.sort()
    return population[-1][1]


def main():
    # Initial population
    population = []
    for i in xrange(POPULATION_SIZE):
        tiles = [' '] * (MAP_WIDTH * MAP_HEIGHT)
        tiles[0] = 'S'
        tiles[-1] = 'G'

        # Randomly set 10% of tiles
        for j in xrange(len(tiles) / 10):
            r = random.randrange(len(tiles))
            if tiles[r] == ' ':
                tiles[r] = '*'

        the_map = map.Map((MAP_WIDTH, MAP_HEIGHT), tiles, TILE_SIZE, include_diagonals=True)
        fitness = calculate_fitness(the_map)

        population.append((fitness, tiles))

    population.sort()

    # Initialise PyGame
    pygame.init()
    clock = pygame.time.Clock()

    window_width = the_map.width * TILE_SIZE
    window_height = the_map.height * TILE_SIZE
    window_size = (window_width, window_height)

    # Create the screen
    screen = pygame.display.set_mode(window_size)

    while True:
        new_population = [x[1] for x in population[-ELITISM:]]

        while len(new_population) < POPULATION_SIZE:
            if random.uniform(0, 1) < MUTATION:
                mutated = tournament_select(population)[:]
                for i in xrange(len(mutated) / 20):
                    r = random.randrange(len(mutated))
                    if mutated[r] == ' ':
                        mutated[r] = '*'
                    elif mutated[r] == '*':
                        mutated[r] = ' '

                new_population.append(mutated)
            else:
                parent_a = tournament_select(population)[:]
                parent_b = tournament_select(population)[:]
                crossover_point = random.randrange(len(parent_a))
                child = parent_a[:crossover_point] + parent_b[crossover_point:]

                new_population.append(child)

        population = []
        for tiles in new_population:
            the_map = map.Map((MAP_WIDTH, MAP_HEIGHT), tiles, TILE_SIZE, include_diagonals=True)
            fitness = calculate_fitness(the_map)

            population.append((fitness, tiles))

        population.sort()
        fitness, tiles = population[-1]
        the_map = map.Map((MAP_WIDTH, MAP_HEIGHT), tiles, TILE_SIZE, include_diagonals=True)
        print "Current fitness:", fitness
        path = get_path(the_map)
        draw_map_and_path(screen, the_map, path)

        # wait_key()

if __name__ == '__main__':
    main()
