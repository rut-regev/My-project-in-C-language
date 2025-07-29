
#define LEN_LINE 82      /* הגדרת קבוע: אורך מקסימלי לשורה (81 תווים) */
#define LEN_BITS 16    
#define LEN_LABEL 31 
#define LEN_TYPE_LABEL 10
#define TOW 2  
#define THREE 3  
#define FORE 4  
#define START_OF_MEMORY 100  
#define SIZE_OF_MEMORY 4096  
#define ASCII_ZERO 48

struct line_label
{
	int line_in_tabel;
	int line_in_file;
};
typedef struct line_label line_label;

struct entry_label
{
	char name[LEN_LABEL];
	int line_of_label;
	int line_of_file;
};
typedef struct entry_label entry_label;

struct labels
{
char namelabel[LEN_LABEL];
char typelabel[LEN_TYPE_LABEL];
int linelabel;
int line_of_file;
};
typedef struct labels labels;


int chaking_label(char line[], int start, int end, int countline);
int chaking_data(char word[], int start, int end);
int coding_action(char word[],int paction, int countline, int ic, char binaryarr[][16]);
int coding_data(char *line,char type[],int length_line);

int save_label(char name[],char type[],int line,labels * place,int countline);
int valid_label(entry_label *ent_lab,labels *label_tabel,int entryplace,int labelplace);


int second_pass(char databiaryarr[][16],char actionbinaryarr[][16],line_label *lineslabelarr,labels *label_tabel,int labelplace,int lineplace,char *name_of_file,int length_name_file,int IC,int DC,int derror);

int enter_line_entry(entry_label *ent_lab,labels *label_tabel,int entryplace,int labelplace);
int entry_file(entry_label *ent_lab,int entryplace,char *name_of_file, int length_name_file);
int update_line_data_label(labels *label_tabel,int labelplace, int IC);
int exist_label(labels *label_table,char new_label[],int labelplace,int countline);

int miss_comma(char str[],int countline);
int without_space(char str[],int length);
int good_comma(char *str,int countline) ;
int without_profits_start(char line[]);
int check_string(char *line,int size,int countline);

int compare_with_words(char arr[], char *words[],int numwords);


int enter_code_data(char arr[]);


