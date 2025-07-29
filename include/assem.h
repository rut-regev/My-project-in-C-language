#define MAX_LEN_MACRO 31
#define LEN_LINE 82
#define TOW 2
struct macro_tabel{
char name[MAX_LEN_MACRO];
char *commands;
struct macro_tabel *next;
};
typedef struct macro_tabel macro_tabel;


int pro_assembler(FILE *ifp,FILE *ofp,char *argv, int length_name_file);
int first_pass(FILE *ifp,char *name_of_file,macro_tabel *macro_name,int error, int length_name_file);
int good_name(char *name_macro,macro_tabel **head,int countline);

int name_of_macro(char *pointer,macro_tabel **head,macro_tabel **p);
int enter_macro(char *name_macro,macro_tabel **head,macro_tabel **curr);
int free_macro(macro_tabel **head);


int is_macro(char name_label[],macro_tabel *p);/*ההגדרה של הפונקציה נמצאת כאן כיון שהיא משתמשת בטבלת המאקרואים*/

