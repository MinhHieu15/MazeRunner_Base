from core.application import Application

class Test(Application):

    def initialize(self):
        print("Initializing program ...")

    def update(self):
        pass

# instantiate this classs and run the program
if __name__ == '__main__':
    Test().run()
