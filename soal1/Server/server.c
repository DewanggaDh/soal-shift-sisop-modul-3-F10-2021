#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <limits.h>
#include <libgen.h>

#define SERVER_PORT 8080
#define BYTES 1024
#define NOT_FOUND 404
#define BACKLOG 7
#define BOOLEAN 1
#define INTEGER 2
#define STRING 3
#define LOGOUT 1
#define REGISTER 2
#define LOGIN 3
#define ADD 4
#define DOWNLOAD 5
#define DELETE 6
#define SEE 7
#define FIND 8

int client_socket;
bool logged_in;
bool keep_handling;
char user_id[50];
char user_password[50];

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

int setup_server(short, int);
int accept_new_connection(int);
void handle_connection(int);

int translate_request(const char* );
void register_handler();
void logout_handler();
void login_handler();
void add_handler();
void download_handler();
void delete_handler();
void search_handler();

void save_session(char*, char* );
void clear_session();
void record_log(int, const char* );
void send_to_client(const void*, int);
void read_from_client(void*, int, int);
void split_string(char [][100], char [], const char []);
void check(int, char* );

int main(int argc, char const *argv[]) {
  int server_socket = setup_server(SERVER_PORT, BACKLOG);

  mkdir("./FILES", 0777);

  while (true) {
    printf("Waiting for connections...\n");
    client_socket = accept_new_connection(server_socket);
    printf("Connected!!\n\n");

    // Do whatever we want
    handle_connection(client_socket);
  }

  close(server_socket);
  return EXIT_SUCCESS;
}


int setup_server(short port, int backlog) {
  int opt = 1;
  int server_socket;
  SA_IN server_address;

  check(
    (server_socket = socket(AF_INET, SOCK_STREAM, 0)), 
    "Failed creating socket!"
  );

  check(
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)), 
    "Failed setting socket option!"
  );

  // Initialize the address struct
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_port = htons(port);

  check(
    bind(server_socket, (SA* ) &server_address, sizeof(server_address)),
    "Bind Failed!"
  );

  check(
    listen(server_socket, backlog),
    "Listend Failed!"
  );

  return server_socket;
}

int accept_new_connection(int server_socket) {
  int address_size = sizeof(SA_IN);
  int client_socket;
  SA_IN client_address;

  check(
    (client_socket = 
      accept(
        server_socket, 
        (SA* ) &client_address, 
        (socklen_t* ) &address_size
      )),
    "Accept Failed!"
  );

  return client_socket;
}

void handle_connection(int client_socket) {
  char request[50];

  keep_handling = true;
  logged_in = false;

  while (keep_handling) {
    printf("Waiting for request...\n");
    read_from_client(request, sizeof(request), STRING);
    printf("Recieved request: %s\n", request);

    switch (translate_request(request)) {
      case REGISTER: register_handler(); break;
      case LOGOUT: logout_handler(); break;
      case LOGIN: login_handler(); break;
      case ADD: add_handler(); break;
      case DELETE: delete_handler(); break;
      case DOWNLOAD: download_handler(); break;

      case SEE: 
      case FIND: 
        search_handler();     
        break;

      default: printf("Invalid request!\n"); break;
    }

    printf("\n");
  }

  close(client_socket);
}

int translate_request(const char* request) {
  if (strcmp(request, "logout") == 0) return LOGOUT;
  if (strcmp(request, "register") == 0) return REGISTER;
  if (strcmp(request, "login") == 0) return LOGIN;
  if (strcmp(request, "add") == 0) return ADD;
  if (strcmp(request, "download") == 0) return DOWNLOAD;
  if (strcmp(request, "delete") == 0) return DELETE;
  if (strcmp(request, "see") == 0) return SEE;
  if (strcmp(request, "find") == 0) return FIND;

  return -1;
}

void logout_handler() {
  keep_handling = false;
  clear_session();
}

void register_handler() {
  char success_message[25] = "Account created";
  char id[50];
  char password[50];
  char account_data[100];

  printf("Reading id & password...\n");

  read_from_client(id, sizeof(id), STRING);
  read_from_client(password, sizeof(password), STRING);

  strcpy(account_data, id);
  strcat(account_data, ":");
  strcat(account_data, password);
  strcat(account_data, "\n");

  FILE* fp = fopen("./akun.txt", "a");
  fputs(account_data, fp);
  fclose(fp);

  send_to_client(success_message, STRING);
}

