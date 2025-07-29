#define MAX_WORD_SIZE 76
#define CELL_SIZE_MEMORY 16
#define AMOUNT_ACTION 16
#define SIZE_OPCODE 4

#define ADDRESS_3_SOURCE 4
#define ADDRESS_2_SOURCE 5
#define ADDRESS_1_SOURCE 6
#define ADDRESS_0_SOURCE 7

#define ADDRESS_3_TARGET 8
#define ADDRESS_2_TARGET 9
#define ADDRESS_1_TARGET 10
#define ADDRESS_0_TARGET 11

#define A 12
#define B 13
#define E 14

#define END_LINE_CODING 15
#define	TWO 2
#define FIVE 5
#define THIRTEEN 13
#define INDEX_THREE 11
#define INDEX_SIX 8
#define INDEX_NINE 5
#define SEVEN 7
#define FOUR 4
#define NINE 9
#define TEN 10
#define TWELVE 12
#define THIRTEEN 13
#define FOURTEEN 14
#define FIFTEEN 15

int coding_action(char word[],int paction, int countline, int ic, char binaryarr[][16]);
int chaking_operands(int amountoperand, int paction);
int Immediate_response(int numopernd, int paction, int type, int countline,char binaryarr[][16],int ic);
int chaking_action(char word[],int start,int end, int countline);
int is_integer(char c[]);
