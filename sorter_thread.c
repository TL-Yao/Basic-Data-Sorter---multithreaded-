#include "sorter_thread.h"

int count_pc = 1;
pthread_t tid[255];
int tid_index = -1;
pthread_mutex_t lock1;
pthread_mutex_t lock2;

char* path_contact(const char* str1,const char* str2){ 
    char* result;  
    result=(char*)malloc(strlen(str1)+strlen(str2)+ 3);
    if(!result){
        printf("fail to allocate memory space\n");  
        exit(1);  
    }  

	strcpy(result,str1);
    strcat(result,"/");   
	strcat(result,str2);  
    return result;  
}  

char *trim(char *word, int index)
{
  char *end;
  while(isspace((unsigned char)*word)){
	word++;
  }
  if(*word == 0)
    return word;
  end = word + index;
  while(end > word && isspace((unsigned char)*end)) {
	  end--;
  }
  *(end+1) = 0;
  return word;
}


char** tokenizer(char* line, size_t num_col){
    
    int i, j, k;
    i = 0;//current position in line;
    j = 0;//current position in tmp;
    k = 0;//current position in result;
    
    char** result = (char**)malloc(sizeof(char*) * (num_col + 1)); //return value;
    char* temp = (char*)malloc(500);//store each word;
	size_t start_quote = FALSE;//1 quote start, 0 quote end;
	
    //go through each character;
    while(i < strlen(line)){
		
		/*reach the start '"' */
		if(line[i] == '"' && start_quote == FALSE){
			start_quote = TRUE;
		}
				
		else if(line[i] == '"' && start_quote == TRUE){
			//store value in result
			result[k] = (char*) malloc((j + 1) * sizeof(char));
			temp = trim(temp, j - 1);
			strcpy(result[k], temp);
			memset(&temp[0], 0, strlen(temp));
			start_quote = FALSE;
			j = 0;
			k++;
			i++;
		}

		//split by ',' or reach the end of line;
        else if((line[i] == ',' || i == strlen(line) - 1) && start_quote != TRUE){
            //if there is no character; (eg: ,,)
            if(!temp){
                temp[0] = '\0';
			}
			if(i == strlen(line) - 1 && line[i] != '\n' && line[i] != ','){
				temp[j] = line[i];
				j++;
			}
            //store value to result;
			result[k] = (char*)malloc((j+1) * sizeof(char));
			temp = trim(temp, j - 1);			
			strcpy(result[k], temp);
			memset(&temp[0], 0, strlen(temp));			
            j = 0;
			k++;
			//if the last character is ',';
			if(line[i] == ',' && i == strlen(line) - 1){
				temp[0] = '\0';
				result[k] = (char*)malloc((j+1) * sizeof(char));
				strcpy(result[k], temp);
				memset(&temp[0], 0, strlen(temp));								
			}

        }else{
			//copy character from line to temp;
			if(j == 0){
				if(line[i] == ' '){
					i++;
					continue;
				}				
			}
            temp[j] = line[i];
			j++;
		}
        i++;
	}
	i = 0;
    return result;
}

int tok_file(FILE *fp, row* data, int num_col){
	row rest_row;
	rest_row.row_text = (char*) malloc (sizeof(char) * BUF_SIZE);
	size_t curr_col = 0;
	size_t curr_row = 0; //current row number in row* data
	int i;//loop virable
	/*end of declaring variable*/
	
	/*deal with data*/
	i = 0;
	while(fgets(rest_row.row_text, BUF_SIZE-1, fp) != NULL){
		rest_row.row_len = strlen(rest_row.row_text);
		rest_row.row_token = (char**) malloc(sizeof(char *) * (num_col+1));
		rest_row.row_token = tokenizer(rest_row.row_text, num_col);
		data[curr_row++] = rest_row;		
	}
	return curr_row;
}

void sort(void* arg){
	/*open the parameter package*/
		struct sort_para* para;
		para = (struct sort_para*) arg;
		char* colname = para -> colname;
		char* tmppath = para -> tmppath;

		/*declare variable*/
		FILE *fp;
		fp = fopen(tmppath,"r");
		
		//first row:
		row first_row;
		row *data;
		char *token;
		char* target;
		size_t num_col = 1;
		int length;
		int i;	
		int j;
		int k;
		int num_row; 
		int target_col;
	
		/*split the 1st token by ','*/
		first_row.row_text = (char*) malloc (sizeof(char) * BUF_SIZE);		
		fgets(first_row.row_text, BUF_SIZE-1, fp);
		first_row.row_len = strlen(first_row.row_text);
		first_row.row_token = (char**) malloc(sizeof(char *) * first_row.row_len);
		token = strtok(first_row.row_text, ",");
		first_row.row_token[0] = token;
	
		//split the rest of token in the first row
		while(token = strtok(NULL, ",")){
			first_row.row_token[num_col++] = token;	
		}
		first_row.num_col = num_col;
		
		//delete the '\n' in the last word;
		
		length = strlen(first_row.row_token[num_col - 1]);
		i = 1;
		while(first_row.row_token[num_col - 1][length - i] <= 13 && first_row.row_token[num_col - 1][length - i] >= 7){
			first_row.row_token[num_col - 1][length - i] = '\0';
			i++;
		}
        
		//trim blank space;
		i = 0;
		while(i < num_col){
			first_row.row_token[i] = trim(first_row.row_token[i], strlen(first_row.row_token[i]) - 1);
			i++;
		}
		
		//deal with rest rows;
		data = (row*) malloc (sizeof(row) * MAX_LINE);
		num_row = tok_file(fp, data, num_col);

		
		//find the target column number;
			target = colname;
			target_col = 0;
			
			while(target_col < first_row.num_col){
				if(strcmp(first_row.row_token[target_col], target) == 0){
					break;
				}
				target_col++;
			}
			
			//no such title in the first row
			if(target_col == (first_row.num_col)){
				printf("Wrong input, no such title.\n");
				fflush(stdout); 
				exit(1);
			}
			mergeSort(data, target_col, num_row);

}

