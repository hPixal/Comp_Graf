import numpy as np
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec

fig = plt.figure(figsize=(12, 6))
gs = GridSpec(1, 2, width_ratios=[2, 1])

ax_main = fig.add_subplot(gs[0])
ax_poly = fig.add_subplot(gs[1])

points = []
colors = ['ro', 'bo', 'go', 'mo', 'yo', 'co', 'ko']
lastadded = 0



def onclick(event):
    global lastadded
    if event.inaxes == ax_main:
        if len(points) < 6:
            points.append((event.xdata, event.ydata))
        else:
            points[lastadded % 6] = (event.xdata, event.ydata)
        lastadded += 1
        refresh()

def refresh():
    ax_main.clear()
    ax_poly.clear()

    ax_main.set_xlim(0, 10)
    ax_main.set_ylim(0, 10)
    ax_poly.set_xlim(0, 1)
    ax_poly.set_ylim(-1, 2)

    for i, point in enumerate(points):
        ax_main.plot(point[0], point[1], colors[i % 6])

    if len(points) > 1:
        for i in range(len(points) - 1):
            ax_main.plot([points[i][0], points[i + 1][0]], [points[i][1], points[i + 1][1]], 'k-')

    if len(points) == 6:
        dots = []

    fig.canvas.draw()

def main():
    ax_main.set_xlim(0, 10)
    ax_main.set_ylim(0, 10)
    ax_poly.set_xlim(0, 1)
    ax_poly.set_ylim(-1, 2)
    cid = fig.canvas.mpl_connect('button_press_event', onclick)
    plt.show()

main()
