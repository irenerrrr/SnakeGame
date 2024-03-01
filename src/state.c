#include "state.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_state_t* state, unsigned int snum);
static char next_square(game_state_t* state, unsigned int snum);
static void update_tail(game_state_t* state, unsigned int snum);
static void update_head(game_state_t* state, unsigned int snum);

/* Task 1 */
game_state_t* create_default_state() {
  // TODO: Implement this function.
  //
  game_state_t *state = malloc(sizeof(game_state_t));
  state -> num_rows = 18;
  unsigned int num_cols = 20;
  state -> board = malloc(state -> num_rows * sizeof(char*));

  for (unsigned int i = 0; i < state -> num_rows; i++) {
    state -> board[i] = malloc ((num_cols + 1)* sizeof(char));
    for (unsigned int j = 0; j < num_cols; j++) {
      if (i == 0 || i == state -> num_rows - 1 || j == 0 || j == num_cols - 1) {
        state -> board[i][j] = '#';
      } else {
        state -> board[i][j] = ' ';
      }
    }
    state -> board[i][num_cols] = '\0';
    
  }

  state -> num_snakes = 1;
  state -> snakes = malloc(state -> num_snakes * sizeof(snake_t));

  state -> snakes[0].head_row = 2;
  state -> snakes[0].head_col = 4;
  state -> snakes[0].tail_row = 2;
  state -> snakes[0].tail_col = 2;
  state -> snakes[0].live = true;

  
  state -> board[2][9] = '*';    //fruit
  
  state -> board[2][4] = 'D';
  state -> board[2][3] = '>';
  state -> board[2][2] = 'd';

  return state;
}


/* Task 2 */
void free_state(game_state_t* state) {

  // TODO: Implement this function.
  if (state == NULL) {
    return;
  }

  for (unsigned int i = 0; i < state -> num_rows; i++) {
    if (state -> board[i] != NULL) {
      free(state -> board[i]);
    }
  }
  
  free(state -> board);
  if (state -> snakes != NULL ) {   
    free(state -> snakes);
  }
  free(state);

  return;
}

/* Task 3 */
void print_board(game_state_t *state, FILE *fp) {
    // TODO: Implement this function.
    for (unsigned int i = 0; i < state->num_rows; i++) {
        fprintf(fp, "%s\n", state->board[i]);
    }
}

/*
  Saves the current state into filename. Does not modify the state object.
  (already implemented for you).
*/
void save_board(game_state_t* state, char* filename) {
  FILE* f = fopen(filename, "w");
  print_board(state, f);
  fclose(f);
}

/* Task 4.1 */


/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_state_t* state, unsigned int row, unsigned int col) {
  return state->board[row][col];
}

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_state_t* state, unsigned int row, unsigned int col, char ch) {
  state->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  
  // TODO: Implement this function.

  return c == 'w' || c == 'a' || c == 's' || c == 'd';
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  
  // TODO: Implement this function.
  return c == 'W' || c == 'A' || c == 'S' || c == 'D' || c == 'x';
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  
  // TODO: Implement this function.
  return is_tail(c) || is_head(c) || c == '^' || c == '<' || c == '>' || c == 'v';
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  
  if (c == '^') {
    return 'w';
  } else if (c == '<') {
    return 'a';
  } else if (c == 'v') {
    return 's';
  } else if (c == '>') {
    return 'd';
  } else {
    return '?';
  }

}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  
  if (c == 'W') {
    return '^';
  } else if (c == 'A') {
    return '<';
  } else if (c == 'S') {
    return 'v';
  } else if (c == 'D') {
    return '>';
  } else {
    return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  
  if (c =='v' || c == 's' || c == 'S') {
    return cur_row + 1;
  } else if (c == '^' || c == 'w' || c == 'W') {
    return cur_row - 1;
  } else {
    return cur_row;
  }

}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  
  if (c =='>' || c == 'd' || c == 'D') {
    return cur_col + 1;
  } else if (c == '<' || c == 'a' || c == 'A') {
    return cur_col - 1;
  } else {
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_state. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_state_t* state, unsigned int snum) {
  
  // TODO: Implement this function.
  snake_t curr_snake = state -> snakes[snum];
  
  char head_dir = get_board_at(state, curr_snake.head_row, curr_snake.head_col);
  unsigned int next_row = get_next_row(curr_snake.head_row, head_dir);
  unsigned int next_col = get_next_col(curr_snake.head_col, head_dir);
  
  char next_char = get_board_at(state, next_row, next_col);
  return next_char;
}

