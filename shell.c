#include "shell.h"
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>


/* Struct Method Implmentation */
com * create_com(){
	com * comstruct = malloc(sizeof(com));
	comstruct->next = NULL;
	comstruct->comargs = create_linklist();
	comstruct->index = -1;
	return comstruct;
}

linklist * create_linklist(){
	linklist * list = malloc(sizeof(linklist));
	list->head = NULL;
	list->tail = NULL;
	return list;
}

void linklist_insert(linklist * list , char * data){
	node * myNode = malloc(sizeof(node));
	myNode->data = data;
	myNode->next = NULL;

	if(list->head == NULL){ //If list is empty, put head and tail as new node.
		list->head = myNode;
		list->tail = myNode;
	}
	else{
		list->tail->next = myNode;
		list->tail = myNode;
	}
}

void linklist_remove(linklist * list , char * data){
	node * tmp = list->head;

	if(list->head->data == data){
		if(list->head->next == NULL){
			linklist_delete(list);
			return;
		}else{
			list->head = list->head->next;
			free(tmp);
		}

	}
	while(tmp->next != NULL){
		if(tmp->next->data == data){
			node * deltmp = tmp->next->data;
			tmp->next = tmp->next->next;
			free(deltmp);
			return;

		}
		tmp = tmp->next;
	}

	printf("\t Error: Value not found in list\n");
	return;

}

void linklist_delete(linklist * list){
	node * tmp = list->head;
	node * nexttmp;
	while(tmp->next!=NULL){
		nexttmp = tmp->next;
		free(tmp);
		tmp = nexttmp;
	}

	free(list);
}

/*Initalizes shell variables */

void shell_init(){
	alias_loop = 0;
	return;
}

/* changes directory when CD+filepath command recieved */
void changedir(char * directory){
	if(chdir(directory) == -1){
		printf("\t %s is not a directory\n" , directory);
	}

	char * pwd;
	pwd = get_current_dir_name();
	setenv("PWD" , pwd , 1);
	return;
}


/* adds an alias to the aliastable */
int addalias(char * alias_name , char * alias_command){
	int i = 0;
	while(aliastable[i][0] != NULL){
		if(strcmp(alias_name , aliastable[i][0]) == 0){
			printf("\t Error: That name is already assigned\n");
			return 1;
		}
		if(strcmp(alias_command,aliastable[i][1]) == 0){
			printf("\t Error: That command is already assigned\n");
			return 1;
		}

		i++;
	}
	
	if(i >= MAX_ALIASES){
		printf("\t Error: Alias table is full.\n");
		return 1;
	}
	else{
		aliastable[i][0] = alias_name;
		aliastable[i][1] = alias_command;
	}
	return 0;
}

int unalias(char * alias_name){
	int i = 0;
	int found = 0;
	
	while(aliastable[i][0]!=NULL){
		if(strcmp(alias_name , aliastable[i][0]) == 0){
			aliastable[i][0] = 0;
			aliastable[i][1] = 0;
			i++;
			found = 1;
			break;
		}
		printf("%s != %s\n" , alias_name , aliastable[i][0]);
		i++;
	}
	while(aliastable[i][0]!=NULL){
		aliastable[i-1][0] = aliastable[i][0];
		aliastable[i-1][1] = aliastable[i][1];		
		if(aliastable[i+1][0] == 0){	
			aliastable[i][0] = 0;
			aliastable[i][1] = 0;
			return 0;
		}
		i++;
	}

	if(found == 0){
		printf("\t Error: %s alias not found.\n" , alias_name);
		return 1;
	}
		
}

/* do_it() runs built in commands */
void do_it(){
	switch(command){

		int i = 0;
		case CDX :  // CD with a directory specified.
			changedir(cd_filepath);
			break;
		case CDH: 	// CD with no directory specified.
			chdir(HOME);
			setenv("PWD" , getenv("HOME") , 1);
			break;
		case SETENV:
			if(setenv( envvar , envvar_value, 1) == -1){
				printf("\t Error: Unable to set %s as %s\n" , envvar , envvar_value);
			}
			break;
		case LISTALIAS:

			while(aliastable[i][0]){
				printf("%s\t", aliastable[i][0]);
				printf("%s\n", aliastable[i++][1]);
			}
			break;
		case HELLOFRIEND:  //Using to make sure shell responds on startup.Can remove later.
			printf("\t Hello back!\n");
			break;
		case ADDALIAS:
			addalias(alias_name , alias_command);
			break;
		case UNALIAS:
			unalias(alias_name);
			break;
		
	}

	return;
}

int executable(char * filename , char * filepath){
	struct dirent * diren;
	DIR * directory_stream;

	directory_stream = opendir(filepath);

	if(directory_stream != 0){
		diren = readdir(directory_stream);
		while(diren){
			if(strcmp(diren->d_name, filename) == 0){
				return 1;
			}
		}
	}

	return 0;
}

int determinePlace(com * comargs){
	if(comargs->next == NULL && comargs->index != 0){
		return ONLYONE;
	}

	if(comargs->next == NULL && comargs->index == 0){
		return LAST;
	}
	if(comargs->next != NULL && comargs->index != 0){
		return MIDDLE;
	}
	if(comargs->next != NULL && comargs->index == 0){
		return FIRST;
	}
}

void in_redir(char * infile){
	int fd = open(infile, O_RDONLY);
	if(fd == -1){
		printf("\tError Opening file in in_redir()\n");
		exit(1);
	}
	close(STDIN);
	dup(fd);
	close(fd);
	return;
}

void out_redir(char * outfile , int appending){
	int fd;
	if(appending){
		fd = open(outfile , O_WRONLY | O_CREAT | O_APPEND );
	}
	else{
		fd = open(outfile , O_WRONLY| O_CREAT );
	}

	if(fd == -1){
		printf("\t Error opening file in out_redir()\n");
		exit(1);
	}
	close(STDOUT);
	dup(fd);
	close(fd);
	return;
}

void execute(){

}





/* Prompts shell input each line */
void prompt(){
	char *curdir = get_current_dir_name(); //current working directory
	printf("SHELL:%s$ " , curdir);
	return;
}
void init_scanner_and_parser(){
	unaliasing = 0;
	alias_caught = 0;
	expanding = 0;
	builtin = 0;
	command = 0;
	CMD = 0;
	return;
}

int getCommand(){
	
	init_scanner_and_parser();

	yyparse();

	while(alias_caught == 1){
		if(alias_loop == 0){
			start_alias = alias_name;
			alias_loop = 1;
		}
		if(strcmp(alias_command , start_alias)== 0){
			printf("\t Error: Infinite alias loop detected. Aborting...\n");
			break;
		}else{
			reflex(alias_command);
		}
	}

	alias_loop = 0;
	return 0;
}


int main(){
	/* initilization */
	shell_init();   // What is the point in this...?

	/* Shell Loop */
	while(1){
		prompt();
		getCommand();
		switch(CMD){
			case OK: 
				if(builtin){
					do_it();
				}
				else{
					//execute(current_command);
					//current_command = NULL;
				};
				
				break;
			case EXIT:
				printf("\t Exiting...\n");
				exit(0);
			case SYSERR:
				printf("\t Nonvalid command\n");
				break;
				//	recover();
		}

	}
	return 0;
}