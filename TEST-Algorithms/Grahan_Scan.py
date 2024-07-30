import scipy as sp
import numpy as np
import matplotlib.pyplot as plt
from collections import deque

# Constants
points = []
fig, ax = plt.subplots()

def squared_distance(p1, p2):
    return (p1[0] - p2[0]) ** 2 + (p1[1] - p2[1]) ** 2

def polar_angle(p, P0):
    y_span = p[1] - P0[1]
    x_span = p[0] - P0[0]
    return np.atan2(y_span, x_span)

def ccw(P0, P1, P2):
    return (P1[0] - P0[0]) * (P2[1] - P0[1]) > (P2[0] - P0[0]) * (P1[1] - P0[1])

def Graham_Scan(points):
    stack = deque()
    P0 = min(points, key=lambda point: (point[1], point[0]))
    print ("P0: ", P0)
    points.append(P0)
    stack.append(P0);
    sorted_points = sorted(points, key=lambda p: (polar_angle(p,P0), -squared_distance(P0, p)))
    
    unique_points = []
    last_angle = None

    for point in sorted_points:
        angle = polar_angle(point,P0)
        if angle != last_angle:
            unique_points.append(point)
            last_angle = angle
    
    for point in unique_points:
        while len(stack) > 1 and not ccw(stack[-2], stack[-1], point):
            stack.pop()
        stack.append(point)
    
    return stack
    
# Function to be called when the plot is clicked
def onclick(event):
    # Check if the click is within the axes
    if event.inaxes:
        # Save the points
        points.append((event.xdata, event.ydata))
        refresh()
        
def refresh():
    ax.clear()
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    ax.plot([p[0] for p in points], [p[1] for p in points], 'ro')
    
    convex_hull = Graham_Scan(points);
    convex_hull.append(convex_hull[0]);
    ax.plot([p[0] for p in convex_hull], [p[1] for p in convex_hull], 'b-');
    
    fig.canvas.draw()
    
def main():
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    # Connect the click event to the onclick function
    cid = fig.canvas.mpl_connect('button_press_event', onclick)
    # Show the plot 
    plt.show()
    
main()

