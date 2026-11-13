import argparse
from core.application import Application


def main():
    parser = argparse.ArgumentParser(description="Micromouse-style AMR simulation in pygame")
    parser.add_argument("--seed", type=int, help="random seed, same seed gives the same map and run")
    parser.add_argument("--rows", type=int, default=30, help="map rows, including the border")
    parser.add_argument("--cols", type=int, default=30, help="map columns, including the border")
    parser.add_argument("--width", type=int, default=1400, help="window width in pixels")
    parser.add_argument("--height", type=int, default=800, help="window height in pixels")
    parser.add_argument("--fps", type=int, default=3, help="moves per second")
    parser.add_argument("--strategy", choices=["explore", "random"], default="explore",
                        help="explore: go to unvisited nodes first; random: random walk")
    args = parser.parse_args()

    Application(screenSize=[args.width, args.height], mapSize=[args.rows, args.cols],
                fps=args.fps, seed=args.seed, strategy=args.strategy).run()


if __name__ == "__main__":
    main()
