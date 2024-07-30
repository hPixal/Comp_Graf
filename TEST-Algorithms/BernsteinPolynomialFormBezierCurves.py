import numpy as np
import matplotlib.pyplot as plt
from matplotlib.gridspec import GridSpec

fig = plt.figure(figsize=(12, 6))
gs = GridSpec(1, 2, width_ratios=[2, 1])

ax_main = fig.add_subplot(gs[0])
ax_poly = fig.add_subplot(gs[1])

points = []
colors = ['ro', 'bo', 'go', 'mo']
lastadded = 0

def cubicBezierCurvePosition(p0, p1, p2, p3, t):
    A = (1 - t) * p0 + t * p1
    B = (1 - t) * p1 + t * p2
    C = (1 - t) * p2 + t * p3
    D = (1 - t) * A + t * B
    E = (1 - t) * B + t * C
    P = (1 - t) * D + t * E
    return P

def getBezierPolys():
    p = np.zeros((4, 4))
    p[0] = [1, 0, 0, 0]
    p[1] = [-3, 3, 0, 0]
    p[2] = [3, -6, 3, 0]
    p[3] = [-1, 3, -3, 1]
    return [np.poly1d(coeff) for coeff in p]

def onclick(event):
    global lastadded
    if event.inaxes == ax_main:
        if len(points) < 4:
            points.append((event.xdata, event.ydata))
        else:
            points[lastadded % 4] = (event.xdata, event.ydata)
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
        ax_main.plot(point[0], point[1], colors[i % 4])

    if len(points) > 1:
        for i in range(len(points) - 1):
            ax_main.plot([points[i][0], points[i + 1][0]], [points[i][1], points[i + 1][1]], 'k-')

    if len(points) == 4:
        dots = []
        for t in np.arange(0, 1.01, 0.01):
            dots.append(cubicBezierCurvePosition(np.array(points[0]), np.array(points[1]), np.array(points[2]), np.array(points[3]), t))
        dots = np.array(dots)
        ax_main.plot(dots[:, 0], dots[:, 1], 'b-')

        bezier_polys = getBezierPolys()
        t_values = np.linspace(0, 1, 100)
        for i, poly in enumerate(bezier_polys):
            poly_values = poly(t_values)
            ax_poly.plot(t_values, poly_values, colors[i % 4][0] + '-', label=f'P{i}')
        
        ax_poly.legend()

    fig.canvas.draw()

def main():
    ax_main.set_xlim(0, 10)
    ax_main.set_ylim(0, 10)
    ax_poly.set_xlim(0, 1)
    ax_poly.set_ylim(-1, 2)
    cid = fig.canvas.mpl_connect('button_press_event', onclick)
    plt.show()

main()
