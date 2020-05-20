#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<ctype.h>

#define MAX 300

typedef struct Book {
	int id;
	char title[50];
	char author[50];
	char genre[20];
	int length;
	char publisher[50];
	int search_match;
} book;

int is_admin = 0;

book *books[MAX] = {NULL};
int noofbooks = 0;

int load_books();
int reload_books();
book *parse_book(int id, char *line);
int delete_book(int id);
int insert_book(char *title, char *author, char *genre, char *publisher, int length);

void admin_login();

void print_books(book **books, int noofbooks, int start, int end);

void menu_display();
void viewall_display();
void book_display(int id);
void insertbook_display();

int search_string(char *str, char *key);

book **sorted_books(char by);
book **searched_books(char by, char *key);

void main() {
	
	system("clear");

	printf("\nLoading Books...\n\n");
	
	if(load_books())
		printf("Loaded books sucessfully!\n");
	else {
		printf("Could not load books!\nExiting...\n");
		fflush(stdout);
		sleep(3);
		system("clear");
		return;
	}

	menu_display();
}

int load_books() {
	FILE *fp;
	char line[200] = "";

	int i=-1;

	fp = fopen("books.csv", "r");
	if (fp == NULL)
        	return 0;

	fgets(line, 100, fp);

	while(fgets(line, 200, fp)) {
		if(line[0]!='\0') {
			i++;
			books[i] = parse_book(i, line);
		}
	}

	noofbooks = ++i;
	fclose(fp);
	return 1;
}

book *parse_book(int id, char *line){
	book *tmpbook;

	tmpbook = (book *)malloc(sizeof(book));
	char tmpstr[50]="";
	int i=-1, j=0, tmpint;

	tmpbook->id = id;
	tmpbook->search_match = 0;

	//Read Book Title
	while(line[++i]!=',')
		tmpbook->title[j++]=line[i];
	tmpbook->title[j]='\0';

	//Read Book Author
	j=0;
	while(line[++i]!=',')
		tmpbook->author[j++]=line[i];
	tmpbook->author[j]='\0';

	//Read Book Genre
	j=0;
	while(line[++i]!=',')
		tmpbook->genre[j++]=line[i];
	tmpbook->genre[j]='\0';

	//Read Book Length
	j=0;
	while(line[++i]!=',')
		tmpstr[j++]=line[i];
	tmpstr[j]='\0';
	sscanf(tmpstr, "%d", &tmpint);
	tmpbook->length = tmpint;

	//Read Book Publisher
	j=0;
	while(line[++i]!='\n')
		tmpbook->publisher[j++]=line[i];
	tmpbook->publisher[j]='\0';

	return tmpbook;
}

int reload_books(){
	int i;

	for(i=0; i<noofbooks; i++) {
		free(books[i]);
		books[i]=NULL;
	}

	noofbooks = 0;

	return load_books();
}

int insert_book(char *title, char *author, char *genre, char *publisher, int length) {
	FILE *fp;
	
	fp = fopen("books.csv", "a");
	if (fp == NULL)
        	return 0;

	fprintf(fp, "%s,%s,%s,%d,%s\n", title, author, genre, length, publisher);
	
	fclose(fp);

	return reload_books();
}

int delete_book(int id) {
	FILE *fp, *tempfp;
	char ch;
	int i=-1;

	fp = fopen("books.csv", "r");
		if (fp == NULL)
        	return 0;

	tempfp = fopen("replica.csv", "w");
		if (tempfp == NULL)
			return 0;

	ch = getc(fp);
	while (ch!=EOF) {
		if (ch=='\n')
			i++;
		if (i!=id)
			putc(ch, tempfp);

		ch = getc(fp);
	}

	fclose(fp);
	fclose(tempfp);

	remove("books.csv");
	rename("replica.csv", "books.csv");

	return reload_books();
}

void admin_login(){
	int choice;
	char password[10];

	system("clear");

	printf("-----Admin Login-----\n\n");

	printf("Are you an admin:\n\t1) Yes\n\t2) No\n");

	printf("\nEnter your choice (or 0 to go back):");
	scanf("%d", &choice);

	if(!choice) {
		return;
	}
	
	if(choice==1) {
		printf("Enter admin password:");
		scanf("%s", password);

		if(!strcmp(password, "admin")) {
			printf("Logging in...");
			fflush(stdout);
			is_admin = 1;
			sleep(1);
			return;
		} else {
			printf("Wrong password, continuing as guest...");
			fflush(stdout);
			sleep(1);
			return;
		}
	} else if(choice!=2) {
		printf("Invalid option, continuing as guest...");
		fflush(stdout);
		sleep(1);
	}
	return;

}

