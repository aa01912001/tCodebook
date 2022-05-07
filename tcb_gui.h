struct userNode {
	struct userNode *next;
	int userID;
	char *name;
	char *createTime;
	char *modTime;
	char *remark;
};

struct passwordNode {
	struct passwordNode *next;
	char *password;
	int count;
};

static void show_main_page_options();

static void show_user_page_options();

static void show_item_page_options();

static void show_help_page_options();

static void freeUserList(struct userNode *);

static void freePasswordList(struct passwordNode *);

void mainPage(char *user);

static void userPage();

static void itemPage();

static void helpPage();
