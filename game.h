
typedef struct game_state {
	int score1;
	int score2;
	int inning; //player 1 is the "top" half of the inning
	int batter; //either 1 or 2
	int balls;
	int strikes;
	int outs;
	int first;
	int second;
	int third;
} game_t;

typedef struct result {
	int hit; //0 no hit, 1 = single, 2 = double, 3 = triple, 4 = home run
	int out; //0 no out, 1 = ground out, 2 = pop out, 3 = fly out
	int strike; //0 no strike, 1 normal strike, 2 foul
	int ball;
}result_t;

void updateGameState(game_t * state, result_t * result);
game_t * init_game_state(void);
void make_random(game_t * game);