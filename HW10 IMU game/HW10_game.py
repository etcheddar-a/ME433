# tilt_maze.py
#
# Simple tilt-maze game in Pygame Zero.
# For now:
#   - Arrow keys simulate IMU tilt
# Later:
#   - Replace tilt_x / tilt_y with serial IMU data
#
# Run with:
#   pgzrun tilt_maze.py

import math
from pygame import Rect
import pgzrun
import serial

WIDTH = 800
HEIGHT = 600

TITLE = "Tilt Maze"

ser = serial.Serial('/dev/tty.usbmodem101')
print('Opening port: ' + ser.name)

# =========================================================
# BALL
# =========================================================

ball_pos = [100.0, 100.0]
ball_vel = [0.0, 0.0]

BALL_RADIUS = 15

# simulated tilt acceleration
tilt_x = 0.0
tilt_y = 0.0

ACCEL = 0.35
FRICTION = 0.985
MAX_SPEED = 12

# =========================================================
# LEVEL
# =========================================================

walls = [
    Rect((150, 0), (20, 450)),
    Rect((300, 150), (20, 450)),
    Rect((450, 0), (20, 450)),
    Rect((600, 150), (20, 450)),

    Rect((150, 430), (80, 20)),
    Rect((300, 150), (80, 20)),
    Rect((450, 430), (80, 20)),
]

goal = Rect((700, 500), (60, 60))

# =========================================================
# UPDATE
# =========================================================

def update():
    global tilt_x, tilt_y

    # retrieve tilt data
    n_str = ser.read_until(b'\n').decode().strip()
    n_list = list(map(float, n_str.split(',')))
    tilt_x = -n_list[0] * 3
    tilt_y = n_list[1] * 3

    # handle_input()
    update_physics()
    check_goal()


def handle_input():
    global tilt_x, tilt_y

    tilt_x = 0
    tilt_y = 0

    # simulate tilt with keyboard
    if keyboard.left:
        tilt_x = -1

    if keyboard.right:
        tilt_x = 1

    if keyboard.up:
        tilt_y = -1

    if keyboard.down:
        tilt_y = 1


def update_physics():

    # acceleration from tilt
    ball_vel[0] += tilt_x * ACCEL
    ball_vel[1] += tilt_y * ACCEL

    # friction
    ball_vel[0] *= FRICTION
    ball_vel[1] *= FRICTION

    # clamp speed
    speed = math.hypot(ball_vel[0], ball_vel[1])

    if speed > MAX_SPEED:
        scale = MAX_SPEED / speed
        ball_vel[0] *= scale
        ball_vel[1] *= scale

    # move on x axis
    ball_pos[0] += ball_vel[0]
    handle_wall_collisions(axis='x')

    # move on y axis
    ball_pos[1] += ball_vel[1]
    handle_wall_collisions(axis='y')

    # screen bounds
    keep_ball_in_screen()


def handle_wall_collisions(axis):

    ball_rect = Rect(
        (ball_pos[0] - BALL_RADIUS,
         ball_pos[1] - BALL_RADIUS),
        (BALL_RADIUS * 2,
         BALL_RADIUS * 2)
    )

    for wall in walls:

        if ball_rect.colliderect(wall):

            if axis == 'x':

                if ball_vel[0] > 0:
                    ball_pos[0] = wall.left - BALL_RADIUS

                elif ball_vel[0] < 0:
                    ball_pos[0] = wall.right + BALL_RADIUS

                ball_vel[0] *= -0.4

            elif axis == 'y':

                if ball_vel[1] > 0:
                    ball_pos[1] = wall.top - BALL_RADIUS

                elif ball_vel[1] < 0:
                    ball_pos[1] = wall.bottom + BALL_RADIUS

                ball_vel[1] *= -0.4


def keep_ball_in_screen():

    if ball_pos[0] < BALL_RADIUS:
        ball_pos[0] = BALL_RADIUS
        ball_vel[0] *= -0.5

    if ball_pos[0] > WIDTH - BALL_RADIUS:
        ball_pos[0] = WIDTH - BALL_RADIUS
        ball_vel[0] *= -0.5

    if ball_pos[1] < BALL_RADIUS:
        ball_pos[1] = BALL_RADIUS
        ball_vel[1] *= -0.5

    if ball_pos[1] > HEIGHT - BALL_RADIUS:
        ball_pos[1] = HEIGHT - BALL_RADIUS
        ball_vel[1] *= -0.5


def check_goal():

    ball_rect = Rect(
        (ball_pos[0] - BALL_RADIUS,
         ball_pos[1] - BALL_RADIUS),
        (BALL_RADIUS * 2,
         BALL_RADIUS * 2)
    )

    if ball_rect.colliderect(goal):
        reset_ball()


def reset_ball():

    ball_pos[0] = 100
    ball_pos[1] = 100

    ball_vel[0] = 0
    ball_vel[1] = 0


# =========================================================
# DRAW
# =========================================================

def draw():

    screen.clear()

    # background
    screen.fill((30, 30, 40))

    # goal
    screen.draw.filled_rect(goal, (50, 200, 70))

    # walls
    for wall in walls:
        screen.draw.filled_rect(wall, (200, 200, 220))

    # ball
    screen.draw.filled_circle(
        (ball_pos[0], ball_pos[1]),
        BALL_RADIUS,
        (255, 120, 120)
    )

pgzrun.go()