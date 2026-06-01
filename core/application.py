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
    def __init__(self, screenSize=[1400, 800]):
        
        self.graphic = Graphics(screenSize)   
        
        # determine is main loop is active
        self.running = True
        # manage time-related data and operations
        self.clock = pygame.time.Clock()
        self.lastTime = pygame.time.get_ticks()
        # manage user input
        self.input = Input()
        self.map = Maps()
        self.map.randomMap()
        self.amr = Amrs(position=[4*self.unitDistance()[1], 2*self.unitDistance()[0]])
        self.sensor = Sensors(position=[4*self.unitDistance()[1], 2*self.unitDistance()[0]])
        self.processor = Processors()

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
            # Event handling
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    self.running = False

            ## update ##                
            self.update()

            ## process sensor ##
            self.sensor.setPos(self.amr.pos)
            sensory = self.sensor.getOutput(self.map.map, self.graphic.screen.get_width(), self.graphic.screen.get_height())
            if sensory:
                ## amr processing ##
                self.processor.staticObstacleAvoidanceSolution(sensory)
                solution = self.processor.makeDecision()
                if solution is not None:
                    if solution in [90, 270]:
                        self.amr.speed = self.unitDistance()[0]
                    elif solution in [0, 180]:
                        self.amr.speed = self.unitDistance()[1]
                    self.amr.heading = solution
                    self.amr.moveForward(self.amr.speed)

            ## actuator processing ##

            ## draw ##
            self.graphic.screen.fill((255, 255, 255))
            
            # Draw map
            self.graphic.drawMap(self.map.map, (220, 220, 220))
            # Draw amr as a small rectangle
            self.graphic.drawAmr(self.amr)
            
            ## display image on screen ##
            pygame.display.flip()

            ## pause if necessary to achieve 60 FPS
            self.clock.tick(3)     
             
        ## shutdown ##
        pygame.quit()
        sys.exit()