#!/usr/bin/python
# -*- coding: UTF-8 -*-

import argparse
import sys
import matplotlib
import matplotlib.pyplot as plt
from pathlib import Path


class _Exit:
    Success = 0
    Failure = 1


def parse_poses_file(path):
    with open(path) as f:
        lines = f.readlines()

    xs = []
    zs = []
    for line in lines:
        fields = line.split(' ')
        x = float(fields[3])
        y = float(fields[7])
        z = float(fields[11])
        xs.append(x)
        zs.append(z)

    return xs, zs


def main():
    parser = argparse.ArgumentParser(description='Compare ground truth poses with the computed ones.',
                                     formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-g', '--ground', help='Location of ground truth poses file', required=True)
    parser.add_argument('-c', '--computed', help='Location of computed poses file', required=True)
    parser.add_argument('-s', '--steps', help='Steps to show arrows', default=5)

    args = parser.parse_args()

    groundTruthPath = Path(args.ground).resolve()
    computedPath = Path(args.computed).resolve()

    print(groundTruthPath)
    print(computedPath)

    gX, gZ = parse_poses_file(groundTruthPath)
    cX, cZ = parse_poses_file(computedPath)

    fig, ax = plt.subplots()

    ax.plot(gX, gZ, label='GT')         # GT data
    ax.plot(gX[0], gZ[0], 'ro')         # GT first point in red
    ax.plot(gX[-1], gZ[-1], 'go')       # GT last point in green
    for i in range(0, len(gX)-1, int(args.steps)):
        ax.arrow(gX[i], gZ[i], gX[i+1]-gX[i], gZ[i+1]-gZ[i], shape='full', lw=0, length_includes_head=False,
                 head_width=0.5)

    ax.plot(cX, cZ, label='Computed')   # computed data
    ax.plot(cX[0], cZ[0], 'yo')         # computed first point in red
    ax.plot(cX[-1], cZ[-1], 'bo')       # computed last point in green
    for i in range(0, len(cX)-1, int(args.steps)):
        ax.arrow(cX[i], cZ[i], cX[i+1]-cX[i], cZ[i+1]-cZ[i], shape='full', lw=0, length_includes_head=False,
                 head_width=0.5, color='r')

    ax.set(xlabel='x coordinate', ylabel='z coordinate',
           title='KITTY Odometry comparison (GT and computed poses)')
    ax.grid()
    ax.legend()
    plt.show()

    return _Exit.Success


if __name__ == "__main__":
    sys.exit(main())
