
#include <avr/io.h>
#include <avr/interrupt.h>
#include "gfx.h"
#include <stdio.h>
#include "touch_api.h"
#include "touch.h"
#include "lcd.h"
#include "utils.h"
#include "util/delay.h"
#include "space_invaders.h"
#include "list.h"

#define __delay_cycles(n)     __builtin_avr_delay_cycles(n)
#define __enable_interrupt()  sei()

#define MORSE_LUNG  3
#define MORSE_SCURT 1

#define GET_SENSOR_STATE(SENSOR_NUMBER) qt_measure_data.qt_touch_status.sensor_states[(SENSOR_NUMBER/8)] & (1 << (SENSOR_NUMBER % 8))
#define GET_ROTOR_SLIDER_POSITION(ROTOR_SLIDER_NUMBER) qt_measure_data.qt_touch_status.rotor_slider_values[ROTOR_SLIDER_NUMBER]

extern void touch_measure();
extern void touch_init( void );
extern void init_system( void );
extern void init_timer_isr(void);
extern void set_timer_period(uint16_t);

/* Timer period in msec. */
uint16_t qt_measurement_period_msec = QT_MEASUREMENT_PERIOD_MS;
uint16_t time_ms_inc=0;

/* flag set by timer ISR when it's time to measure touch */
volatile uint8_t time_to_measure_touch = 0u;

/* current time, set by timer ISR */
volatile uint16_t current_time_ms_touch = 0u;

/* Globals for game-state control. */
int 			dead_aliens;
player_t 		player;

/* The score text. */
char 			buf[BUFLEN];
unsigned int 	score;
unsigned int 	highscore;

/* The asteroids and aliens. */
asteroid_t 		ast[AST_CNT];
alien_t 		invasion_fleet[FLEET_SIZE_W][FLEET_SIZE_H];

/* Bullet data. */
list 			*enemy_bullets;
int 			enemy_aggro;
int 			enemy_trigger_cnt;
int 			bullets_shot;
bullet_t b;

/* Qtouch record value. */
uint8_t lastvalue = 0;
    
/* Delay function used by the LCD library. */ 
void myDelay(int16_t ms);

/* Checks the collision between an alien and a bullet. */
static void check_collision_points(alien_t *alien, bullet_t *bullet)
{
	int i;
    bullet_t b = *bullet;
   
	/* Check for each point in the alien hitbox. */
	for (i = 0; i < COLLISION_POINTS_COUNT; ++i) {
		/* Compute within an acceptable error. */
        int dif1 = abs(b.x - (alien->x + collision_pts_x[i] * SQU_A));
        int dif2 = abs(b.y - (alien->y + collision_pts_y[i] * SQU_A));
        dif1 = (dif1 < 0) ? -dif1 : dif1;
        dif2 = (dif2 < 0) ? -dif2 : dif2;
        if (dif1 <= ERR && dif2 <= ERR) {
            /* Alien dies and bullet is destroyed. */
            alien->dead = true;
			++dead_aliens;
			score += SCORE_INCREASE_ON_ALIEN_KILL;
            draw_bullet(*bullet, true);
            draw_alien(*alien, true);
            bullet->timeout = BULLET_TIMEOUT;
			if (dead_aliens == FLEET_SIZE) {
				enemy_trigger_cnt *= 2;
				enemy_aggro = 1;
			}
            return;
        }
			
	}
}

/* Sets up the text as wrappable, the color and the minimum size. */
static void setup_text()
{
	setCursor(1, 32);
    setTextColor(TEXT_COLOR, BG_COLOR);
	setTextSize(1);
	setTextWrap(LCD_WIDTH);
	buf[0] = '\0';
}

/* Populates the asteroid field. */
static void make_asteroids()
{
	int i;
	
	for (i = 0; i < AST_CNT; ++i) {
		ast[i].x = 10 + SQU_A * SQU_SZ * i + i * 10; 
        ast[i].y = AST_Y;
        setup_asteroid_parts(&ast[i]);
    }
}

/* Populates the invasion fleet. */
static void setup_fleet()
{
    int i, j;
	dead_aliens = 0;
    
    for (i = 0; i < FLEET_SIZE_W; ++i)
        for (j = 0; j < FLEET_SIZE_H; ++j) {
            invasion_fleet[i][j].x = ALIEN_W * i + 5 * i;
            invasion_fleet[i][j].y = ALIEN_START_Y + j * ALIEN_H + 5 * j;
            invasion_fleet[i][j].dead = false;
        }
}

/* Moves a bullet vertically, dependant on who fired it. */
static void transform_bullet(bullet_t *b)
{
    draw_bullet(*b, true);
    b->y += BULLET_H * ((b->dir) ? 1 : -1);
    draw_bullet(*b, false);
    ++b->timeout;
    if (b->timeout == BULLET_TIMEOUT)
        draw_bullet(*b, true);
}

