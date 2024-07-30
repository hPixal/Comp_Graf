import numpy as np
import numpy.linalg as la
import matplotlib.pyplot as plt

# Constants
points = []
lastadded = 0
fig, ax = plt.subplots()

# This will be done in a Round-Robin style.
# points[0] and points[1] will define the line
# points[2] will be the point

def getSign():
    global points
    p1 = np.array(points[1]) - np.array(points[0])
    p2 =  np.array(points[2]) - np.array(points[0])
    val = (p1[0] * p2[1] - p1[1] * p2[0])
    print(val)
    return -1 if val > 0 else 1

def getDistance(points):
    l0, l1, p = np.array(points[0]), np.array(points[1]), np.array(points[2])
    dir = l1 - l0
    v = p - l0
    return la.norm(np.cross(dir, v)) / la.norm(dir)

def getNormalVector(points):
    l0, l1 = np.array(points[0]), np.array(points[1])
    tangent = l1 - l0
    normal = np.array([-tangent[1], tangent[0]])
    normal = normal / la.norm(normal)
    return normal

def onclick(event):
    global lastadded
    global points
    if event.inaxes:
        if len(points) < 3:
            points.append((event.xdata, event.ydata))
        else:
            points[lastadded % 3] = (event.xdata, event.ydata)
        lastadded += 1
        refresh()

def refresh():
    ax.clear()
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    ax.plot([p[0] for p in points], [p[1] for p in points], 'ro')
    
    if len(points) == 3:
        # Calculate the direction vector
        direction = np.array(points[1]) - np.array(points[0])
        direction = direction / np.linalg.norm(direction)
        
        ax.plot([points[0][0], points[1][0]], [points[0][1], points[1][1]], '-b')
        
        # Extend the line in both directions
        far_point1 = np.array(points[0]) - direction * 1000  # far away in one direction
        far_point2 = np.array(points[1]) + direction * 1000  # far away in the other direction

        ax.plot([far_point1[0], far_point2[0]], [far_point1[1], far_point2[1]], '-b')
        
        normal = getNormalVector(points)
        distance = getDistance(points)
        sign = getSign()
        nearpoint = np.array([points[2][0], points[2][1]]) + normal * distance * sign
        ax.plot([points[2][0], nearpoint[0]], [points[2][1], nearpoint[1]], '-r')
    fig.canvas.draw()

def main():
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    # Connect the click event to the onclick function
    cid = fig.canvas.mpl_connect('button_press_event', onclick)
    # Show the plot 
    plt.show()

main()
