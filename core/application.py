"""
This is a main class for an application to simulate an amr in the enviroment.
An application has three stages:
- Initializing.
- Goto the main loop.
- Exit.
The main loop of an application has four stages:
- Updating Input from outsides.
- Sensoring the environment.
- Amr Processing.
- Updating orientation and position of the amr.
"""
import random
import pygame
import sys
from core.input import Input
from core.map import Maps
from core.graphic import Graphics
from core.amr import Amrs
from component.sensor import Sensors
from component.processor import Processors
from utils.utils import *

class Application(object):
    # strategy: "explore" prefers unvisited nodes and plans a BFS path to the nearest one,
    #           "random" is a random walk that never turns straight back
    def __init__(self, screenSize=[1400, 800], mapSize=[30, 30], startNode=(2, 4), fps=3,
                 seed=None, strategy="explore"):

        if seed is not None:
            random.seed(seed)

        self.graphic = Graphics(screenSize)

        # determine is main loop is active
        self.running = True
        # manage time-related data and operations
        self.clock = pygame.time.Clock()
        self.lastTime = pygame.time.get_ticks()
        self.fps = fps
        self.strategy = strategy
        # manage user input
        self.input = Input()
        self.map = Maps(mapSize)
        self.map.randomMap()
        # the start node must be free, otherwise the amr begins inside an obstacle
        self.map.map[startNode[0]][startNode[1]] = 0

        startPos = turn2pixel(self.map.map, self.graphic.screen.get_height(),
                              self.graphic.screen.get_width(), startNode[0], startNode[1])
        self.amr = Amrs(position=list(startPos))
        self.sensor = Sensors(position=list(startPos))
        self.processor = Processors()

        self.steps = 0
        self.visited = {tuple(startNode)}
        self.reachable = countReachable(self.map.map, tuple(startNode))

    def unitDistance(self):
        row_segment = len(self.map.map) - 1
        col_segment = len(self.map.map[0]) - 1
        row_distance = self.graphic.screen.get_height()/row_segment
        col_distance = self.graphic.screen.get_width()/col_segment
        return (row_distance, col_distance)

    # implement by extending class
    def initialize(self):
        pass

    # implement by extending class
    def update(self):
        pass

    # move the amr one node, return False when it has nowhere to go
    def step(self):
        width, height = self.graphic.screen.get_width(), self.graphic.screen.get_height()

        ## process sensor ##
        self.sensor.setPos(self.amr.pos)
        sensory = self.sensor.getOutput(self.map.map, width, height)
        if not sensory:
            return False

        ## amr processing ##
        self.processor.staticObstacleAvoidanceSolution(sensory)
        unvisited = ()
        if self.strategy == "explore":
            node = turn2node(self.map.map, width, height, self.amr.pos[0], self.amr.pos[1])
            unvisited = [h for h in sensory if neighbourNode(node, h) not in self.visited]
            if not unvisited and not self.processor.path:
                self.processor.pathPlanning(pathToNearestUnvisited(self.map.map, node, self.visited))
        solution = self.processor.makeDecision(unvisited)
        if solution is None:
            return False
        if solution in [90, 270]:
            self.amr.speed = self.unitDistance()[0]
        elif solution in [0, 180]:
            self.amr.speed = self.unitDistance()[1]
        self.amr.heading = solution
        self.amr.moveForward(self.amr.speed)

        self.steps += 1
        self.visited.add(turn2node(self.map.map, width, height, self.amr.pos[0], self.amr.pos[1]))
        return True

    def draw(self):
        self.graphic.screen.fill((255, 255, 255))
        self.graphic.drawVisited(self.map.map, self.visited)
        self.graphic.drawMap(self.map.map, (220, 220, 220))
        self.graphic.drawAmr(self.amr)
        pygame.display.set_caption(
            f"MazeRunner ({self.strategy}) | steps: {self.steps} | "
            f"visited: {len(self.visited)}/{self.reachable} reachable nodes")
        pygame.display.flip()

    # main loop
    def run(self):

        ## startup ##
        self.initialize()

        ## main loop ##
        while self.running:

            ## process input ##
            self.input.update()
            if self.input.quit:
                self.running = False

            ## update ##
            self.update()

            self.step()

            ## draw ##
            self.draw()

            ## limit the simulation speed (moves per second) ##
            self.clock.tick(self.fps)

        ## shutdown ##
        pygame.quit()
        sys.exit()
