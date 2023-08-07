/*
 * space_invaders.h
 *
 *  Created on: Aug 5, 2023
 *      Author: kaustubh
 */

#ifndef APPLICATION_INC_SPACE_INVADERS_H_
#define APPLICATION_INC_SPACE_INVADERS_H_

// ********** Capture image dimensions out of BMP **********
#define PLAYERW     ((uint8_t) player_ship[18])
#define PLAYERH     ((uint8_t) player_ship[22])
#define LASERW      ((uint8_t) laser[18])
#define LASERH      ((uint8_t) laser[22])
#define BUNKERW     ((uint8_t) bunker_light[18])
#define BUNKERH     ((uint8_t) bunker_light[22])
#define ENEMYW    	((uint8_t) enemy_ship[18])
#define ENEMYH    	((uint8_t) enemy_ship[22])

// ********** Number of enemies **********
#define NUMBER_OF_ENEMIES		3

// ********** Max. number of missiles **********
#define USER_MISSILE_MAX 		6
#define ENEMY_MISSILE_MAX		6

// ********** Points obtained per kill **********
#define POINTS_PER_KILL			50

// ********** Image persist period **********
#define PERSIST_PERIOD			3000			// 3 secs

// ********** Laser generation period of the enemy **********
#define ENEMY_LASER_GENERATION_PERIOD	1000	// 1 secs

// ********** Laser movement period **********
#define LASER_MOVEMENT_PERIOD			500		// 0.5 secs

// ********** Laser displacement per iteration **********
#define USER_LASER_DISPLACEMENT			3		// 3 pixels
#define ENEMY_LASER_DISPLACEMENT		5		// 5 pixels

// ********** Screen refresh period **********
#define SCREEN_REFRESH_PERIOD			16		// approximately 60Hz refresh rate

typedef struct sprite_info {
	uint8_t x;      					// x-coordinate
	uint8_t y;      					// y-coordinate
	const uint8_t *image; 				// associated sprite image
	uint8_t life;         				// 0=dead, 1=alive
} sprite_info_t;

typedef struct laser_info {
	struct sprite_info laser_sprite;     // laser related status
	int8_t hit_status;					 // to check if missile is destined to hit an enemy or not
} laser_info_t;

typedef struct game_objects {
	struct sprite_info enemy[3];
	struct sprite_info user;
	struct sprite_info bunker;
	struct laser_info enemy_laser[ENEMY_MISSILE_MAX];
	struct laser_info user_laser[USER_MISSILE_MAX];
} game_objects_t;

typedef struct game_stats {
	uint8_t kill_count;
	uint8_t user_missile_count;
	uint8_t enemy_missile_count;
	uint8_t game_over_flag;
} game_stats_t;

#endif /* APPLICATION_INC_SPACE_INVADERS_H_ */
