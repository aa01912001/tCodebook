#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <json-c/json.h>
#include "tcb_gui.h"

#define MAX_ATTIBUTE_LEN 100
#define MAX_JSON_LEN 65535

char* getStrongness(const char* password) {
    // to calculate strongness and return safety level
    int hasLower = 0, hasUpper = 0, hasNum = 0, hasOther = 0;
    int len = strlen(password);
    for(int i=0; i<len; i++) {
        if(password[i] >= 'a'  && password[i] <= 'z') {
            hasLower = 1;
        } else if(password[i] >= '0' && password[i] <= '9') {
            hasNum = 1;
        } else if(password[i] >='A' && password[i] <= 'Z') {
            hasUpper = 1;
        } else {
            hasOther = 1;
        }
    }

    int score = hasLower + hasUpper + hasNum + hasOther;
    if(len >= 10) score += 1;

    if(score == 5) {
        return "strong";
    } else if(score >= 3) {
        return "normal";
    }
    return "weak";
} 

int main(int argc, char *argv[]) {
    struct stat st;

    // check if users folder exists
    if(stat("./users", &st) == -1) { 
        mkdir("./users", 0777);
    }

    int opt;
    while((opt = getopt(argc, argv, "a:c:d:i:ugr:m:")) != -1) {
        switch(opt) {
            case 'a': { // add an item
                char *path = malloc(strlen("./users/") + strlen(optarg) + 1);
                strcpy(path, "./users/");
                strcat(path, optarg);
                if(stat(path, &st) == -1) {
                    printf("user %s is not existed\n", optarg);
                } else {
                    char *site = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    char *account = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    char *password = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    char *strongness = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    char *email = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    char *phone = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    char *remark = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    printf("site: ");
                    fgets(site, MAX_ATTIBUTE_LEN, stdin);
                    if(strtok(site, "\n") != NULL) {
                        strcpy(site, strtok(site, "\n"));
                    } else {
                        strcpy(site, "(null)");
                    }
                    
                    printf("account: ");
                    fgets(account, MAX_ATTIBUTE_LEN, stdin);
                    if(strtok(account, "\n") != NULL) {
                        strcpy(account, strtok(account, "\n"));
                    } else {
                        strcpy(account, "(null)");
                    }

                    printf("password: ");
                    fgets(password, MAX_ATTIBUTE_LEN, stdin);
                    if(strtok(password, "\n") != NULL) {
                        strcpy(password, strtok(password, "\n"));
                    } else {
                        strcpy(password, "(null)");
                    }

                    printf("email: ");
                    fgets(email, MAX_ATTIBUTE_LEN, stdin);
                    if(strtok(email, "\n") != NULL) {
                        strcpy(email, strtok(email, "\n"));
                    } else {
                        strcpy(email, "(null)");
                    }

                    printf("phone: ");
                    fgets(phone, MAX_ATTIBUTE_LEN, stdin);
                    if(strtok(phone, "\n") != NULL) {
                        strcpy(phone, strtok(phone, "\n"));
                    } else {
                        strcpy(phone, "(null)");
                    }

                    printf("remark: ");
                    fgets(remark, MAX_ATTIBUTE_LEN, stdin);
                    if(strtok(remark, "\n") != NULL) {
                        strcpy(remark, strtok(remark, "\n"));
                    } else {
                        strcpy(remark, "(null)");
                    }


                    if(strcmp(site, "(null)") == 0) {
                        printf("site is required!!\n");
                        return 0;
                    }
                    
                    if(strcmp(password, "(null)") == 0) {
                        strcpy(password, "(null)");
                        strcpy(strongness, "(null)");
                    } else {
                        strcpy(strongness, getStrongness(password));
                    }

                    // to build user json
                    char *content = malloc(MAX_JSON_LEN*sizeof(char));
                    sprintf(content ,"{\"site\":\"%s\",\"account\":\"%s\",\"password\":\"%s\",\"strongness\":\"%s\",\"email\":\"%s\",\"phone\":\"%s\",\"remark\":\"%s\",}",  site, account, password, strongness, email, phone, remark);
                    struct json_object *newItem = json_tokener_parse(content);
                    free(content);

                    char *jsonStr = malloc(MAX_JSON_LEN*sizeof(char));
                    memset(jsonStr, 0, MAX_JSON_LEN);

                    FILE *fp;
                    fp = fopen(path, "r");
                    fgets(jsonStr, MAX_JSON_LEN, fp);
                    fclose(fp);
                    struct json_object *root = json_tokener_parse(jsonStr);

                    struct json_object *items = json_object_object_get(root, "item");

                    // check whether site existed
                    int itemLen = json_object_array_length(items);
                    for(int i=0; i<itemLen; i++) {
                        struct json_object *item = json_object_array_get_idx(items, i);
                        struct json_object *itemSite = json_object_object_get(item, "site");

                        char *tmp = strdup(json_object_to_json_string(itemSite));
                        strcpy(tmp, strtok(tmp, "\""));
                        if(strcmp(site, tmp) == 0) {
                            printf("%s has exited!!\n", site);
                            free(tmp);
                            return 0;
                        }
                        free(tmp);
                    }

                    json_object_array_add(items, newItem);

                    time_t current_time;
                    time(&current_time);
                    struct json_object *modTimeObj = json_object_object_get(root, "modTime");
                    json_object_object_add(root, "modTime", json_object_new_string(strtok(ctime(&current_time), "\n")));

                    fp = fopen(path, "w");
                    fputs(json_object_to_json_string(root), fp);
                    fclose(fp);
                    free(path);

                    free(jsonStr);
                    free(site);
                    free(account);
                    free(password);
                    free(strongness);
                    free(email);
                    free(phone);
                    free(remark);
                }
                break;
            }
            case 'c': { // create a user
                char *path = malloc(strlen("./users/") + strlen(optarg) + 1);
                strcpy(path, "./users/");
                strcat(path, optarg);
                if(stat(path, &st) != -1) { 
                    printf("%s has existed\n", optarg);
                } else {
                    FILE *fp;
                    fp = fopen(path, "a");
                    time_t current_time;
                    time(&current_time);

                    char *remark = "";
                    char *content = malloc(MAX_JSON_LEN*sizeof(char));

                    sprintf(content ,"{\"createTime\":\"%s\",\"modTime\":\"%s\",\"remark\":\"%s\",\"item\":[]}", strtok(ctime(&current_time), "\n"), strtok(ctime(&current_time), "\n"), remark);
                    fputs(content, fp);
                    fclose(fp);
                    printf("%s has been created\n", optarg);
                    free(path);
                }
                break;
            }
            case 'd': { // delete a user
                char *path = malloc(strlen("./users/") + strlen(optarg) + 1);
                strcpy(path, "./users/");
                strcat(path, optarg); 
                if(stat(path, &st) == -1) {
                    printf("user %s is not existed\n", optarg);
                } else {
                    if(remove(path) == 0) {
                        printf("%s has been deleted\n", optarg);
                    }
                }
                free(path);
                break;
            }
            case 'i': { // modify user info
                char *path = malloc(strlen("./users/") + strlen(optarg) + 1);
                strcpy(path, "./users/");
                strcat(path, optarg); 
                if(stat(path, &st) == -1) {
                    printf("user %s is not existed\n", optarg);
                } else {
                    int modOpt;
                    printf("choose an info you want to modify( 0: name, 1: remark ): ");
                    scanf("%d", &modOpt);

                    FILE *fp;
                    fp = fopen(path, "r");

                    char *jsonStr = malloc(MAX_JSON_LEN*sizeof(char));
                    memset(jsonStr, 0, MAX_JSON_LEN);
                    fgets(jsonStr, MAX_JSON_LEN, fp);
                    fclose(fp);
                    struct json_object *root = json_tokener_parse(jsonStr);
                    free(jsonStr);

                    if(modOpt == 0) { // modify user name
                        printf("new name: ");
                        char *newName = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                        char c;
                        while((c = getchar()) != '\n' && c != EOF) {}
                        fgets(newName, MAX_ATTIBUTE_LEN, stdin);

                        if(strtok(newName, "\n") != NULL) {
                            strcpy(newName, strtok(newName, "\n"));
                        } else {
                            printf("empty is not allowed!!\n");
                            return 0;
                        }
                        

                        char *newPath = malloc(strlen("./users/") + strlen(newName) + 1);
                        strcpy(newPath, "./users/");
                        strcat(newPath, newName);

                        if(stat(newPath, &st) == -1) { 
                            rename(path, newPath);
                        } else {
                            printf("%s has existed\n", newName);
                        }
                        free(newPath);
                        free(newName);

                    } else if(modOpt == 1) { // modify remark
                        printf("new remark: ");
                        char *newRemark = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                        char c;
                        while((c = getchar()) != '\n' && c != EOF) {}
                        fgets(newRemark, MAX_ATTIBUTE_LEN, stdin);
                        
                        if(strtok(newRemark, "\n") != NULL) {
                            strcpy(newRemark, strtok(newRemark, "\n"));
                        } else {
                            strcpy(newRemark, "(null)");
                        }

                        struct json_object *remarkObj = json_object_object_get(root, "remark");
                        json_object_object_add(root, "remark", json_object_new_string(newRemark));
                        fp = fopen(path, "w");
                        fputs(json_object_to_json_string(root), fp);
                        fclose(fp);
                        free(newRemark);
                    }
                    free(path);
                }
                break;
            }
            case 'u': { // show all users
                DIR *d;
                struct dirent *dir;
                d = opendir("./users");
                if(d) {
                    while((dir = readdir(d)) != NULL) {
                        if(dir->d_name[0] != '.') {
                            printf("%s\n", dir->d_name);
                        }
                    }
                    closedir(d);
                } else {
                    printf("no any user\n");
                }
                break;
            }
            case 'g': { // text GUI mode
                printf("enter a user you wanna log in (directly enter as guest): ");
                char *user = malloc(MAX_ATTIBUTE_LEN*sizeof(char));;
                fgets(user, MAX_ATTIBUTE_LEN, stdin);
                if(strcmp(user, "\n") == 0) {
                    strcpy(user, "");
                } else {
                    strcpy(user, strtok(user, "\n"));
                }

                char *path = malloc(strlen("./users/") + strlen(user) + 1);
                strcpy(path, "./users/");
                strcat(path, user); 

                if(strcmp(user, "") != 0 && stat(path, &st) == -1) {
                    printf("user %s is not existed\n", user);
                } else { // if user exists or log in as a guest
                    mainPage(user);
                }
                free(user);
                free(path);
                break;
            }
            case 'r': { // remove item
                char *path = malloc(strlen("./users/") + strlen(optarg) + 1);
                strcpy(path, "./users/");
                strcat(path, optarg); 
                if(stat(path, &st) == -1) {
                    printf("user %s is not existed\n", optarg);
                } else {
                    FILE *fp;
                    fp = fopen(path, "r");

                    char *jsonStr = malloc(MAX_JSON_LEN*sizeof(char));
                    memset(jsonStr, 0, MAX_JSON_LEN);
                    fgets(jsonStr, MAX_JSON_LEN, fp);
                    fclose(fp);
                    struct json_object *root = json_tokener_parse(jsonStr);
                    free(jsonStr);

                    printf("remove site: ");
                    char *removeSite = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    fgets(removeSite, MAX_ATTIBUTE_LEN, stdin);
                    if(strcmp(removeSite, "\n") == 0) {
                        printf("empty is not allowed!!\n");
                        return 0;
                    } else {
                        strcpy(removeSite, strtok(removeSite, "\n"));
                    }    

                    struct json_object *items = json_object_object_get(root, "item");
                    int itemLen = json_object_array_length(items);
                    struct json_object *newItems = json_object_new_array();

                    int itemExsit = 0;
                    for(int i=0; i<itemLen; i++) { 
                        struct json_object *item = json_object_array_get_idx(items, i);
                        struct json_object *itemSite = json_object_object_get(item, "site");
                        char *tmp = strdup(json_object_to_json_string(itemSite));
                        if(strcmp(removeSite, strtok(tmp, "\"")) != 0) {
                            free(tmp);
                            char *site = strdup(json_object_to_json_string(json_object_object_get(item, "site")));
                            char *account = strdup(json_object_to_json_string(json_object_object_get(item, "account")));
                            strcpy(account, strtok(account, "\""));
                            char *password = strdup(json_object_to_json_string(json_object_object_get(item, "password")));
                            strcpy(password, strtok(password, "\""));
                            char *strongness = strdup(json_object_to_json_string(json_object_object_get(item, "strongness")));
                            strcpy(strongness, strtok(strongness, "\""));
                            char *email = strdup(json_object_to_json_string(json_object_object_get(item, "email")));
                            strcpy(email, strtok(email, "\""));
                            char *phone = strdup(json_object_to_json_string(json_object_object_get(item, "phone")));
                            strcpy(phone, strtok(phone, "\""));
                            char *remark = strdup(json_object_to_json_string(json_object_object_get(item, "remark")));
                            strcpy(remark, strtok(remark, "\""));

                            char *content = (char *)malloc(MAX_JSON_LEN);
                            sprintf(content ,"{\"site\":\"%s\",\"account\":\"%s\",\"password\":\"%s\",\"strongness\":\"%s\",\"email\":\"%s\",\"phone\":\"%s\",\"remark\":\"%s\",}",  strtok(site, "\"") , account, password, strongness, email, phone, remark);
                            struct json_object *copyItem = json_tokener_parse(content);
                            json_object_array_add(newItems, copyItem);
                            
                            free(content);
                            free(site);
                            free(account);
                            free(password);
                            free(strongness);
                            free(email);
                            free(phone);
                            free(remark);
                        } else {
                            itemExsit = 1;
                        }
                        free(removeSite);
                    }

                    json_object_object_add(root, "item", newItems);

                    if(itemExsit) {
                        time_t current_time;
                        time(&current_time);
                        struct json_object *modTimeObj = json_object_object_get(root, "modTime");
                        json_object_object_add(root, "modTime", json_object_new_string(strtok(ctime(&current_time), "\n")));
                    } else {
                        printf("site is not existed!\n");
                    }

                    fp = fopen(path, "w");
                    fputs(json_object_to_json_string(root), fp);
                    fclose(fp);
                    free(path);
                }
                break;
            }
            case 'm': { // modify item info
                char *path = malloc(strlen("./users/") + strlen(optarg) + 1);
                strcpy(path, "./users/");
                strcat(path, optarg); 
                if(stat(path, &st) == -1) {
                    printf("user %s is not existed\n", optarg);
                } else {
                    char *modSite = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                    printf("which site you wanna modify: ");
                    fgets(modSite, MAX_ATTIBUTE_LEN, stdin);
                    if(strcmp(modSite, "\n") == 0) {
                        printf("empty is not allowed!!\n");
                        return 0;
                    } else {
                        strcpy(modSite, strtok(modSite, "\n"));
                    }    

                    FILE *fp;
                    fp = fopen(path, "r");

                    char *jsonStr = malloc(MAX_JSON_LEN*sizeof(char));
                    memset(jsonStr, 0, MAX_JSON_LEN);
                    fgets(jsonStr, MAX_JSON_LEN, fp);
                    fclose(fp);
                    struct json_object *root = json_tokener_parse(jsonStr);
                    free(jsonStr);

                    struct json_object *items = json_object_object_get(root, "item");
                    struct json_object *targetItem;
                    int itemLen = json_object_array_length(items);
                    int itemExsit = 0;
                    for(int i=0; i<itemLen; i++) {
                        struct json_object *item = json_object_array_get_idx(items, i);
                        struct json_object *itemSite = json_object_object_get(item, "site");

                        char *tmp = strdup(json_object_to_json_string(itemSite));
                        strcpy(tmp, strtok(tmp, "\""));
                        if(strcmp(modSite, tmp) == 0) {
                            free(tmp);
                            targetItem = item;
                            itemExsit = 1;
                            break;
                        }
                    }
                    free(modSite);

                    if(itemExsit) {
                        int modOpt;
                        printf("choose an info you want to modify( 0: site, 1: account, 2: password, 3: email, 4: phone, 5: remark ): ");
                        scanf("%d", &modOpt);
                        char c;
                        while((c = getchar()) != '\n' && c != EOF) {}
                        if(modOpt == 0) {
                            printf("new site: ");
                            char *newSite = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                            fgets(newSite, MAX_ATTIBUTE_LEN, stdin);
                            if(strlen(newSite) == 1) {
                                printf("site is required!!\n");
                                return 0;
                            }
                            strcpy(newSite, strtok(newSite, "\n"));
                             
                            // check whether site existed
                            for(int i=0; i<itemLen; i++) {
                                struct json_object *item = json_object_array_get_idx(items, i);
                                struct json_object *itemSite = json_object_object_get(item, "site");

                                char *tmp = strdup(json_object_to_json_string(itemSite));
                                strcpy(tmp, strtok(tmp, "\""));
                                if(strcmp(newSite, tmp) == 0) {
                                    free(tmp);
                                    printf("%s has exited!!\n", newSite);
                                    return 0;
                                }
                            }

                            json_object_object_add(targetItem, "site", json_object_new_string(newSite));
                            free(newSite);
                        } else if(modOpt == 1) {
                            printf("new account: ");
                            char *newAccount = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                            fgets(newAccount, MAX_ATTIBUTE_LEN, stdin);
                            
                            if(strlen(newAccount) == 1) {
                                strcpy(newAccount, "(null)");
                            } 

                            strcpy(newAccount, strtok(newAccount, "\n"));
                            json_object_object_add(targetItem, "account", json_object_new_string(newAccount));
                            free(newAccount);
                        } else if(modOpt == 2) {
                            printf("new password: ");
                            char *newPassword = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                            fgets(newPassword, MAX_ATTIBUTE_LEN, stdin);
                            if(strlen(newPassword) == 1) {
                                strcpy(newPassword, "(null)");
                            } 
                            
                            strcpy(newPassword, strtok(newPassword, "\n"));
                            json_object_object_add(targetItem, "password", json_object_new_string(newPassword));

                            char *newStrongness = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                            strcpy(newStrongness, getStrongness(newPassword));
                            json_object_object_add(targetItem, "strongness", json_object_new_string(newStrongness));
                        } else if(modOpt == 3) {
                            printf("new email: ");
                            char *newEmail = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                            fgets(newEmail, MAX_ATTIBUTE_LEN, stdin);
                            if(strlen(newEmail) == 1) {
                                strcpy(newEmail, "(null)");
                            } 
                            
                            strcpy(newEmail, strtok(newEmail, "\n"));
                            json_object_object_add(targetItem, "email", json_object_new_string(newEmail));
                        } else if(modOpt == 4) {
                            printf("new phone: ");
                            char *newPhone = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                            fgets(newPhone, MAX_ATTIBUTE_LEN, stdin);
                            if(strlen(newPhone) == 1) {
                                strcpy(newPhone, "(null)");
                            } 
                            
                            strcpy(newPhone, strtok(newPhone, "\n"));
                            json_object_object_add(targetItem, "phone", json_object_new_string(newPhone));
                        } else if(modOpt == 5) {
                            printf("new remark: ");
                            char *newRemark = malloc(MAX_ATTIBUTE_LEN*sizeof(char));
                            fgets(newRemark, MAX_ATTIBUTE_LEN, stdin);
                            if(strlen(newRemark) == 1) {
                                strcpy(newRemark, "(null)");
                            } 
                        
                            strcpy(newRemark, strtok(newRemark, "\n"));
                            json_object_object_add(targetItem, "remark", json_object_new_string(newRemark));
                        } else {
                            printf("option out of range!!\n");
                            return 0;
                        }

                        time_t current_time;
                        time(&current_time);
                        struct json_object *modTimeObj = json_object_object_get(root, "modTime");
                        json_object_object_add(root, "modTime", json_object_new_string(strtok(ctime(&current_time), "\n")));

                        fp = fopen(path, "w");
                        fputs(json_object_to_json_string(root), fp);
                        fclose(fp);
                    } else {
                        printf("site is not exited!!\n");
                    }
                }
                free(path);
                break;
            }
            default:
                printf("incorrect usage!!\n");
        }
    }

    return 0;
}