void login_handler() {
  send_to_client(&logged_in, BOOLEAN);
  if (logged_in) return;

  char success_message[50] = "Now you're logged in!";
  char failed_message[50] = "Incorrect id or password!";
  char id[50];
  char password[50];

  read_from_client(id, sizeof(id), STRING);
  read_from_client(password, sizeof(password), STRING);

  FILE* account_file = fopen("./akun.txt", "r");
  char data[256];
  char account[2][100];

  if (account_file == NULL) {
    FILE* create_account_file = fopen("./akun.txt", "a");
    fclose(create_account_file);

    account_file = fopen("./akun.txt", "r");
  }

  /* Iterating akun.txt */
  while (fgets(data, 256, account_file)) {
    data[strcspn(data, "\n")] = 0;  /* remove '\n' character from fgets */
    split_string(account, data, ":");

    const int ID = 0;
    const int PASS = 1;

    if (strcmp(id, account[ID]) == 0 && strcmp(password, account[PASS]) == 0) {
      logged_in = true;
      save_session(id, password);
      break;
    }
  }

  fclose(account_file);

  logged_in 
    ? send_to_client(success_message, STRING)
    : send_to_client(failed_message, STRING);
}

void add_handler() {
  send_to_client(&logged_in, BOOLEAN);

  if (!logged_in) return;

  int client_file_status;
  read_from_client(&client_file_status, sizeof(client_file_status), INTEGER);

  if (client_file_status == NOT_FOUND) return;

  int CHUNK_SIZE = 1 * BYTES;
  int total_file_size = 0;
  int bytes_read;
  char chunk[CHUNK_SIZE];
  char record_data[1024];

  char publisher[50];
  char tahun[10];
  char file_name[50];
  char file_path[PATH_MAX];

  read_from_client(publisher, sizeof(publisher), STRING);
  read_from_client(tahun, sizeof(tahun), STRING);
  read_from_client(file_name, sizeof(file_name), STRING);

  sprintf(file_path, "./FILES/%s", file_name);

  FILE* database_file = fopen("./files.tsv", "a");
  FILE* destination_file = fopen(file_path, "wb");

  if (database_file == NULL || destination_file == NULL) {
    printf("Error open file!\n");
    return;
  }

  sprintf(record_data, "%s\t%s\t%s\n", file_path, tahun, publisher);
  fputs(record_data, database_file);

  do {
    read(client_socket, &bytes_read, sizeof(int));
    read(client_socket, chunk, bytes_read);
    total_file_size += bytes_read;
    fwrite(chunk, 1, bytes_read, destination_file);
    bzero(chunk, CHUNK_SIZE);
    printf("bytes read = %d\n", bytes_read);
  } while (bytes_read >= CHUNK_SIZE);

  fclose(database_file);
  fclose(destination_file);
  printf("%d bytes read from client.\n", total_file_size);

  record_log(ADD, file_name);
}

void download_handler() {
  send_to_client(&logged_in, BOOLEAN);

  if (!logged_in) return;

  bool file_available = false;
  FILE* database_file = fopen("./files.tsv", "r");
  char file_path[100];
  char information[256];
  char requested_file[100];
  char records[3][100];

  read_from_client(requested_file, sizeof(requested_file), STRING);
  sprintf(file_path, "./FILES/%s", requested_file);

  /* Iterating files.tsv */
  while (fgets(information, 256, database_file) != NULL) {
    information[strcspn(information, "\n")] = 0; /* remove '\n' character from fgets */
    split_string(records, information, "\t");

    if (strcmp(file_path, records[0]) == 0) {
      printf("File available: (%s) (%s)\n", file_path, records[0]);
      file_available = true;
      break;
    }
  }

  send_to_client(&file_available, BOOLEAN);
  if (!file_available) return;

  FILE* source_file = fopen(file_path, "rb");

  if (source_file == NULL) {
    printf("Failed opening file!\n");
    return;
  }

  int CHUNK_SIZE = 1 * BYTES;
  int total_file_size = 0;
  unsigned char chunk[CHUNK_SIZE];

  while (!feof(source_file)) {
    int bytes_read = fread(chunk, 1, CHUNK_SIZE, source_file);
    total_file_size += bytes_read;
    send(client_socket, &bytes_read, sizeof(int), 0);
    send(client_socket, chunk, bytes_read, 0);
    printf("bytes read = %d\n", bytes_read);
  }
  
  fclose(source_file);
  printf("File (%d bytes) was successfully sent!\n", total_file_size);
}

