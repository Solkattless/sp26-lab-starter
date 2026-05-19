#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
game_t *create_default_game() {
  // TODO: Implement this function.
  game_t *my_game = malloc(sizeof(game_t));
  my_game->num_rows = 18;
  my_game->num_snakes = 1;
  my_game->board = malloc(sizeof(char *) * my_game->num_rows);
  for(int i = 0; i < my_game->num_rows; i++){
    my_game->board[i] = malloc(sizeof(char) * 22);
  }
  for(int i = 0; i < my_game->num_rows; i++){
    if(i == 0 || i == my_game->num_rows - 1){
      strcpy(my_game->board[i], "####################\n");
    }
    else{
      strcpy(my_game->board[i], "#                  #\n");
    }
  }
  my_game->board[2][9] = '*';
  my_game->snakes = malloc(sizeof(snake_t) * my_game->num_snakes);
  my_game->snakes[0].tail_row = 2;
  my_game->snakes[0].tail_col = 2;
  my_game->snakes[0].head_row = 2;
  my_game->snakes[0].head_col = 4;
  my_game->snakes[0].live = true;
  my_game->board[2][2] = 'd';
  my_game->board[2][4] = 'D';
  my_game->board[2][3] = '>';
  return my_game;
}

/* Task 2 */
void free_game(game_t *game) {
  // TODO: Implement this function.
  for(int i = 0; i < game->num_rows; i++){
    free(game->board[i]);
  }
  free(game->board);
  free(game->snakes);
  free(game);
  return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  // TODO: Implement this function.
  for(int i = 0; i < game->num_rows; i++){
    fprintf(fp, "%s", game->board[i]);
  }
  return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  return(c == 'w' || c == 'a' || c == 's' || c == 'd');
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  return(c == 'W' || c == 'A' || c == 'S' || c =='D');
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  return(is_tail(c) || is_head(c) || c == '^' || c == '<' || c == 'v' || c == '>');
}

/*
  Converts a character in the snake's body ("^<v>")
  to the matching character representing the snake's
  tail ("wasd").
*/
static char body_to_tail(char c) {
  // TODO: Implement this function.
  switch(c){
    case '^': return 'w';
    case '<': return 'a';
    case 'v': return 's';
    case '>': return 'd';
    default: return '?';
  }
}

/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  switch(c){
    case 'W': return '^';
    case 'A': return '<';
    case 'S': return 'v';
    case 'D': return '>';
    default: return '?';
  }
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if(c == 'v' || c == 's' || c == 'S'){
    return cur_row + 1;
  }
  else if(c == '^' || c == 'w' || c == 'W'){
    return cur_row - 1;
  }
  else{
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
  if(c == '>' || c == 'd' || c == 'D'){
    return cur_col + 1;
  }
  else if(c == '<' || c == 'a' || c == 'A'){
    return cur_col - 1;
  }
  else{
    return cur_col;
  }
}

/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int r = game->snakes[snum].head_row;
  unsigned int c = game->snakes[snum].head_col;

  char headchar = game->board[r][c];
  unsigned int next_r = get_next_row(r, headchar);
  unsigned int next_c = get_next_col(c, headchar);
  char next_char = game->board[next_r][next_c];
  return next_char;
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int r = game->snakes[snum].head_row;
  unsigned int c = game->snakes[snum].head_col;

  char headchar = game->board[r][c];
  unsigned int next_r = get_next_row(r, headchar);
  unsigned int next_c = get_next_col(c, headchar);
  game->snakes[snum].head_row = next_r;
  game->snakes[snum].head_col = next_c;
  game->board[r][c] = head_to_body(headchar);
  game->board[next_r][next_c] = headchar;
  return;
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/
static void update_tail(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int r = game->snakes[snum].tail_row;
  unsigned int c = game->snakes[snum].tail_col;

  char tailchar = game->board[r][c];
  game->board[r][c] = ' ';
  unsigned int next_r = get_next_row(r, tailchar);
  unsigned int next_c = get_next_col(c, tailchar);
  game->snakes[snum].tail_row = next_r;
  game->snakes[snum].tail_col = next_c;
  tailchar = body_to_tail(game->board[next_r][next_c]);
  game->board[next_r][next_c] = tailchar;
  return;
}

/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  // TODO: Implement this function.
  for(unsigned int i = 0; i < game->num_snakes; i++){
    if(!game->snakes[i].live){
      continue;
    }
    if(next_square(game, i) != ' ' && next_square(game, i) != '*'){
      unsigned int r = game->snakes[i].head_row;
      unsigned int c = game->snakes[i].head_col;
      game->board[r][c] = 'x';
      game->snakes[i].live = false;
    }
    else if(next_square(game, i) == '*'){
      update_head(game, i);
      add_food(game);
    }
    else{
      update_head(game, i);
      update_tail(game, i);
    }
  }
  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  size_t length = 0;
  size_t buff_size = 10;

  char *buff = (char *) malloc(sizeof(char) * buff_size);
  if (buff == NULL){
    free(buff);
    return NULL;
  }

  while(fgets(buff + length, (int)(buff_size - length), fp)){
    if(strchr(buff + length, '\n')){
      return buff;
    }

  length += strlen(buff + length);
  buff_size *= 2;
  buff = realloc(buff, buff_size);
  }

  if(length == 0){
    free(buff);
  }
  return NULL;
}

/* Task 5.2 */
game_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  game_t *game = (game_t *) malloc(sizeof(game_t));
  game->num_rows = 0;
  game->num_snakes = 0;
  game->board = NULL;
  game->snakes = NULL;

  char *line = NULL;
  while((line = read_line(fp))){
    char **temp = (char **) realloc(game->board, (game->num_rows + 1) * sizeof(char *));
    
    if (temp == NULL){
      for(int i = 0; i < game->num_rows; i++){
        free(temp[i]);
      }
      free(game->board);
      free(game);
    }

    temp[game->num_rows] = line;
    game->board = temp;
    (game->num_rows)++;
  }
  return game;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  unsigned int r = game->snakes[snum].tail_row;
  unsigned int c = game->snakes[snum].tail_col;
  char target = game->board[r][c];
  while(target != 'D' && target != 'W' && target != 'A' && target != 'S'){
    unsigned int next_r = get_next_row(r, target);
    unsigned int next_c = get_next_col(c, target);
    target = game->board[next_r][next_c];
    r = next_r;
    c = next_c;
  }
  game->snakes[snum].head_row = r;
  game->snakes[snum].head_col = c;
  return;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {
  // TODO: Implement this function.
  game->num_snakes = 0;
  for(unsigned int i = 0; i < game->num_rows; i++){
    unsigned int j = 0;
    while(game->board[i][j] != '\n'){
      char c = game->board[i][j];
      if(is_tail(c)){
        game->num_snakes++;}
      j++;
    }
  }

  unsigned int snum = 0;
  game->snakes = malloc(sizeof(snake_t) * game->num_snakes);
  for(unsigned int i = 0; i < game->num_rows; i++){
    unsigned int j = 0;
    while(game->board[i][j] != '\n'){
      char c = game->board[i][j];
      if(is_tail(c)){
        game->snakes[snum].live = 1;
        game->snakes[snum].tail_row = i;
        game->snakes[snum].tail_col = j;
        find_head(game, snum);
        snum++;
      }
      j++;
    }
  }
  return game;
}