int isDirectory(char *path) {
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
        return 0;
    return S_ISDIR(statbuf.st_mode);
 }

int checkcsv(char* path, char* colname){
	FILE *fptr;
	fptr = fopen(path, "r");

	//get the first line of this file;
	char* text = (char*)malloc(500);
	fgets(text, BUF_SIZE-1, fptr);
	
	char* token = malloc(50);
	char** row_token = (char**) malloc(sizeof(char *) * (strlen(text)));
	

	token = strtok(text, ",");
	row_token[0] = token;

	int num_col = 1;
	while(token = strtok(NULL, ",")){
		row_token[num_col++] = token;	
	}

    int length = strlen(row_token[num_col - 1]);
	int i = 1;

	while(row_token[num_col - 1][length - i] <= 13 && row_token[num_col - 1][length - i] >= 7){
		row_token[num_col - 1][length - i] = '\0';
		i++;
	}

	//find the target column;
	int target_col = 0;
	while(target_col < num_col){
		if(strcmp(row_token[target_col], colname) == 0){
			break;
		}
		target_col++;
	}

	//no such title, not the target csv file;
	if(target_col == num_col){
		return 0;
	}
	return 1; 
	
}
void directory(void* arg){

    struct sort_para* para;
	para = (struct sort_para*) arg;
	char* colname = para -> colname;
	char* tmppath = para -> tmppath;

    DIR *dir_p;
    struct dirent *dir_ptr;
    dir_p = opendir(tmppath);
    FILE *result;
    int err = 0;

    if(dir_p == NULL){
		printf("Wrong Path\n");
		fflush(stdout); 
        exit(1);
    }
    
    
    // loop each file and folder in current directory
    while(dir_ptr = readdir(dir_p)){
        char* temppath;
        temppath = path_contact(tmppath, dir_ptr->d_name);
        struct stat st;
        stat(temppath, &st);
        
        /*skip forward and back folder*/
        if(!strcmp(dir_ptr->d_name, ".")  ||
		   !strcmp(dir_ptr->d_name, "..") ||
			dir_ptr->d_name[0] == '.'){//change
	            continue;
        }
        
        if(isDirectory(temppath)){

            pthread_mutex_lock(&lock1);
            tid_index++;
            para -> tmppath = temppath;
            err = pthread_create(&tid[tid_index], NULL, (void *)&directory, (void*)para);
            if(err != 0){
                printf("Failed to create new thread.\n");
            }
            pthread_mutex_unlock(&lock1);

        }
        else{ // file
            char *name = dir_ptr->d_name;
            int length = strlen(name);
            
            /* .csv file*/
            if(name[length - 3] == 'c' &&
               name[length - 2] == 's' &&
               name[length - 1] == 'v'){

                if(checkcsv(temppath, colname)){

                    pthread_mutex_lock(&lock2);
					tid_index++;
                    para -> tmppath = temppath;
                    err = pthread_create(&tid[tid_index], NULL, &sort, (void*)para);
                    if(err != 0){
                        printf("Failed to create new thread.\n");
                    }
                    pthread_mutex_unlock(&lock2);

				}   	
            }
        }
        
    }

}
int main (int argc, char* argv[]){

	//declare variables;
    char* colname = (char*)malloc(100);
	char* dirname = (char*)malloc(100);
	char* odirname = (char*)malloc(100);
    char currDir[MAX_DIR];
    getcwd(currDir, MAX_DIR);
    
	if(argc < 2){
		printf("Too few input.\n");
		exit(0);
	}
	int i = 1;
	while(argv[i]){
		if(i > 6){
			printf("Too many input.\n");
			exit(0);
		}
		if(!strcmp(argv[i], "-c")){
			colname = strcpy(colname, argv[i+1]);
		}
		if(!strcmp(argv[i], "-d")){
			dirname = strcpy(dirname, argv[i+1]);
		}
		if(!strcmp(argv[i], "-o")){
			odirname = strcpy(odirname, argv[i+1]);
		}
		i += 2;
	}
	if(!colname){
		printf("Wrong input, column name missed.\n");
	}
    if(pthread_mutex_init(&lock1, NULL) != 0){
        printf("Error on lock1.\n");
    }
     if(pthread_mutex_init(&lock2, NULL) != 0){
        printf("Error on lock2.\n");
    }
    
    i = 0;
    while(tid[i]){
        pthread_join(tid[i], NULL);
        i++;
    }
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);
	return 0;
}