void menu_display() {
	int ch=99;
	while(ch) {
		system("clear");

		printf("-----LIBRARY MANAGEMENT SYSTEM-----\n\n");

		printf("\t1) View All Books\n");

		if(is_admin){
			printf("\t2) Logout\n");
		} else {
			printf("\t2) Login\n");
		}

		printf("\t0) Exit\n");
		printf("\nEnter your choice:");
		scanf("%d", &ch);
		
		switch(ch) {
			case 1:
				viewall_display(books);
				break;
			case 2:
				if(is_admin)
					is_admin = 0;
				else
					admin_login();
		}
	}

	system("clear");
}

void print_books(book **books, int noofbooks, int start, int end){
	int i;
	printf("\n\n\t%-3.3s\t%-40.40s\t%-20.20s\t%-15.15s\t%-15.15s\t%-10.10s\n\n","Id", "Title", "Author", "Genre", "Publisher", "Length");
	for(i=start; i<end && i<noofbooks; i++){
		printf("\t%-3.3d\t%-40.40s\t%-20.20s\t%-15.15s\t%-15.15s\t%-10d\n", books[i]->id, books[i]->title, books[i]->author, books[i]->genre, books[i]->publisher, books[i]->length);
	}
}

void viewall_display() {
	int i=0, ch=99, id=0, c=0;
	char key[50];
	book **displayed_books = books;

	while(ch){
		system("clear");

		print_books(displayed_books, noofbooks, i*20, (i*20)+20);

		printf("\n\t\tPage %d of %d\n", i+1, (noofbooks/20)+1);
		printf("\n\t%-15.15s\t%-15.15s\t%-20.20s", "1) Previous", "2) Next", "3) Select Book"); 
		if(is_admin)
			printf("\t%-15.15s", "4) Insert Book");
		printf("\n\t%-20.20s\t%-20.20s\t%-20.20s\t%-20.20s\t%-20.20s\n", "5) Sort by Title", "6) Sort by Author", "7) Sort by Genre", "8) Sort by Publisher", "9) Sort by Length");
		printf("\t%-20.20s\n", "10) Search for book");
		printf("\t%-15.15s\n", "0) Back");

		printf("\nEnter your choice:");
		scanf("%d", &ch);

		switch(ch) {
			case 1:
				if(i!=0)
					i--;
				break;
			case 2:
				if((i*20)+20<noofbooks)
					i++;
				break;
			case 3:
				printf("Enter book id:");
				scanf("%d", &id);
				if(id<noofbooks && id>=0)
					book_display(id);
				break;
			case 4:
				insertbook_display();
				break;
			case 5:	
				displayed_books = sorted_books('t');
				i=0;
				break;
			case 6:	
				displayed_books = sorted_books('a');
				i=0;
				break;
			case 7:	
				displayed_books = sorted_books('g');
				i=0;
				break;
			case 8:	
				displayed_books = sorted_books('p');
				i=0;
				break;
			case 9:	
				displayed_books = sorted_books('l');
				i=0;
				break;
			case 10:
				printf("\t1) Search by Title\t2) Search by Author\t3) Search by Genre\t4) Search by Publisher\n");
				printf("Enter your choice:");
				scanf("%d", &c);
				printf("Enter search phrase:");
				scanf("%49s", key);
				switch(c) {
					case 1:
						displayed_books = searched_books('t', key);
						break;
					case 2:
						displayed_books = searched_books('a', key);
						break;
					case 3:
						displayed_books = searched_books('g', key);
						break;
					case 4:
						displayed_books = searched_books('p', key);
						break;
				}
				i=0;
				break;
		}
	}
}

void book_display(int id) {
	int ch=99, dc=0;

	while(ch) {
		system("clear");

		printf("\n\n\tTitle: %s", books[id]->title);
		printf("\n\t\tBy: %s", books[id]->author);
		printf("\n\n\t\tGenre: %s", books[id]->genre);
		printf("\n\t\tPublisher: %s", books[id]->publisher);
		printf("\n\t\tLength: %d", books[id]->length);

		if(is_admin)
			printf("\n\n\t1) Delete Book");
		printf("\n\n\t0) Back");

		printf("\n\nEnter your choice:");
		scanf("%d", &ch);

		switch(ch) {
			case 1:
				if(is_admin) {
					printf("\nDelete book '%s'?", books[id]->title);
					printf("\n\t0) No\t1) Yes");

					printf("\n\nEnter you choice:");
					scanf("%d", &dc);
					if(dc) {
						if(delete_book(id))
							printf("Deleted the book!");
						else
							printf("Could not delete the book!");
					} else {
						continue;
					}

					fflush(stdout);
					sleep(2);
					return;
				}
		}
	}
}

