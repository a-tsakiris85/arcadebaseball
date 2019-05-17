/*
The structure used to keep track of the current game state.

It holds information such as the score of both players, the
current inning, which player is at bat, the numbers of outs,
the number of balls and strikes, etc.
*/
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

/*
After the batter swings, this structure is used to maintain information
about the result of the attempt (i.e. whether the batter got out, if 
they got a strike or a ball, or if the batter took a base)
*/
typedef struct result {
	int hit; //0 no hit, 1 = single, 2 = double, 3 = triple, 4 = home run
	int out; //0 no out, 1 = out
	int strike; //0 no strike, 1 normal strike, 2 foul
	int ball;
}result_t;


void updateGameState(game_t * state, result_t * result);
game_t * init_game_state(void);
int random(int high);
int inBox(int row, int col);
result_t * produce_result(int row, int col, int wasBall);
int determine_speed(int a);
void create_pitch(int sw1, int sw2);
result_t * init_result(void);


