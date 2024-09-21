def bresenham_line(x0, y0, x1, y1):
    """Generate points for a line from (x0, y0) to (x1, y1) using Bresenham's algorithm."""
    points = []
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy

    while True:
        points.append((x0, y0))
        if x0 == x1 and y0 == y1:
            break
        e2 = 2 * err
        if e2 > -dy:
            err -= dy
            x0 += sx
        if e2 < dx:
            err += dx
            y0 += sy

    return points

def test_bresenham_line():
    """Test the Bresenham line algorithm with some example cases."""
    # Test case 1: Simple diagonal line
    points = bresenham_line(0, 0, 5, 5)
    expected = [(0, 0), (1, 1), (2, 2), (3, 3), (4, 4), (5, 5)]
    assert points == expected, f"Test 1 failed: {points} != {expected}"

    # Test case 2: Horizontal line
    points = bresenham_line(0, 0, 5, 0)
    expected = [(0, 0), (1, 0), (2, 0), (3, 0), (4, 0), (5, 0)]
    assert points == expected, f"Test 2 failed: {points} != {expected}"

    # Test case 3: Vertical line
    points = bresenham_line(0, 0, 0, 5)
    expected = [(0, 0), (0, 1), (0, 2), (0, 3), (0, 4), (0, 5)]
    assert points == expected, f"Test 3 failed: {points} != {expected}"

    # Test case 4: General case
    points = bresenham_line(2, 3, 7, 9)
    expected = [(2, 3), (3, 4), (4, 5), (5, 6), (6, 7), (7, 9)]
    assert points == expected, f"Test 4 failed: {points} != {expected}"

    print("All tests passed!")

# Run the test function
test_bresenham_line()