/* Checks the qtouch and the button for user input. */
static void check_controls()
{
	/* The Qtouch wants all pull-ups disabled to measure. */
    MCUCR |= (1u << PUD);
	touch_measure();
    MCUCR &= ~(1u << PUD);
   
	/* Check horizontal movement. */
    if (qt_measure_data.qt_touch_status.rotor_slider_values[0] != 0) {
        uint8_t newvalue = qt_measure_data.qt_touch_status.rotor_slider_values[0];
        if (newvalue > lastvalue && newvalue < (lastvalue + 64)) {
            if (player.x + PLAYER_MOVE + PLAYER_W < LCD_WIDTH) {
                draw_player(player, true);
                player.x += PLAYER_MOVE;
                draw_player(player, false);
                _delay_ms(10);
            }
        } else if (newvalue < lastvalue && newvalue > (lastvalue - 64)) {
            if (player.x - PLAYER_MOVE >= 0) {
                draw_player(player, true); 
                player.x -= PLAYER_MOVE;
                draw_player(player, false);
                _delay_ms(10);
            }
        }
		
        lastvalue = newvalue;
    }
 
	/* Check the fire button. */
	PORTD |= (1 << PD6);
	if ((PIND & (1 << PD6)) == 0) {
		PORTD ^= (1 << PD7);
        _delay_ms(50);
        if (b.timeout == BULLET_TIMEOUT) {
            draw_bullet(b, true);
            shoot_bullet(&b, player);
            ++bullets_shot;
        }
    }
}

/* Function that checks the collisions of a bullet with the asteroids. */
static void check_asteroid_collisions(bullet_t *bullet)
{
    int i;
    bullet_t b = *bullet;
    b.y += (BULLET_H / 2) * ((bullet->dir) ? -1 : 1);
    
    for (i = 0; i < AST_CNT; ++i) {
        if ((b.x >= ast[i].x) && (b.x <= ast[i].x + AST_W)) {
            if ((b.y >= ast[i].y) && (b.y <= ast[i].y + AST_H)) {
                /* In the bounds of an asteroid. Check for which part. */
                int row = (b.y - ast[i].y) / SQU_A;
                int col = (b.x - ast[i].x) / SQU_A;
                
                if (!ast[i].parts[row][col]) { /* This part wasn't hit before. */
                    /* Destroy the bullet and the part. */
                    bullet->timeout = BULLET_TIMEOUT;
                    draw_bullet(*bullet, true);
                    ast[i].parts[row][col] = 1;
					score += SCORE_INCREASE_ON_ASTEROID;
                    fillRect(ast[i].x + col * SQU_A, ast[i].y - row * SQU_A,
                        SQU_A, SQU_A, BG_COLOR);
                    return;
                }
            }
        }
    }
}

/* Check if the player bullet hit an alien. */
static void check_alien_hits()
{
    int i, j;
    
    for (i = 0; i < FLEET_SIZE_W; ++i)
        for (j = 0; j < FLEET_SIZE_H; ++j) {
            if (!invasion_fleet[i][j].dead)
                check_collision_points(&invasion_fleet[i][j], &b);
        }
}

/* Check if the player was hit by a bullet. */
static char check_player_is_hit(bullet_t b)
{
    if ((b.x >= player.x) && (b.x <= player.x + PLAYER_W)) {
        if ((b.y <= player.y) && (b.y >= player.y - PLAYER_H))
            return true;
    }
    
    return false;
}

/* Checks the enemy bullets, returns true if the player was hit. */
static char check_enemy_bullets()
{
    list *aux = enemy_bullets;
    
	/* Go through the bullets. */
    for (; aux != NULL; aux = aux->next) {
        if (aux->i == NULL)
            continue;
        bullet_t *ab = (bullet_t *) aux->i;
        
		/* Check asteroid and player collisions. */
        if (ab->timeout < BULLET_TIMEOUT) {
            check_asteroid_collisions(ab);
            if (check_player_is_hit(*ab))
                return true;
        }
		/* Move it downward. */
        if (ab->timeout < BULLET_TIMEOUT) {
            transform_bullet(ab);
        }
		/* Check if the bullet is destroyed. */
        if (ab->timeout >= BULLET_TIMEOUT) {
            draw_bullet(*ab, true);
            free(ab);
            aux->i = NULL;
        }
    }
	/* Player was not hit. */
    return false;
}

