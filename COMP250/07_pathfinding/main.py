import pygame
import time
import glob

import map


def reconstruct_path(node):
    path = []
    while node is not None:
        path.insert(0, node)
        node = node.came_from
    return path


def search(the_map, screen):
    # Initialise the open and visited nodes
    open_nodes = [the_map.start]
    visited_nodes = set()

    while len(open_nodes) > 0:
        # Pop an open node and mark it as visited
        current_node = open_nodes.pop()
        visited_nodes.add(current_node)

        # If we have reached the goal, return the path
        if current_node is the_map.goal:
            return reconstruct_path(current_node)

        # Loop through the neighbours, adding unvisited neighbours to the open list
        for edge in current_node.edges:
            if edge.to_node not in open_nodes and edge.to_node not in visited_nodes:
                edge.to_node.came_from = current_node
                open_nodes.append(edge.to_node)

        # Redraw the map
        screen.fill((255, 255, 255))
        the_map.draw(screen)

        # Draw the open nodes in red
        for node in open_nodes:
            node.draw(screen, (255, 0, 0), 6)

        # Draw the current node in green
        current_node.draw(screen, (0, 255, 0), 12)

        # Flip the display and pause for 0.1 seconds
        pygame.display.flip()
        time.sleep(0.1)
        pygame.event.get()


def a_star_search(the_map, screen):
    open_nodes = [the_map.start]
    the_map.start.priority = map.euclidean_distance(the_map.start.pos, the_map.goal.pos)
    the_map.start.cost_so_far = 0
    visited_nodes = set()

    while len(open_nodes) > 0:
        open_nodes.sort(key=lambda n: n.priority)
        current_node = open_nodes.pop(0)
        visited_nodes.add(current_node)

        if current_node is the_map.goal:
            return reconstruct_path(current_node)

        for edge in current_node.edges:
            if edge.to_node not in visited_nodes:
                new_f = current_node.cost_so_far + edge.length + map.euclidean_distance(edge.to_node.pos, the_map.goal.pos)
                if edge.to_node not in open_nodes or new_f < edge.to_node.priority:
                    edge.to_node.came_from = current_node
                    edge.to_node.cost_so_far = current_node.cost_so_far + edge.length
                    edge.to_node.priority = new_f
                    if edge.to_node not in open_nodes:
                        open_nodes.append(edge.to_node)

        screen.fill((255, 255, 255))
        the_map.draw(screen)

        for node in open_nodes:
            node.draw(screen, (255, 0, 0), 6)

        current_node.draw(screen, (0, 255, 0), 12)

        pygame.display.flip()
        time.sleep(0.1)
        pygame.event.get()


def main():
    tile_size = 50

    # Initialise PyGame
    pygame.init()
    clock = pygame.time.Clock()

    for map_name in sorted(glob.glob("*.txt")):
        the_map = map.Map(map_name, tile_size, include_diagonals=True)

        window_width = the_map.width * tile_size
        window_height = the_map.height * tile_size
        window_size = (window_width, window_height)

        # Create the screen
        screen = pygame.display.set_mode(window_size)

        path = search(the_map, screen)

        screen.fill((255, 255, 255))

        the_map.draw(screen)
        if path is not None:
            for i in xrange(1, len(path)):
                pygame.draw.line(screen, (0, 0, 0), path[i - 1].pos, path[i].pos, 6)

        pygame.display.flip()

        while True:
            event = pygame.event.wait()
            if event.type == pygame.QUIT or event.type == pygame.KEYDOWN:
                break


if __name__ == '__main__':
    main()