void insertbook_display() {
	char title[50] = "", author[50] = "", genre[20] = "", publisher[50] = "";
	int length = 0, ic=0;

	while(title[0]=='\0') {
		system("clear");
		printf("\nEnter 0 to cancel");
		printf("\n\nEnter book title:");
		scanf("%s", title);
		if(strcmp(title, "0")==0)
			return;
	}

	system("clear");
	printf("\nEnter 0 to cancel");
	printf("\n\nTitle: %s", title);
	printf("\n\nEnter author name:");
	scanf("%s", author);
	if(strcmp(author, "0")==0)
		return;

	system("clear");
	printf("\nEnter 0 to cancel");
	printf("\n\nTitle: %s", title);
	printf("\nAuthor: %s", author);
	printf("\n\nEnter length:");
	scanf("%d", &length);
	if(!length)
		return;
	
	system("clear");
	printf("\nEnter 0 to cancel");
	printf("\n\nTitle: %s", title);
	printf("\nAuthor: %s", author);
	printf("\nLength: %d", length);
	printf("\n\nEnter genre:");
	scanf("%s", genre);
	if(strcmp(genre, "0")==0)
		return;	

	system("clear");
	printf("\nEnter 0 to cancel");
	printf("\n\nTitle: %s", title);
	printf("\nAuthor: %s", author);
	printf("\nLength: %d", length);
	printf("\nGenre: %s", genre);
	printf("\n\nEnter publisher:");
	scanf("%s", publisher);
	if(strcmp(publisher, "0")==0)
		return;	

	system("clear");
	printf("\nEnter 0 to cancel");
	printf("\n\nTitle: %s", title);
	printf("\nAuthor: %s", author);
	printf("\nLength: %d", length);
	printf("\nGenre: %s", genre);
	printf("\nPublisher: %s", publisher);

	printf("\n\nDo you want to insert this book?");
	printf("\n\t0) No\t1) Yes");
	printf("\n\nEnter your choice:");
	scanf("%d", &ic);

	if(!ic)
		return;

	if(insert_book(title, author, genre, publisher, length))
		printf("Inserted book sucessfully!");
	else
		printf("Could not insert book!");

	fflush(stdout);
	sleep(2);

}

book **sorted_books(char by) {
	book **new_books, *temp;
	
	new_books = (book **)malloc(MAX*sizeof(book *));

	int i, j, swap=0;
	for(i=0; i<noofbooks; i++) {
		new_books[i] = books[i];

		for(j=i; j>0; j--) {
			switch(by) {
				case 'l':
					if(new_books[j]->length < new_books[j-1]->length)
						swap = 1;
					break;
				case 't':
					if(strcmp(new_books[j]->title, new_books[j-1]->title)<0)
						swap = 1;
					break;
				case 'p':
					if(new_books[j]->publisher[0]!='\0' && (strcmp(new_books[j]->publisher, new_books[j-1]->publisher)<0) || new_books[j-1]->publisher[0]=='\0')
						swap = 1;
					break;
				case 'g':
					if(new_books[j]->genre[0]!='\0' && (strcmp(new_books[j]->genre, new_books[j-1]->genre)<0) || new_books[j-1]->genre[0]=='\0')
						swap = 1;
					break;
				case 'a':
					if(new_books[j]->author[0]!='\0' && (strcmp(new_books[j]->author, new_books[j-1]->author)<0) || new_books[j-1]->author[0]=='\0')
						swap = 1;
					break;
				case 's':
					if(new_books[j]->search_match > new_books[j-1]->search_match)
						swap = 1;
					break;
			}

			if (swap) {
				temp = new_books[j];
				new_books[j] = new_books[j-1];
				new_books[j-1] = temp;
				swap = 0;
			} else {
				break;
			}
		}
	}

	return new_books;
}

int search_string(char *str_og, char *key_og) {
	int i;
	char *result, str[100], key[100];

	i=0;
	while(1){
		str[i]=tolower(str_og[i]);
		if(str[i]=='\0')
			break;
		i++;
	}

	i=0;
	while(1){
		key[i]=tolower(key_og[i]);
		if(key[i]=='\0')
			break;
		i++;
	}

	result = strstr(str, key);

	if(result)
		return 1;
	else
		return 0;
}

book **searched_books(char by, char key[50]) {
	book **new_books, *temp;
	int i;

	new_books = (book **)malloc(MAX*sizeof(book *));

	for(i=0; i<noofbooks; i++){
		switch(by) {
			case 't':
				books[i]->search_match = search_string(books[i]->title, key);
				break;
			case 'a':
				books[i]->search_match = search_string(books[i]->author, key);
				break;
			case 'g':
				books[i]->search_match = search_string(books[i]->genre, key);
				break;
			case 'p':
				books[i]->search_match = search_string(books[i]->publisher, key);			
				break;
		}
	}

	new_books = sorted_books('s');

	for(i=0; i<noofbooks; i++)
		books[i]->search_match=0;

	return new_books;
}