/* Render the invaders and the game text. */
static void render()
{
	int i, j;
	
	for (i = 0; i < FLEET_SIZE_W; ++i)
		for (j = 0; j < FLEET_SIZE_H; ++j) {
			if (!invasion_fleet[i][j].dead)
				draw_alien(invasion_fleet[i][j], false);
            }
          
	setCursor(1, GAME_TEXT_Y);
	sprintf(buf, "You: %d    HS: %d", score, highscore);
    print(buf);
}

/* Update the game state. */
static int update()
{
	int i;

	/* Check bullets and controls. */
    check_controls();
    check_alien_hits();
    if (b.timeout < BULLET_TIMEOUT) {
		check_asteroid_collisions(&b);
        check_alien_hits();
    }
       if (b.timeout < BULLET_TIMEOUT)
		transform_bullet(&b); 
        
    if (check_enemy_bullets()) { // player is hit, he loses
        list_destroy(&enemy_bullets);
		enemy_bullets = NULL;
		return 1;
    }
	
	/* Render the screen after input and collision check. */
	render();
        
	/* Check if the enemies are triggered to fire. */
    if (bullets_shot >= enemy_aggro) {
        int rand_r, rand_c;
        
		/* Generate enemy bullets. */
        bullets_shot = 0;
        for (i = 0; i < enemy_trigger_cnt; ++i) {
				bullet_t *ab = malloc(sizeof(bullet_t));
				rand_r = rand() % FLEET_SIZE_H;
				rand_c = rand() % FLEET_SIZE_W;
				if (!invasion_fleet[rand_r][rand_c].dead) {
					shoot_bullet_alien(ab, invasion_fleet[rand_r][rand_c]);
					list_append(&enemy_bullets, ab);
				}
			}
        }
	
	/* Check if the enemy fleet was destroyed, regenerate. */	
	if (dead_aliens == FLEET_SIZE) {
		list_destroy(&enemy_bullets);
		enemy_bullets = NULL;
		draw_player(player, true);
		player.x = LCD_WIDTH / 2 - SQU_P;
		player.y = LCD_HEIGHT - SQU_P * 2;
		draw_player(player, false);
		b.timeout = BULLET_TIMEOUT;
		setup_fleet();
		bullets_shot = 0;
		b.x = 0; b.y = 0;
	}
	
	return 0;
}

/* Starts the game (enters the game loop). */
static void start_game()
{
	int i;

    enemy_aggro = 2;    // for every 2 bullet a player shoots, aliens attack
	enemy_trigger_cnt = 1;	// how many shots
	setup_text();
	dead_aliens = 0;
    
	/* Put the player. */
    player.x = LCD_WIDTH / 2 - SQU_P;
	player.y = LCD_HEIGHT - SQU_P * 2;
    draw_player(player, false);
	b.timeout = BULLET_TIMEOUT;
	/* Put the fleet. */
    setup_fleet();
    
	/* Bullet is ready. */
	b.x = 0; b.y = 0;
	
	/* Put the asteroids. */
	make_asteroids();

	/* Wipe the screen and the initial stuff. */
	fillScreen(BG_COLOR);
    draw_player(player, false);
    for (i = 0; i < AST_CNT; ++i)
            draw_asteroid(ast[i],false);
      
	while (1) {	// main game loop
		if (update())
			break;
 	}
}

/* Waits for user input while displaying a message. */
static void player_start_sequence(const char *msg)
{
	/* Print score. */
	setup_text();
	fillRect(0, ALIEN_START_Y - ALIEN_H - 5, LCD_WIDTH, 
        LCD_HEIGHT, BG_COLOR); 
	setCursor(1, GAME_TEXT_Y);
    sprintf(buf, "You: %d    HS: %d", score, highscore);
    print(buf);
	/* Print the text. */
    setCursor(5, AST_Y);
    sprintf(buf, msg);
    print(buf);
	/* Wait for player to press the button. */
	while ((PIND & (1 << PD6))) ;
}

int main( void )
{
	/* PD7 is the led (Output) and PD6 is the button (input). */
	DDRD |= (1 << PD7);
    DDRD &= ~(1 << PD6);
	PORTD |= (1 << PD6);
	
    /* initialise host app, pins, watchdog, etc */
    init_system();

    /* configure timer ISR to fire regularly */
    init_timer_isr();

    /* Initialize Touch sensors */
    touch_init();
	
	/* Setup the text. */
	setup_text();
	
	MCUCR &= ~(1u << PUD);
	PORTD |= (1 << PD6);   

    /* LCD_init */
    init();
	myDelay(500);
	
	/* Start the initial game sequence. */
	player_start_sequence("Welcome! Press fire to start.");
	
	/* Enter the game loop. */
	while(1) {
		start_game();
		highscore = (highscore < score) ? score : highscore;
		player_start_sequence("You LOSE! Press fire to try again.");
		score = 0;
	}
    
    return 0;
}

