from random import choice, randint

class Processors(object):

    def __init__(self, obstacleSolution=()):
        self.obstacleSolution = obstacleSolution
        self.path = []
        self.lastDecision = 90

    # rule for not repeating the previous position
    def rule1(self):
        pass

    def staticObstacleAvoidanceSolution(self, sensorInput)->tuple:
        self.obstacleSolution = tuple(sensorInput)
    
    def pathPlanning(self, path):
        self.path = path

    def makeDecision(self)->int:
        bestSolution = None
        if (solutionNumber:=len(self.obstacleSolution)) > 0:
            if solutionNumber > 1:
                obstacleSolution = list(self.obstacleSolution)
                if self.lastDecision in obstacleSolution:
                    obstacleSolution.remove(self.lastDecision)
                bestSolution = int(choice(obstacleSolution))
            else:
                bestSolution = int(self.obstacleSolution[0])
        self.lastDecision = bestSolution + 180
        if self.lastDecision >= 360:
            self.lastDecision %= 360
        return bestSolution
    
    def output(self):
        pass