/*
  Task 4.3

  Helper function for update_state. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.  

  snake_t *curr_snake = &(state -> snakes[snum]);
  char head_dir = get_board_at(state, curr_snake -> head_row, curr_snake -> head_col);
  char original_head = head_to_body(head_dir);

  unsigned int next_row = get_next_row(curr_snake -> head_row, head_dir);
  unsigned int next_col = get_next_col(curr_snake -> head_col, head_dir);
  set_board_at(state, curr_snake -> head_row, curr_snake -> head_col, original_head);
  
  state -> board[next_row][next_col] = head_dir;
  state -> snakes[snum].head_row = next_row;
  state -> snakes[snum].head_col = next_col;

  return;


}

/*
  Task 4.4

  Helper function for update_state. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_state_t* state, unsigned int snum) {
  // TODO: Implement this function.

  snake_t *curr_snake = &(state -> snakes[snum]);
  char tail_dir = get_board_at(state, curr_snake -> tail_row, curr_snake -> tail_col);
    
  unsigned int next_row = get_next_row(curr_snake -> tail_row, tail_dir);
  unsigned int next_col = get_next_col(curr_snake -> tail_col, tail_dir);
  
  set_board_at(state, curr_snake -> tail_row, curr_snake -> tail_col, ' ');

  char new_tail = get_board_at(state, next_row, next_col);
  set_board_at(state, next_row, next_col, body_to_tail(new_tail));

  state -> snakes[snum].tail_row = next_row;
  state -> snakes[snum].tail_col = next_col;
  
  return;
}

/* Task 4.5 */
void update_state(game_state_t* state, int (*add_food)(game_state_t* state)) {
  // TODO: Implement this function.

  for (unsigned int i = 0; i < state -> num_snakes; i++) {
      if (state -> snakes[i].live) {
          snake_t *curr_snake = &(state -> snakes[i]);
          char moving_to = next_square(state, i);
          if (is_snake (moving_to) || moving_to == '#') {
              curr_snake -> live = false;
              set_board_at(state, curr_snake -> head_row, curr_snake -> head_col, 'x');
          } else if (moving_to == '*') {
              update_head(state, i);
              add_food(state);
          } else {
              update_head(state, i);
              update_tail(state, i);
          }
      }
  }

  return;
}

/* Task 5 */


game_state_t* load_board(FILE* fp){
if(fp == NULL){
    return NULL;
}
unsigned int col = 0;
game_state_t* state = calloc(sizeof(game_state_t),1);
state->board = calloc(sizeof(char*),1);
state -> num_rows = 0;
state -> board[0] =NULL;
int c;
while((c = fgetc(fp))!=EOF){
    char ch = (char)c;
    if(ch!= '\n'){
        state-> board[state->num_rows]= realloc(state->board[state->num_rows],(col+2)*sizeof(char));
        if (state->board[state->num_rows]==NULL){
            for (unsigned int i=0; i<state->num_rows;i++){
                free(state->board[i]);
            }
            free(state->board);
            free(state);
        }
        state->board[state ->num_rows][col]=ch;
        col++;
    } else{
        state->board[state->num_rows][col] = '\0';
        col = 0;
        state->num_rows+=1;
        char **temp = realloc(state->board, (state->num_rows +1) * sizeof(char *));
        state->board = temp;
        state-> board[state -> num_rows]= NULL;
    }
}
return state;
}
 

  // TODO: Implement this function.


/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
    static void find_head(game_state_t* state, unsigned int snum) {
        unsigned int x = state->snakes[snum].tail_row;
        unsigned int y = state->snakes[snum].tail_col;
        char curr = get_board_at(state,x,y);
        while(!is_head(curr)){
            x = get_next_row( x,curr);
            y  = get_next_col(y,curr);
        curr = get_board_at(state, x, y);

    }
    state->snakes[snum].head_row=x;
    state->snakes[snum].head_col=y;

}

/* Task 6.2 */

game_state_t* initialize_snakes(game_state_t* state) {
    unsigned int count = 0;
    state->snakes = malloc(sizeof(snake_t)); 
    for (unsigned int i = 0; i < state->num_rows; i++) {
        long unsigned int scan = strlen(state->board[i]);
        for (unsigned int j = 0; j < scan; j++) {
            char temp = get_board_at(state, i, j);
            if (is_tail(temp)) {
                if (count >= 1) {
                    state->snakes = realloc(state->snakes, (count + 1) * sizeof(snake_t));
                }
                snake_t *t_snake = &state->snakes[count]; 
                t_snake->live = true;  
                t_snake->tail_row = i;

                t_snake->tail_col = j;
                find_head(state, count);
                count++;
            }
        }
    }

    state->num_snakes = count;
    return state;
}

