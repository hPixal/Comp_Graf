import pygame
import pygame_gui

def rgb_to_hsl(r, g, b):
    r /= 255.0
    g /= 255.0
    b /= 255.0

    c_max = max(r, g, b)
    c_min = min(r, g, b)
    delta = c_max - c_min

    l = (c_max + c_min) / 2

    if delta == 0:
        s = 0
    else:
        s = delta / (1 - abs(2 * l - 1))

    if delta == 0:
        h = 0
    elif c_max == r:
        h = 60 * (((g - b) / delta) % 6)
    elif c_max == g:
        h = 60 * (((b - r) / delta) + 2)
    elif c_max == b:
        h = 60 * (((r - g) / delta) + 4)

    h = round(h) % 360
    s = round(s * 100)
    l = round(l * 100)

    return h, s, l


def hsl_to_rgb(h, s, l):
    s /= 100.0
    l /= 100.0

    def hue_to_rgb(p, q, t):
        if t < 0:
            t += 1
        if t > 1:
            t -= 1
        if t < 1 / 6:
            return p + (q - p) * 6 * t
        if t < 1 / 2:
            return q
        if t < 2 / 3:
            return p + (q - p) * (2 / 3 - t) * 6
        return p

    if s == 0:
        r = g = b = l
    else:
        q = l * (1 + s) if l < 0.5 else l + s - l * s
        p = 2 * l - q
        h /= 360.0
        r = hue_to_rgb(p, q, h + 1 / 3)
        g = hue_to_rgb(p, q, h)
        b = hue_to_rgb(p, q, h - 1 / 3)

    r = round(r * 255)
    g = round(g * 255)
    b = round(b * 255)

    return r, g, b


# Initialize Pygame
pygame.init()

# Display dimensions
width, height = 500, 400
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption("Dynamic RGB/HSL Input with Gamma")

# Pygame GUI Manager
manager = pygame_gui.UIManager((width, height))

# Input fields
rgb_input = pygame_gui.elements.UITextEntryLine(
    relative_rect=pygame.Rect((50, 300), (150, 30)),
    manager=manager,
    placeholder_text="RGB (e.g., 255,0,128)",
)

hsl_input = pygame_gui.elements.UITextEntryLine(
    relative_rect=pygame.Rect((250, 300), (150, 30)),
    manager=manager,
    placeholder_text="HSL (e.g., 360,100,50)",
)

gamma_input = pygame_gui.elements.UITextEntryLine(
    relative_rect=pygame.Rect((150, 350), (150, 30)),
    manager=manager,
    placeholder_text="Gamma (e.g., 2.2)",
)

# Function to draw a "pixel" with RGB stripes
def draw_pixel_with_stripes(x, y, size, r, g, b, gamma):
    stripe_width = size // 3

    def apply_gamma(value, gamma):
        return int(((value / 255.0) ** gamma) * 255.0)

    # def remove_gamma(value, gamma):
    #     return int(((value / 255.0) ** (1 / gamma)) * 255.0)

    r, g, b = apply_gamma(r, gamma), apply_gamma(g, gamma), apply_gamma(b, gamma)

    pygame.draw.rect(screen, (r, 0, 0), (x, y, stripe_width, size))  # Red stripe
    pygame.draw.rect(screen, (0, g, 0), (x + stripe_width, y, stripe_width, size))  # Green stripe
    pygame.draw.rect(screen, (0, 0, b), (x + 2 * stripe_width, y, stripe_width, size))  # Blue stripe


# Initial color and gamma
r, g, b = 255, 255, 255
gamma = 2.6

# Main loop
clock = pygame.time.Clock()
running = True
pixel_size = 6

while running:
    time_delta = clock.tick(30) / 1000.0
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

        # Handle user input
        if event.type == pygame_gui.UI_TEXT_ENTRY_FINISHED:
            if event.ui_element == rgb_input:
                try:
                    r, g, b = map(int, rgb_input.get_text().split(","))
                    r, g, b = max(0, min(255, r)), max(0, min(255, g)), max(0, min(255, b))
                    hsl_input.set_text(f"{rgb_to_hsl(r, g, b)[0]},{rgb_to_hsl(r, g, b)[1]},{rgb_to_hsl(r, g, b)[2]}")
                except ValueError:
                    print("Invalid RGB input")
            elif event.ui_element == hsl_input:
                try:
                    h, s, l = map(int, hsl_input.get_text().split(","))
                    h, s, l = max(0, min(360, h)), max(0, min(100, s)), max(0, min(100, l))
                    r, g, b = hsl_to_rgb(h, s, l)
                    rgb_input.set_text(f"{r},{g},{b}")
                except ValueError:
                    print("Invalid HSL input")
            elif event.ui_element == gamma_input:
                try:
                    gamma = float(gamma_input.get_text())
                    gamma = max(0.1, min(5.0, gamma))  # Limit gamma to a reasonable range
                except ValueError:
                    print("Invalid Gamma input")

        manager.process_events(event)

    # Update the GUI
    manager.update(time_delta)

    # Clear the screen
    screen.fill((0, 0, 0))

    # Render the grid of "pixels"
    for y in range(128):
        for x in range(128):
            draw_pixel_with_stripes(x * pixel_size, y * pixel_size, pixel_size, r, g, b, gamma)

    # Render the GUI
    manager.draw_ui(screen)

    # Update the display
    pygame.display.flip()

pygame.quit()