void delete_handler() {
  send_to_client(&logged_in, BOOLEAN);

  if (!logged_in) return;

  FILE* old_database_file = fopen("./files.tsv", "r");
  FILE* new_database_file = fopen("./new-files.tsv", "w");

  bool file_available = false;
  int requested_file_length = 0;
  char file_path[100];
  char new_file_path[100];
  char information[256];
  char copy_information[256];
  char requested_file[100];
  char filename[100];
  char records[3][100];

  read_from_client(requested_file, sizeof(requested_file), STRING);
  strcpy(filename, requested_file);

  // printf("deleted filename: %s\n", filename);

  sprintf(file_path, "./FILES/%s", requested_file);
  sprintf(new_file_path, "./FILES/old-%s", requested_file);

  // printf("deleted filename: %s\n", filename);

  /* Iterating files.tsv */
  while (fgets(information, 256, old_database_file) != NULL) {
    strcpy(copy_information, information);

    /* remove '\n' character from fgets */
    information[strcspn(information, "\n")] = 0;

    split_string(records, information, "\t");

    if (strcmp(file_path, records[0]) == 0) {
      printf("File available: (%s) (%s)\n", file_path, records[0]);
      file_available = true;
      continue;
    }

    fputs(copy_information, new_database_file);
  }

  // printf("deleted filename: %s\n", filename);

  send_to_client(&file_available, BOOLEAN);

  fclose(old_database_file);
  fclose(new_database_file);

  if (!file_available) {
    remove("./new-files.tsv");
    return;
  }

  remove("./files.tsv");
  rename("./new-files.tsv", "./files.tsv");
  rename(file_path, new_file_path);

  strcpy(filename, basename(file_path));
  record_log(DELETE, filename);
}

void search_handler() {
  send_to_client(&logged_in, BOOLEAN);
  
  if (!logged_in) return;

  char information[256];
  bool keep_read = false;
  FILE* database_file = fopen("./files.tsv", "r");

  do {
    keep_read = fgets(information, 256, database_file);
    information[strcspn(information, "\n")] = 0;

    send_to_client(&keep_read, BOOLEAN);
    
    if (!keep_read) break;
    
    send_to_client(information, STRING);
  } while (keep_read);

  fclose(database_file);
}

void save_session(char* id, char* password) {
  strcpy(user_id, id);
  strcpy(user_password, password);
}

void clear_session() {
  bzero(user_id, 50);
  bzero(user_password, 50);
}

void record_log(int mode, const char* filename) {
  FILE* log = fopen("./running.log", "a");
  char log_format[256];
  bzero(log_format, 256);

  switch (mode) {
    case ADD:
      sprintf(log_format, "Tambah : %s (%s:%s)\n", filename, user_id, user_password);
      break;

    case DELETE:
      sprintf(log_format, "Hapus : %s (%s:%s)\n", filename, user_id, user_password);
      break;

    default:
      printf("Unknown log mode!\n");
      break;
  }

  fputs(log_format, log);
  fclose(log);
}

void send_to_client(const void* data, int mode) {
  int length = 0;

  switch (mode) {
    case BOOLEAN:
      send(client_socket, data, sizeof(bool), 0);
      return;
      
    case INTEGER:
      send(client_socket, data, sizeof(int), 0);
      return;

    case STRING:
      length = strlen(data);
      send(client_socket, &length, sizeof(int), 0);
      send(client_socket, data, length, 0);
      return;

    default:
      printf("Unknown sending mode!\n");
  }
}

void read_from_client(void* data, int data_size, int mode) {
  int length = 0;

  switch (mode) {
    case BOOLEAN:
      read(client_socket, data, data_size);
      return;
      
    case INTEGER:
      read(client_socket, data, data_size);
      return;

    case STRING:
      bzero(data, data_size);
      read(client_socket, &length, sizeof(int));
      read(client_socket, data, length);
      return;

    default:
      printf("Unknown sending mode!\n");
  }
}

void split_string(char splitted[][100], char origin[], const char delimiter[]) {
  int i = 0;
  char* token = strtok(origin, delimiter);
  
  while (token != NULL) {
    strcpy(splitted[i++], token);
    token = strtok(NULL, " ");
  }
}

void check(int result, char* error_message) {
  const int ERROR = -1;
  if (result != ERROR) return;

  perror(error_message);
  exit(EXIT_FAILURE);
}
