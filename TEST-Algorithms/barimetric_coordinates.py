import numpy as np
import matplotlib.pyplot as plt

fig, ax = plt.subplots();
triangle = [[2,2], [5,8], [8,3]];

def circumcenter(triangle):
    A, B, C = np.array(triangle[0]), np.array(triangle[1]), np.array(triangle[2])
    AB_m = (B - A)*1/2
    AC_m = (C - A)*1/2
    BC_m = (C - B)*1/2
    
    D = 2 * (AB_m[0] * BC_m[1] - AB_m[1] * BC_m[0])
    Ux = ((np.linalg.norm(A)**2) * BC_m[1] - (np.linalg.norm(B)**2) * BC_m[1] + (np.linalg.norm(C)**2) * BC_m[1]) / D
    Uy = ((np.linalg.norm(A)**2) * BC_m[0] - (np.linalg.norm(B)**2) * BC_m[0] + (np.linalg.norm(C)**2) * BC_m[0]) / D
    return np.array([Ux, Uy])
    
def baricenter(triangle):
    A, B, C = np.array(triangle[0]), np.array(triangle[1]), np.array(triangle[2])
    return (A + B + C) / 3; 

def cross2d(v1, v2):
    return v1[0] * v2[1] - v1[1] * v2[0];


def getBarycenttricCoordinates(triangle, point):
    A, B, C = np.array(triangle[0]), np.array(triangle[1]), np.array(triangle[2])
    P = np.array(point);
    AB = B-A;
    AC = C-A;
    BC = C-B;

    BP = P-B;
    AP = P-A;
    
    a = np.linalg.norm(cross2d(AB,AP))/2
    b = np.linalg.norm(cross2d(AC,AP))/2
    c = np.linalg.norm(cross2d(BC,BP))/2
    
    area = a+b+c;
    a = a/area;
    b = b/area;
    c = c/area;
    
    return np.array([a,b,c])
    
    
def getPointFromBarycentric(triangle, barcientric):
    A, B, C = np.array(triangle[0]), np.array(triangle[1]), np.array(triangle[2])
    return A * barcientric[0] + B * barcientric[1] + C * barcientric[2]
    

def onclick(event):
    if event.inaxes:
        point = [event.xdata, event.ydata];
        refresh(point)
        
def refresh(point=[]):
    ax.clear()
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    ax.plot([p[0] for p in triangle], [p[1] for p in triangle], 'ro')
    ax.plot([p[0] for p in triangle], [p[1] for p in triangle], '-b')
    ax.plot([triangle[0][0],triangle[2][0]],[triangle[0][1],triangle[2][1]],'-b')
    
    if(len(point) > 0): 
        ax.plot(point[0], point[1], 'ro')
        print("Point (Cartesian): ", float(point[0]), float(point[1]))
        print("Point (Barycentric): ", getBarycenttricCoordinates(triangle, point))
        print("Point (Cartesian): ", getPointFromBarycentric(triangle, getBarycenttricCoordinates(triangle, point)))

    fig.canvas.draw()
    
def main():
    ax.set_xlim(0, 10)
    ax.set_ylim(0, 10)
    # Connect the click event to the onclick function
    cid = fig.canvas.mpl_connect('button_press_event', onclick)
    # Show the plot 
    refresh()
    plt.show()
    
main()