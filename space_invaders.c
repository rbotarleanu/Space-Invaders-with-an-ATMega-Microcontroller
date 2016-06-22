#include "space_invaders.h"

/* The collision points for the alien. */
const char collision_pts_x[] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
};

const char collision_pts_y[] = {
	-1, -3, -1, 0, 0, -2, 0, 0, -1, -3, -1
};

void draw_player(const player_t player, char clear)
{
	int color = clear ? BG_COLOR : PLAYER_COLOR;
	char x = player.x;
	char y = player.y;

	/* Draw the base. */
	fillRect(x, y, SQU_P * 3, SQU_P, color);
	/* Draw the turret. */
	fillRect(x + SQU_P, y - SQU_P, SQU_P, SQU_P, color);
}

/* Draws an alien at a given position. */
void draw_alien(const alien_t alien, char clear)
{
	int color = clear ? BG_COLOR : ALIEN_COLOR;
	char x = alien.x;
	char y = alien.y;
	
	/* Draw it's left arm. */
	fillRect(x, y - SQU_A * 3, SQU_A, SQU_A * 3, color);
	fillRect(x + SQU_A, y - SQU_A * 4, SQU_A, SQU_A * 2, color);
	
	/* Draw it's body. */
	fillRect(x + SQU_A * 2, y - SQU_A * 5, SQU_A, SQU_A * 5, color);
	fillRect(x + SQU_A * 3, y - SQU_A * 5, SQU_A, SQU_A, color);
	fillRect(x + SQU_A * 3, y - SQU_A * 3, SQU_A, SQU_A * 1, color);
	fillRect(x + SQU_A * 4, y - SQU_A * 5, SQU_A * 3, SQU_A * 4, color);
	fillRect(x + SQU_A * 7, y - SQU_A * 5, SQU_A, SQU_A, color);
	fillRect(x + SQU_A * 7, y - SQU_A * 3, SQU_A, SQU_A * 1, color);
	fillRect(x + SQU_A * 8, y - SQU_A * 5, SQU_A, SQU_A * 5, color);
	
	/* Draw it's right arm. */
	fillRect(x + SQU_A * 9, y - SQU_A * 4, SQU_A, SQU_A * 2, color);
	fillRect(x + SQU_A * 10, y - SQU_A * 3, SQU_A, SQU_A * 3, color);
	
	/* Draw it's legs. */
	fillRect(x + SQU_A * 3, y, SQU_A * 2, SQU_A, color);
	fillRect(x + SQU_A * 6, y, SQU_A * 2, SQU_A, color);
	
	/* Draw it's antennae. */
	fillRect(x + SQU_A * 2, y - SQU_A * 7, SQU_A, SQU_A, color);
	fillRect(x + SQU_A * 3, y - SQU_A * 6, SQU_A, SQU_A, color);
	
	fillRect(x + SQU_A * 7, y - SQU_A * 6, SQU_A, SQU_A, color);
	fillRect(x + SQU_A * 8, y - SQU_A * 7, SQU_A, SQU_A, color);
}

/* Draws a bullet. */
void draw_bullet(const bullet_t bullet, char clear)
{
	int color = clear ? BG_COLOR : BULLET_COLOR;
	
	fillRect(bullet.x, bullet.y - BULLET_H / 2, BULLET_W, BULLET_H,
		color);
}

/* Starts a bullet from the player ship turret. */
void shoot_bullet(bullet_t *b, const player_t p)
{
	b->x = p.x + SQU_P + (SQU_P - BULLET_W) / 2; 
	b->y = p.y - SQU_P * 2 + (SQU_P - BULLET_H / 2);
	b->timeout = 0;
	b->dir = 0;
}

/* Starts a bullet from an alien ship. */
void shoot_bullet_alien(bullet_t *b, const alien_t a)
{
	b->x = a.x + ALIEN_W / 2;
	b->y = a.y;
	b->timeout = 0;
	b->dir = 1;
}

/* Draws an asteroid. */
void draw_asteroid(const asteroid_t asteroid, char clear)
{
	fillRect(asteroid.x, asteroid.y, SQU_SZ * SQU_A, SQU_SZ * SQU_A,
		ASTEROID_COLOR);
}

/* Sets the parts of the asteroid as unhit. */
void setup_asteroid_parts(asteroid_t *asteroid)
{
	char i, j;
	
	for (i = 0; i < SQU_SZ; ++i)
		for (j = 0; j < SQU_SZ; ++j)
			asteroid->parts[i][j] = 0;
}