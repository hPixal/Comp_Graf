import glfw
from OpenGL.GL import *
from OpenGL.GL.shaders import compileProgram, compileShader
import numpy as np
from PIL import Image
import math
import glm

# Shader de vértices
vertex_shader = """
#version 330 core
layout(location = 0) in vec3 position;  // Coordenadas del vértice
layout(location = 1) in vec2 texCoord;  // Coordenadas de textura

out vec2 TexCoord;  // Enviamos las coordenadas de textura al shader de fragmento

uniform mat4 model;       // Matriz de modelo
uniform mat4 view;        // Matriz de vista
uniform mat4 projection;  // Matriz de proyección

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0);  // Transformamos la posición del vértice
    TexCoord = texCoord;  // Pasamos las coordenadas de textura al shader de fragmento
}
"""

# Shader de fragmentos
fragment_shader = """
#version 330 core
in vec2 TexCoord;  // Coordenadas de textura recibidas del shader de vértices
out vec4 FragColor;  // Color del fragmento final
uniform sampler2D texture1;  // Textura

void main()
{
    FragColor = texture(texture1, TexCoord);  // Aplicamos la textura usando las coordenadas de textura
}
"""

def generate_texture_coordinates_cylindrical(vertices):
    """
    Genera coordenadas de textura cilíndricas para cada vértice de la esfera.
    """
    newpoints = []
    for point in vertices:
        x, y, z = point
        theta = math.atan2(z, x)  # Ángulo en el plano xy
        if theta < 0:
            theta += 2 * math.pi
            
        s = (-theta) / (2 * math.pi) + 0.5  # Coordenada de textura s
        t = (y + 1) / 2  # Coordenada de textura t (suponiendo que y varía de -1 a 1)
        
        if t > 1:
            t = 1
        
        if s > 1:
            s = 1
            
        
        newpoints.append([s, t])
    return np.array(newpoints, dtype=np.float32)

def load_texture(path):
    """
    Carga una textura desde un archivo y la devuelve como un objeto de textura de OpenGL.
    """
    image = Image.open(path)
    image = image.transpose(Image.FLIP_TOP_BOTTOM)  # Volteamos la imagen para corregir la orientación
    img_data = image.convert("RGBA").tobytes()
    width, height = image.size

    texture = glGenTextures(1)  # Generamos un identificador para la textura
    glBindTexture(GL_TEXTURE_2D, texture)  # Enlazamos la textura
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, img_data)  # Cargamos los datos de la textura
    glGenerateMipmap(GL_TEXTURE_2D)  # Generamos mipmaps para mejorar la calidad de la textura

    return texture

def create_sphere(radius, slices, stacks):
    """
    Crea una esfera con las coordenadas de los vértices y los índices de los triángulos.
    """
    vertices = []
    indices = []
    for i in range(stacks + 1):
        lat = math.pi / 2 - i * math.pi / stacks  # Ángulo de latitud
        for j in range(slices + 1):
            lon = 2 * math.pi * j / slices  # Ángulo de longitud
            x = radius * math.cos(lat) * math.cos(lon)
            y = radius * math.sin(lat)
            z = radius * math.cos(lat) * math.sin(lon)
            vertices.append([x, y, z])  # Añadimos el vértice calculado
    vertices = np.array(vertices, dtype=np.float32)

    for i in range(stacks):
        for j in range(slices):
            first = i * (slices + 1) + j
            second = first + slices + 1
            indices.append(first)
            indices.append(second)
            indices.append(first + 1)
            indices.append(second)
            indices.append(second + 1)
            indices.append(first + 1)
    indices = np.array(indices, dtype=np.uint32)
    return vertices, indices

def main():
    if not glfw.init():
        return

    window = glfw.create_window(800, 600, "Sphere with Texture", None, None)
    if not window:
        glfw.terminate()
        return

    glfw.make_context_current(window)

    # Compilamos los shaders
    shader = compileProgram(
        compileShader(vertex_shader, GL_VERTEX_SHADER),
        compileShader(fragment_shader, GL_FRAGMENT_SHADER)
    )

    # Creamos la esfera y generamos las coordenadas de textura
    sphere_vertices, sphere_indices = create_sphere(1.0, 40, 40)
    sphere_tex_coords = generate_texture_coordinates_cylindrical(sphere_vertices)

    # Configuramos los buffers y el VAO
    VAO = glGenVertexArrays(1)
    VBO = glGenBuffers(1)
    EBO = glGenBuffers(1)
    TBO = glGenBuffers(1)

    glBindVertexArray(VAO)

    glBindBuffer(GL_ARRAY_BUFFER, VBO)
    glBufferData(GL_ARRAY_BUFFER, sphere_vertices.nbytes, sphere_vertices, GL_STATIC_DRAW)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, None)
    glEnableVertexAttribArray(0)

    glBindBuffer(GL_ARRAY_BUFFER, TBO)
    glBufferData(GL_ARRAY_BUFFER, sphere_tex_coords.nbytes, sphere_tex_coords, GL_STATIC_DRAW)
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, None)
    glEnableVertexAttribArray(1)

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO)
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_indices.nbytes, sphere_indices, GL_STATIC_DRAW)

    # Cargamos y configuramos la textura
    texture = load_texture("test_texture.jpg")

    glUseProgram(shader)
    glUniform1i(glGetUniformLocation(shader, "texture1"), 0)  # Configuramos la textura

    # Configuramos las matrices de transformación
    projection = glm.perspective(glm.radians(45.0), 800/600, 0.1, 100.0)
    view = glm.translate(glm.mat4(1.0), glm.vec3(0.0, 0.0, -3.0))
    model = glm.mat4(1.0)

    projection_loc = glGetUniformLocation(shader, "projection")
    view_loc = glGetUniformLocation(shader, "view")
    model_loc = glGetUniformLocation(shader, "model")

    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm.value_ptr(projection))
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm.value_ptr(view))

    glEnable(GL_DEPTH_TEST)  # Habilitamos el test de profundidad

    while not glfw.window_should_close(window):
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)  # Limpiamos el buffer

        glActiveTexture(GL_TEXTURE0)
        glBindTexture(GL_TEXTURE_2D, texture)  # Enlazamos la textura activa

        angle = glfw.get_time()/4  # Calculamos el ángulo de rotación
        model = glm.rotate(glm.mat4(1.0), angle, glm.vec3(0.0, 1.0, 0.0))
        glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm.value_ptr(model))  # Actualizamos la matriz del modelo

        glBindVertexArray(VAO)
        glDrawElements(GL_TRIANGLES, len(sphere_indices), GL_UNSIGNED_INT, None)  # Dibujamos la esfera

        glfw.swap_buffers(window)
        glfw.poll_events()

    glDeleteVertexArrays(1, [VAO])
    glDeleteBuffers(1, [VBO, EBO, TBO])
    glfw.terminate()

if __name__ == "__main__":
    main()
