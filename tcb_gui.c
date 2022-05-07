#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <json-c/json.h>
#include <limits.h>
#include "tcb_gui.h"

#define MAX_ATTIBUTE_LEN 100
#define MAX_JSON_LEN 65535

static char loginUser[MAX_ATTIBUTE_LEN] = "";
static int yMax, xMax;
static int stackCount = 0;
static int firUserIndex = 0;
static int firItemIndex = 0;

static void freeUserList(struct userNode *head) {
	struct userNode *tmp;
	while(head != NULL) {
		tmp = head;
		head = head->next;
		free(tmp->name);
		free(tmp->createTime);
		free(tmp->modTime);
		free(tmp->remark);
		free(tmp);
	}
	return;
}

static void freePasswordList(struct passwordNode *head) {
	struct passwordNode *tmp;
	while(head != NULL) {
		tmp = head;
		head = head->next;
		free(tmp->password);
		free(tmp);
	}
	return;
}

static void show_main_page_options() {
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_CYAN);

	getmaxyx(stdscr, yMax, xMax);

	//welcome title
	if(strcmp(loginUser, "") != 0) { // if has user as parameter
		mvaddstr(yMax/2-3, xMax/2-strlen(loginUser)/2, loginUser);	
	}
	mvaddstr(yMax/2-1, xMax/2-strlen("Welcome to be Here !!")/2, "Welcome to be Here !!");
	mvaddstr(yMax/2+1, xMax/2-strlen("You can use following key options to browse your code book.")/2, "You can use following key options to browse your code book.");

	// set key options
	mvaddstr(yMax-1, 0, "h:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 2, "Help");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 6, "m:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 8, "Home ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 13, "u:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 15, "Users ");
	attroff(COLOR_PAIR(1));

	if(strcmp(loginUser, "") != 0) { // if has user as parameter
		mvaddstr(yMax-1, 21, "i:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 23, "Items ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 29, "b:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 31, "Back ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 36, "q:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 38, "Quit ");
		attroff(COLOR_PAIR(1));
		use_default_colors();
	} else {
		mvaddstr(yMax-1, 21, "b:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 23, "Back ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 28, "q:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 30, "Quit ");
		attroff(COLOR_PAIR(1));
		use_default_colors();
	}
}

static void show_user_page_options() {
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_CYAN);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_color(COLOR_GREEN, 0, 700, 0);
	init_pair(3, COLOR_GREEN, -1);
	init_pair(4, COLOR_YELLOW, -1);
	init_pair(5, COLOR_MAGENTA, -1);
	init_pair(6, 8, -1);
	init_pair(7, COLOR_RED, -1); // bright red
	init_pair(8, COLOR_CYAN, -1);

	getmaxyx(stdscr, yMax, xMax);

	int userCount = 0;
	struct userNode *userList = NULL; // to keep all users
	struct userNode *rear;

	DIR *d;
    struct dirent *dir;
    d = opendir("./users");

    if(d) { 
    	// build user's info list
        while((dir = readdir(d)) != NULL) {
            if(dir->d_name[0] != '.') {
            	struct userNode *newUser = malloc(sizeof(struct userNode));
            	newUser->next = NULL;
            	newUser->userID = userCount;
            	newUser->name = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
            	newUser->name = strcpy(newUser->name, dir->d_name);

            	char *path = malloc(strlen("./users/") + strlen(newUser->name) + 1);
            	strcpy(path, "./users/");
                strcat(path, newUser->name);

                FILE *fp;
                fp = fopen(path, "r");
                free(path);

                char *jsonStr = malloc(MAX_JSON_LEN*sizeof(char));
                memset(jsonStr, 0, MAX_JSON_LEN);
                fgets(jsonStr, MAX_JSON_LEN, fp);
                fclose(fp);
                struct json_object *root = json_tokener_parse(jsonStr);
                free(jsonStr);

                newUser->createTime = strdup(json_object_to_json_string(json_object_object_get(root, "createTime")));
                if(strtok(newUser->createTime, "\"") == NULL) strcpy(newUser->createTime, "null");

                newUser->modTime = strdup(json_object_to_json_string(json_object_object_get(root, "modTime")));
                if(strtok(newUser->modTime, "\"") == NULL) strcpy(newUser->modTime, "null");

                newUser->remark = strdup(json_object_to_json_string(json_object_object_get(root, "remark")));
                if(strtok(newUser->remark, "\"") == NULL) strcpy(newUser->remark, "null");

            	newUser->next = NULL;

            	if(userList == NULL) { // if first node
            		rear = newUser;
            		userList = newUser;
            	} else {
            		rear->next = newUser;
            		rear = newUser;
            	}
            	userCount++;
            }
        }
        closedir(d);
    } else {
        mvaddstr(1, 3, "no user");
    }

    // statistical bar
    attron(COLOR_PAIR(8));
    mvaddstr(1, 3, "Total users: ");
    attroff(COLOR_PAIR(8));
	char buf[1024];
    sprintf(buf, "%d", userCount);
	mvaddstr(1, 16, buf);

    // info bar
    attron(COLOR_PAIR(1));
    for(int i=0; i<xMax; i++) {
    	mvaddstr(4, i, " ");
    }
    mvaddstr(4, 3, "UID");
    mvaddstr(4, 10, "User");
    mvaddstr(4, 20, "Created Time");
    mvaddstr(4, 50, "Modified Time");
    mvaddstr(4, 80, "Remark");
    attroff(COLOR_PAIR(1));

    // to build user info array
    int uid[userCount];
    char *userName[userCount];
    char *userCreateTime[userCount];
    char *userModTime[userCount];
    char *userRemark[userCount];
    struct userNode *index = userList;
    for(int i=0; i<userCount; i++) {
	    uid[i] = index->userID;
    	userName[i] = strdup(index->name);
    	userCreateTime[i] = strdup(index->createTime);
    	userModTime[i] = strdup(index->modTime);
    	userRemark[i] = strdup(index->remark);
    	index = index->next;
    }

    // to list users 
    if(firUserIndex < 0 ) firUserIndex = 0;
    if(firUserIndex >= userCount ) firUserIndex = userCount;
	for(int i=firUserIndex, j=1; i<userCount && j<=(yMax - 1 - 6 + 1); i++, j++) {
		char buf[1024];
    	sprintf(buf, "%d", uid[i]);
    	attron(COLOR_PAIR(4));
    	mvprintw(i+5-firUserIndex, 2, "%4s",buf);
    	attroff(COLOR_PAIR(4));
		mvaddstr(i+5-firUserIndex, 10, userName[i]);
		mvaddstr(i+5-firUserIndex, 20, userCreateTime[i]);
		mvaddstr(i+5-firUserIndex, 50, userModTime[i]);
		if(strcmp(userRemark[i], "null") != 0) {
			attron(COLOR_PAIR(3));
			mvaddstr(i+5-firUserIndex, 80, userRemark[i]);
			attroff(COLOR_PAIR(3));	
		} else {
			attron(COLOR_PAIR(6));
			mvaddstr(i+5-firUserIndex, 80, userRemark[i]);
			attroff(COLOR_PAIR(6));	
		}
	}

	// set key options
	mvaddstr(yMax-1, 0, "h:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 2, "Help");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 6, "m:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 8, "Home ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 13, "u:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 15, "Users ");
	attroff(COLOR_PAIR(1));

	if(strcmp(loginUser, "") != 0) { // if has user as parameter
		mvaddstr(yMax-1, 21, "i:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 23, "Items ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 29, "b:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 31, "Back ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 36, "q:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 38, "Quit ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 43, "1:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 45, "Page up ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 53, "2:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 55, "Page down ");
		attroff(COLOR_PAIR(1));
	} else {
		mvaddstr(yMax-1, 21, "b:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 23, "Back ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 28, "q:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 30, "Quit ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 35, "1:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 37, "Page up ");
		attroff(COLOR_PAIR(1));
		mvaddstr(yMax-1, 45, "2:");
		attron(COLOR_PAIR(1));
		mvaddstr(yMax-1, 47, "Page down ");
		attroff(COLOR_PAIR(1));
	}
	use_default_colors();

	for(int i=0; i<userCount; i++) {
		free(userName[i]);
		free(userCreateTime[i]);
		free(userModTime[i]);
		free(userRemark[i]);
	}

	freeUserList(userList);
}

static void show_item_page_options() {
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_CYAN);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_color(COLOR_GREEN, 0, 700, 0);
	init_pair(3, COLOR_GREEN, -1);
	init_pair(4, COLOR_YELLOW, -1);
	init_pair(5, COLOR_MAGENTA, -1);
	init_pair(6, 8, -1);
	init_pair(7, COLOR_RED, -1); // bright red
	init_pair(8, COLOR_CYAN, -1);

	getmaxyx(stdscr, yMax, xMax);

	char *path = malloc(strlen("./users/") + strlen(loginUser) + 1);
    strcpy(path, "./users/");
    strcat(path, loginUser);

    FILE *fp;
    fp = fopen(path, "r");
    free(path);

    char *jsonStr = (char*)malloc(MAX_JSON_LEN*sizeof(char));
    memset(jsonStr, 0, MAX_JSON_LEN);
    fgets(jsonStr, MAX_JSON_LEN, fp);
    fclose(fp);
    struct json_object *root = json_tokener_parse(jsonStr);
    free(jsonStr);
    struct json_object *items = json_object_object_get(root, "item");
    
    int itemCount = json_object_array_length(items);


    int mostCount = INT_MIN, leastCount = INT_MAX;
	char *mostUsedPassword = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	char *leastUsedPassword = malloc(MAX_JSON_LEN*sizeof(char));
	int weakCount = 0, normalCount = 0,strongCount = 0;

	struct passwordNode *passwordList = NULL; // to keep all passwords
	struct passwordNode *rear = passwordList;

    if(itemCount > 0) {
	    int pid[itemCount];
	    char *itemSite[itemCount];
	    char *itemAccount[itemCount];
	    char *itemPassword[itemCount];
	    char *itemStrongness[itemCount];
	    char *itemEmail[itemCount];
	    char *itemPhone[itemCount];
	    char *itemRemark[itemCount];

	    for(int i=0; i<itemCount; i++) { // to build item info array
	    	struct json_object *item = json_object_array_get_idx(items, i);

	    	pid[i] = i;

	    	itemSite[i] = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(itemSite[i], json_object_to_json_string(json_object_object_get(item, "site")));
	    	strcpy(itemSite[i], strtok(itemSite[i], "\""));
	    	if(strcmp(itemSite[i], "(null)") == 0) strcpy(itemSite[i], "null");

			itemAccount[i] = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(itemAccount[i], json_object_to_json_string(json_object_object_get(item, "account")));
	    	strcpy(itemAccount[i], strtok(itemAccount[i], "\""));
	    	if(strcmp(itemAccount[i], "(null)") == 0) strcpy(itemAccount[i], "null");

	    	itemPassword[i] = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(itemPassword[i], json_object_to_json_string(json_object_object_get(item, "password")));
	    	strcpy(itemPassword[i], strtok(itemPassword[i], "\""));
	    	if(strcmp(itemPassword[i], "(null)") == 0) strcpy(itemPassword[i], "null");

	    	itemStrongness[i] = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(itemStrongness[i], json_object_to_json_string(json_object_object_get(item, "strongness")));
	    	strcpy(itemStrongness[i], strtok(itemStrongness[i], "\""));
	    	if(strcmp(itemPassword[i], "null") == 0 || strcmp(itemStrongness[i], "(null)") == 0) strcpy(itemStrongness[i], "null");
			if(strcmp(itemStrongness[i], "null") != 0) {
				if(strcmp(itemStrongness[i], "weak") == 0) {
					weakCount++;
				} else if(strcmp(itemStrongness[i], "normal") == 0) {
					normalCount++;
				} else {
					strongCount++;
				}
			}


			itemEmail[i] = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(itemEmail[i], json_object_to_json_string(json_object_object_get(item, "email")));
	    	strcpy(itemEmail[i], strtok(itemEmail[i], "\""));
	    	if(strcmp(itemEmail[i], "(null)") == 0) strcpy(itemEmail[i], "null");

	    	itemPhone[i] = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(itemPhone[i], json_object_to_json_string(json_object_object_get(item, "phone")));
	    	strcpy(itemPhone[i], strtok(itemPhone[i], "\""));
	    	if(strcmp(itemPhone[i], "(null)") == 0) strcpy(itemPhone[i], "null");

	    	itemRemark[i] = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(itemRemark[i], json_object_to_json_string(json_object_object_get(item, "remark")));
	    	strcpy(itemRemark[i], strtok(itemRemark[i], "\""));
	    	if(strcmp(itemRemark[i], "(null)") == 0) strcpy(itemRemark[i], "null");
	    }

	    // to list items 
	    if(firItemIndex < 0 ) firItemIndex = 0;
	    if(firItemIndex >= itemCount ) firItemIndex = itemCount;
		for(int i=firItemIndex, j=1; i<itemCount && j<=(yMax - 1 - 6 + 1); i++, j++) {
			char buf[1024];
	    	sprintf(buf, "%d", pid[i]);
	    	attron(COLOR_PAIR(4));
	    	mvprintw(i+5-firItemIndex, 2, "%4s",buf);
	    	attroff(COLOR_PAIR(4));

	    	if(strcmp(itemSite[i], "null") == 0) {
	    		attron(COLOR_PAIR(6));
	    		mvaddstr(i+5-firItemIndex, 10, itemSite[i]);
	    		attroff(COLOR_PAIR(6));
	    	} else {
	    		mvaddstr(i+5-firItemIndex, 10, itemSite[i]);
	    	}

	    	if(strcmp(itemAccount[i], "null") == 0) {
	    		attron(COLOR_PAIR(6));
	    		mvaddstr(i+5-firItemIndex, 25, itemAccount[i]);
	    		attroff(COLOR_PAIR(6));
	    	} else {
	    		mvaddstr(i+5-firItemIndex, 25, itemAccount[i]);
	    	}

			if(strcmp(itemPassword[i], "null") == 0) {
	    		attron(COLOR_PAIR(6));
	    		mvaddstr(i+5-firItemIndex, 45, itemPassword[i]);
	    		attroff(COLOR_PAIR(6));
	    	} else {
	    		mvaddstr(i+5-firItemIndex, 45, itemPassword[i]);
	    	}

			if(strcmp(itemStrongness[i], "null") == 0) {
	    		attron(COLOR_PAIR(6));
	    		mvaddstr(i+5-firItemIndex, 65, itemStrongness[i]);
	    		attroff(COLOR_PAIR(6));
	    	} else {
	    		if(strcmp(itemStrongness[i], "weak") == 0) {
	    			attron(COLOR_PAIR(7));
	    			mvaddstr(i+5-firItemIndex, 65, itemStrongness[i]);
	    			attroff(COLOR_PAIR(7));
	    		} else if(strcmp(itemStrongness[i], "normal") == 0) {
	    			attron(COLOR_PAIR(4));
	    			mvaddstr(i+5-firItemIndex, 65, itemStrongness[i]);
	    			attroff(COLOR_PAIR(4));
	    		} else {
	    			attron(COLOR_PAIR(3));
	    			mvaddstr(i+5-firItemIndex, 65, itemStrongness[i]);
	    			attroff(COLOR_PAIR(3));
	    		}
	    		
	    	}

			if(strcmp(itemEmail[i], "null") == 0) {
	    		attron(COLOR_PAIR(6));
	    		mvaddstr(i+5-firItemIndex, 85, itemEmail[i]);
	    		attroff(COLOR_PAIR(6));
	    	} else {
	    		mvaddstr(i+5-firItemIndex, 85, itemEmail[i]);
	    	}

	    	if(strcmp(itemPhone[i], "null") == 0) {
	    		attron(COLOR_PAIR(6));
	    		mvaddstr(i+5-firItemIndex, 115, itemPhone[i]);
	    		attroff(COLOR_PAIR(6));
	    	} else {
	    		mvaddstr(i+5-firItemIndex, 115, itemPhone[i]);
	    	}

			if(strcmp(itemRemark[i], "null") != 0) {
				attron(COLOR_PAIR(3));
				mvaddstr(i+5-firItemIndex, 130, itemRemark[i]);
				attroff(COLOR_PAIR(3));	
			} else {
				attron(COLOR_PAIR(6));
				mvaddstr(i+5-firItemIndex, 130, itemRemark[i]);
				attron(COLOR_PAIR(6));
			}
			
		} 

		// to find most used password
	    for(int i=0; i<itemCount; i++) {
	    	if(strcmp(itemPassword[i], "null") == 0) continue;
	    	struct passwordNode *t = passwordList;
	    	int isFind = 0;
	    	while(t != NULL) {
	    		if(strcmp(t->password, itemPassword[i]) == 0) {
	    			isFind = 1;
	    			t->count++;	
	    			if(t->count > mostCount) {
	    				mostCount = t->count;
	    				strcpy(mostUsedPassword, itemPassword[i]);
	    			}
	    			break;
	    		}
	    		t = t->next;
	    	}

	    	if(isFind) {
	    		continue;
	    	}
	    	struct passwordNode *newPassword = malloc(sizeof(struct passwordNode));
	    	newPassword->password = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
	    	strcpy(newPassword->password, itemPassword[i]);
	    	newPassword->count = 1;
	    	newPassword->next = NULL;
	    	if(newPassword->count > mostCount) {
	    		mostCount = newPassword->count;
	    		strcpy(mostUsedPassword, itemPassword[i]);
	    	}

	    	if(passwordList == NULL) { // if first node
	            rear = newPassword;
	            passwordList = newPassword;
	        } else {
	            rear->next = newPassword;
	            rear = newPassword;
	        }
	    }

	    // to find least used password
	    struct passwordNode *t = passwordList;
	    while(t != NULL) {
	    	if(t->count < leastCount) {
	    		leastCount = t->count;
	    		strcpy(leastUsedPassword, t->password);
	    	}
	    	t = t->next;
	    }

	    for(int i=0; i<itemCount; i++) {
			free(itemSite[i]);
	    	free(itemAccount[i]);
	    	free(itemPassword[i]);
	    	free(itemStrongness[i]);
	    	free(itemEmail[i]);
	    	free(itemPhone[i]);
	    	free(itemRemark[i]);
		}
    }

	// statistical bar
    attron(COLOR_PAIR(8));
    mvaddstr(1, 3, "Total items: ");
    attroff(COLOR_PAIR(8));
	char buf[1024];
    sprintf(buf, "%d", itemCount);
	mvaddstr(1, 16, buf);

	if(mostCount != INT_MIN) {
		attron(COLOR_PAIR(8));
	    mvaddstr(1, 25, "Most used password: ");
	    attroff(COLOR_PAIR(8));
	    mvaddstr(1, 45, mostUsedPassword);

	    sprintf(buf, "%.2f%c", ((float)mostCount/itemCount)*100, '%');
	    attron(COLOR_PAIR(6));
	    mvaddstr(1, 45+strlen(mostUsedPassword)+1, buf);
	    attroff(COLOR_PAIR(6));

	    attron(COLOR_PAIR(8));
	    mvaddstr(1, 45+strlen(mostUsedPassword)+1+10, "Least used password: ");
	    attroff(COLOR_PAIR(8));
	    mvaddstr(1, 45+strlen(mostUsedPassword)+1+10+21, leastUsedPassword);

	    sprintf(buf, "%.2f%c", ((float)leastCount/itemCount)*100, '%');
	    attron(COLOR_PAIR(6));
	    mvaddstr(1, 45+strlen(mostUsedPassword)+1+10+21+strlen(leastUsedPassword)+1, buf);
	    attroff(COLOR_PAIR(6));

	    attron(COLOR_PAIR(7));
	    mvaddstr(1, 110, "weak");
	    attroff(COLOR_PAIR(7));

	    attron(COLOR_PAIR(4));
	    mvaddstr(1, 125, "normal");
	    attroff(COLOR_PAIR(4));

	    attron(COLOR_PAIR(3));
	    mvaddstr(1, 142, "strong");
	    attroff(COLOR_PAIR(3));

	    sprintf(buf, "%.2f%c", ((float)weakCount/(weakCount+normalCount+strongCount))*100, '%');
	    attron(COLOR_PAIR(6));
		mvaddstr(2, 109, buf);
		attroff(COLOR_PAIR(6));

		sprintf(buf, "%.2f%c", ((float)normalCount/(weakCount+normalCount+strongCount))*100, '%');
	    attron(COLOR_PAIR(6));
		mvaddstr(2, 125, buf);
		attroff(COLOR_PAIR(6));

		sprintf(buf, "%.2f%c", ((float)strongCount/(weakCount+normalCount+strongCount))*100, '%');
	    attron(COLOR_PAIR(6));
		mvaddstr(2, 142, buf);
		attroff(COLOR_PAIR(6));
    }
	
    // info bar
    attron(COLOR_PAIR(1));
    for(int i=0; i<xMax; i++) {
    	mvaddstr(4, i, " ");
    }
    mvaddstr(4, 3, "PID");
    mvaddstr(4, 10, "Site");
    mvaddstr(4, 25, "Account");
    mvaddstr(4, 45, "Password");
    mvaddstr(4, 65, "Strongness");
    mvaddstr(4, 85, "Email");
    mvaddstr(4, 115, "Phone");
    mvaddstr(4, 130, "Remark");
    attroff(COLOR_PAIR(1));

    // set key options
	mvaddstr(yMax-1, 0, "h:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 2, "Help");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 6, "m:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 8, "Home ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 13, "u:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 15, "Users ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 21, "i:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 23, "Items ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 29, "b:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 31, "Back ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 36, "q:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 38, "Quit ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 43, "1:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 45, "Page up ");
	attroff(COLOR_PAIR(1));
	mvaddstr(yMax-1, 53, "2:");
	attron(COLOR_PAIR(1));
	mvaddstr(yMax-1, 55, "Page down ");
	attroff(COLOR_PAIR(1));

	use_default_colors();

	freePasswordList(passwordList);
}

static void show_help_page_options() {
	start_color();
	init_pair(1, COLOR_BLACK, COLOR_CYAN);
	init_pair(2, COLOR_BLACK, COLOR_GREEN);
	init_color(COLOR_GREEN, 0, 700, 0);
	init_pair(3, COLOR_GREEN, -1);
	init_pair(4, COLOR_YELLOW, -1);
	init_pair(5, COLOR_MAGENTA, -1);
	init_pair(6, 8, -1);
	init_pair(7, COLOR_RED, -1); // bright red
	init_pair(8, COLOR_CYAN, -1);
	init_pair(9, COLOR_BLACK, COLOR_YELLOW);

	// to print CLI mode instruction
	attron(COLOR_PAIR(9));
	mvaddstr(0, 0, "CLI mode:");
	attroff(COLOR_PAIR(8));

	attron(COLOR_PAIR(8));
	mvaddstr(1, 4, " -a user_name");
	attroff(COLOR_PAIR(8));
	mvaddstr(2, 8, " to add a password item into user's codebook");

	attron(COLOR_PAIR(8));
	mvaddstr(3, 4, " -c user_name");
	attroff(COLOR_PAIR(8));
	mvaddstr(4, 8, " to create a user in user pool");

	attron(COLOR_PAIR(8));
	mvaddstr(5, 4, " -d user_name");
	attroff(COLOR_PAIR(8));
	mvaddstr(6, 8, " to delete a user from user pool");

	attron(COLOR_PAIR(8));
	mvaddstr(7, 4, " -g user_name");
	attroff(COLOR_PAIR(8));
	mvaddstr(8, 8, " to switch into text GUI mode to browse user's codebook");

	attron(COLOR_PAIR(8));
	mvaddstr(9, 4, " -i user_name");
	attroff(COLOR_PAIR(8));
	mvaddstr(10, 8, " to modify user's information");

	attron(COLOR_PAIR(8));
	mvaddstr(11, 4, " -m user_name");
	attroff(COLOR_PAIR(8));
	mvaddstr(12, 8, " to modify information of password item from user's codebook");

	attron(COLOR_PAIR(8));
	mvaddstr(13, 4, " -r user_name");
	attroff(COLOR_PAIR(8));
	mvaddstr(14, 8, " to remove an password item from user's codebook");

	attron(COLOR_PAIR(8));
	mvaddstr(15, 4, " -u");
	attroff(COLOR_PAIR(8));
	mvaddstr(16, 8, " to show all existed users from user pool");

	// to print text GUI mode instruction
	attron(COLOR_PAIR(9));
	mvaddstr(0, 75, "CLI mode:");
	attroff(COLOR_PAIR(8));

	attron(COLOR_PAIR(8));
	mvaddstr(1, 79, " h: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(1, 83, "help page");

	attron(COLOR_PAIR(8));
	mvaddstr(1, 95, " m: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(1, 99, "home page");

	attron(COLOR_PAIR(8));
	mvaddstr(1, 111, " u: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(1, 115, "user page");

	attron(COLOR_PAIR(8));
	mvaddstr(1, 127, " i: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(1, 131, "item page");

	attron(COLOR_PAIR(8));
	mvaddstr(3, 79, " 1: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(3, 83, "page up");

	attron(COLOR_PAIR(8));
	mvaddstr(3, 95, " 2: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(3, 99, "home down");

	attron(COLOR_PAIR(8));
	mvaddstr(5, 79, " b: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(5, 83, "back to previous page");

	attron(COLOR_PAIR(8));
	mvaddstr(7, 79, " q: ");
	attroff(COLOR_PAIR(8));
	mvaddstr(7, 83, "quit text GUI mode");

	mvaddstr(yMax-1, 0, "press any key to return...");
}

void mainPage(char *user) {
	// home page in GUI mode
	
	stackCount++; // record the remaining number of previous pages in stack
	strcpy(loginUser, user);

	initscr();

	show_main_page_options();
	int key;
	// while (key = getch()) != 27
    while ((key = getch())) {
        if (key == KEY_RESIZE) {
            clear();
			show_main_page_options();
        } else if(key == 'h') {
        	clear();
        	helpPage();
        	show_main_page_options();
        } else if(key == 'm') {
        	if(strcmp(loginUser, "") == 0) {
        		mvaddstr(yMax-1, 35, "        ");
        		mvaddstr(yMax-1, 35, "");
        	} else {
        		mvaddstr(yMax-1, 43, "        ");
        		mvaddstr(yMax-1, 43, "");
        	}
        	continue;
        } else if(key == 'u') {
        	clear();
        	userPage();
        	show_main_page_options();
        } else if(key == 'b') {
        	stackCount--;
        	if(stackCount == 0) {
        		endwin();
        		exit(0);
        	} else {
        		clear();
        		return;
        	}
        } else if(key == 'q') {
        	clear();
        	endwin();
        	exit(0);
        } else if(key == 'i' && strcmp(loginUser, "") != 0) {
        	clear();
        	itemPage();
        	show_main_page_options();
        } else { // clear char on screen
        	if(strcmp(loginUser, "") == 0) {
        		mvaddstr(yMax-1, 35, "        ");
        		mvaddstr(yMax-1, 35, "");
        	} else {
        		mvaddstr(yMax-1, 43, "        ");
        		mvaddstr(yMax-1, 43, "");
        	}
        }
        refresh();
    }
    endwin();   
}

static void userPage() {
	// all users page in GUI mode
	stackCount++;

	show_user_page_options();
	int key;
	while((key = getch())) {
		if (key == KEY_RESIZE) {
            clear();
			show_user_page_options();
        } else if(key == 'h') {
        	clear();
        	helpPage();
        	show_user_page_options();
        } else if(key == 'm') {
        	clear();
        	mainPage(loginUser);
        	show_user_page_options();
        } else if(key == 'u') {
        	if(strcmp(loginUser, "") == 0) {
        		mvaddstr(yMax-1, 57, "        ");
        		mvaddstr(yMax-1, 57, "");
        	} else {
        		mvaddstr(yMax-1, 65, "        ");
        		mvaddstr(yMax-1, 65, "");
        	}
        	continue;
        } else if(key == 'i' && strcmp(loginUser, "") != 0) {
        	clear();
        	itemPage();
        	show_user_page_options();
        } else if(key == 'b') {
        	stackCount--;
        	if(stackCount == 0) {
        		endwin();
        		exit(0);
        	} else {
        		clear();
        		return;
        	}
        } else if(key == 'q') {
        	clear();
        	endwin();
        	exit(0);
        } else if(key == '1') {
        	firUserIndex -= yMax - 1 - 6 + 1;
        	clear();
        	show_user_page_options();
        } else if(key == '2') {
        	firUserIndex += yMax - 1 - 6 + 1;
        	clear();
        	show_user_page_options();
        } else {
        	if(strcmp(loginUser, "") == 0) {
        		mvaddstr(yMax-1, 57, "        ");
        		mvaddstr(yMax-1, 57, "");
        	} else {
        		mvaddstr(yMax-1, 65, "        ");
        		mvaddstr(yMax-1, 65, "");
        	}
        }
        refresh();
	}
	
	return;
}

static void itemPage() {
	// all items page in GUI mode
	stackCount++;

	show_item_page_options();

	int key;
	while((key = getch())) {
		if (key == KEY_RESIZE) {
            clear();
			show_item_page_options();
        } else if(key == 'h') {
        	clear();
        	helpPage();
        	show_item_page_options();
        } else if(key == 'm') {
        	clear();
        	mainPage(loginUser);
        	show_item_page_options();
        } else if(key == 'u') {
        	clear();
        	userPage();
        	show_item_page_options();
        } else if(key == 'i') {
        	mvaddstr(yMax-1, 65, "        ");
        	mvaddstr(yMax-1, 65, "");
        	continue;
        } else if(key == 'b') {
        	stackCount--;
        	if(stackCount == 0) {
        		endwin();
        		exit(0);
        	} else {
        		clear();
        		return;
        	}
        } else if(key == 'q') {
        	clear();
        	endwin();
        	exit(0);
        } else if(key == '1') {
        	firItemIndex -= yMax - 1 - 6 + 1;
        	clear();
        	show_item_page_options();
        } else if(key == '2') {
        	firItemIndex += yMax - 1 - 6 + 1;
        	clear();
        	show_item_page_options();
        } else {
        	mvaddstr(yMax-1, 65, "        ");
        	mvaddstr(yMax-1, 65, "");
        }
        refresh();
	}
	
	return;
}

static void helpPage() {
	// show help page
	stackCount++; // record the remaining number of previous pages in stack

	show_help_page_options();

	int key;
    while ((key = getch())) {
    	stackCount--;
    	clear();
    	return;
    }
}