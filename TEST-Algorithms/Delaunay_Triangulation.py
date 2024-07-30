import numpy as np
import matplotlib.pyplot as plt
from matplotlib.patches import Circle

# Constants
points = []
fig, ax = plt.subplots()

def circumcenter(triangle):
    A, B, C = np.array(triangle[0]), np.array(triangle[1]), np.array(triangle[2])
    D = 2 * (A[0] * (B[1] - C[1]) + B[0] * (C[1] - A[1]) + C[0] * (A[1] - B[1]))
    Ux = ((np.linalg.norm(A)**2) * (B[1] - C[1]) + (np.linalg.norm(B)**2) * (C[1] - A[1]) + (np.linalg.norm(C)**2) * (A[1] - B[1])) / D
    Uy = ((np.linalg.norm(A)**2) * (C[0] - B[0]) + (np.linalg.norm(B)**2) * (A[0] - C[0]) + (np.linalg.norm(C)**2) * (B[0] - A[0])) / D
    return np.array([Ux, Uy])

def circumradius(triangle):
    circum_center = circumcenter(triangle)
    radius = np.linalg.norm(circum_center - np.array(triangle[0]))
    return radius

def is_point_in_circumcircle(triangle, point):
    circum_center = circumcenter(triangle)
    radius = circumradius(triangle)
    distance = np.linalg.norm(circum_center - np.array(point))
    return distance <= radius

def bowyer_watson(points):
    # Define the supertriangle
    min_x = min([p[0] for p in points])
    min_y = min([p[1] for p in points])
    max_x = max([p[0] for p in points])
    max_y = max([p[1] for p in points])
    dx = max_x - min_x
    dy = max_y - min_y
    delta_max = max(dx, dy)
    mid_x = (min_x + max_x) / 2
    mid_y = (min_y + max_y) / 2

    supertriangle = [(mid_x - 20 * delta_max, mid_y - delta_max),
                     (mid_x, mid_y + 20 * delta_max),
                     (mid_x + 20 * delta_max, mid_y - delta_max)]
    triangles = [supertriangle]

    # Create the Delaunay triangulation
    for point in points:
        bad_triangles = []
        for triangle in triangles:
            if is_point_in_circumcircle(triangle, point):
                bad_triangles.append(triangle)
        
        polygon = []
        for triangle in bad_triangles:
            for i in range(3):
                edge = (triangle[i], triangle[(i + 1) % 3])
                if not any(edge == (tri[(j + 1) % 3], tri[j]) for tri in bad_triangles for j in range(3)):
                    polygon.append(edge)
        
        for triangle in bad_triangles:
            triangles.remove(triangle)
        
        for edge in polygon:
            triangles.append([edge[0], edge[1], point])

    triangles = [triangle for triangle in triangles if not any(vertex in supertriangle for vertex in triangle)]
    return triangles

def onclick(event):
    if event.inaxes:
        points.append((event.xdata, event.ydata))
        refresh()
        
def refresh():
    ax.clear()
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    ax.plot([p[0] for p in points], [p[1] for p in points], 'ro')
    
    if len(points) >= 3:
        triangles = bowyer_watson(points)
        for tri in triangles:
            triangle = [tri[i] for i in range(3)]
            triangle.append(triangle[0])  # Close the triangle
            ax.plot([p[0] for p in triangle], [p[1] for p in triangle], 'b-')
            
            # Plot circumcircle
            circum_center = circumcenter(triangle)
            radius = circumradius(triangle)
            circum_circle = Circle(circum_center, radius, fill=False, edgecolor='green', linestyle='dotted')
            ax.add_patch(circum_circle)

    fig.canvas.draw()
    
def main():
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    # Connect the click event to the onclick function
    cid = fig.canvas.mpl_connect('button_press_event', onclick)
    # Show the plot 
    plt.show()
    
main()
