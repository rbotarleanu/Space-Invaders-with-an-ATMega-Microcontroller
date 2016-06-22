/*	Botarleanu Robert-Mihai 331CB
 *	Main header file for the space_invaders game, implemented for an 
 *	128x160 color LCD display.
 *
 *	All data types and the interface for the useable functions are provided
 *	here.
 */
#ifndef SPACE_INVADERS_HEADER_GUARD_
#define SPACE_INVADERS_HEADER_GUARD_

#include "lcd.h"
#include "utils.h"
#include "gfx.h"

#define ERR 2
#define SCORE_INCREASE_ON_ALIEN_KILL	50
#define SCORE_INCREASE_ON_ASTEROID		2

/* The size of the text buffer. */
#define BUFLEN		120

/* The size of each constituent square for the player/alien ships. */
#define SQU_P 		5
#define	SQU_A		2
/* The size of the bullet. */
#define BULLET_W	2
#define BULLET_H	4
#define BULLET_TIMEOUT	22

/* Game text and game square stuff. */
#define GAME_TEXT_HEIGHT 10
#define GAME_RECT_WIDTH	 (LCD_WIDTH - 5)
#define GAME_RECT_HEIGHT (LCD_HEIGHT - GAME_TEXT_HEIGHT - 5)
#define GAME_TEXT_Y		 35

/* The colors used. */
#define PLAYER_COLOR	ST7735_GREEN
#define ASTEROID_COLOR	ST7735_GREEN
#define BULLET_COLOR	ST7735_MAGENTA
#define ALIEN_COLOR		ST7735_WHITE
#define BG_COLOR		ST7735_BLACK
#define TEXT_COLOR		ST7735_WHITE

/* Alien properties. */
#define COLLISION_POINTS_COUNT	11
#define ALIEN_H					(8 * SQU_A)
#define ALIEN_W					(10 * SQU_A)
#define ALIEN_START_Y			(GAME_TEXT_Y + GAME_TEXT_HEIGHT + 5 + ALIEN_H)
#define FLEET_SIZE_W			5
#define FLEET_SIZE_H			2
#define FLEET_SIZE				(FLEET_SIZE_W * FLEET_SIZE_H)

/* The asteroid properties. */
#define SQU_A		2
#define SQU_SZ		10
#define AST_Y		(LCD_HEIGHT - SQU_P * 10)
#define AST_CNT		4
#define AST_W		(SQU_A * SQU_SZ)
#define AST_H		(SQU_A * SQU_SZ)

/* Palyer properties. */
#define PLAYER_MOVE		SQU_P
#define PLAYER_W		(SQU_P * 3)
#define PLAYER_H		(SQU_P * 2)


/* The collision points for the alien. These will be in SQU_As,
 * from the alien's bottom-leftmost corner.
 */
extern const char collision_pts_x[];
extern const char collision_pts_y[];

/* The player datatype. */
typedef struct player {
	/* Coordinates. */
	unsigned char x;
	unsigned char y;
	
} player_t;

/* The space invader datatype .*/
typedef struct alien {
	/* Coordinates. */
	unsigned char x;
	unsigned char y;
	
	char dead;
} alien_t;

/* The bullet datatype. */
typedef struct bullet {
	/* Coordinates. */
	unsigned char x;
	unsigned char y;
	
	unsigned char timeout;
	unsigned char dir;
} bullet_t;

/* The asteroid datatype. */
typedef struct asteroid {
	/* Coordinates. */
	unsigned char x;
	unsigned char y;
	
	/* The component parts. */
	unsigned char parts[SQU_SZ][SQU_SZ];
} asteroid_t;

/* The interface functions. */

/* Draws a player at a given position. */
void draw_player(const player_t, char);

/* Draws an alien at a given position. */
void draw_alien(const alien_t, char);

/* Draws a bullet. */
void draw_bullet(const bullet_t, char);

/* Starts a bullet from the player ship turret. */
void shoot_bullet(bullet_t *, const player_t);

/* Draws an asteroid. */
void draw_asteroid(const asteroid_t, char);

/* Sets the parts of the asteroid as unhit. */
void setup_asteroid_parts(asteroid_t *);

/* Starts a bullet from an alien ship. */
void shoot_bullet_alien(bullet_t *, const alien_t);

#endif