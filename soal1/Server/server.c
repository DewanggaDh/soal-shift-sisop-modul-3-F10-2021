#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include<pthread.h>
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
#define JUMLAH_THREAD BACKLOG + 1
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

pthread_t thread_id[JUMLAH_THREAD];

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

int setup_server(short, int);
int accept_new_connection(int);
void* handle_connection(void* );

int translate_request(const char* );
void register_handler(int);
void login_handler(int, bool*, char*, char*, int, int);
void add_handler(int, bool, const char*, const char* );
void download_handler(int, bool);
void delete_handler(int, bool, const char*, const char* );
void search_handler(int, bool);

off_t fsize(const char * );
void record_log(int, const char*, const char*, const char* );
void send_to_client(int, const void*, int);
void read_from_client(int, void*, int, int);
void split_string(char [][100], char [], const char []);
void check_thread(int, const char* );
void check(int, char* );

int main(int argc, char const *argv[]) {
  int server_socket = setup_server(SERVER_PORT, BACKLOG);
  int client_number = 0;

  mkdir("./FILES", 0777);

  while (true) {
    printf("\nListening...\n");
    int client_socket = accept_new_connection(server_socket);
    printf("\nConnected!!\n");

    check_thread(
      pthread_create(&thread_id[client_number++], NULL, handle_connection, &client_socket),
      "Error creating thread!!"
    );
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

void* handle_connection(void* argument) {
  int client_socket = (int) *((int* ) argument);

  char request[50];
  char id[50];
  char password[50];

  bool logged_in = false;
  bool keep_handling = true;

  while (keep_handling) {
    printf("Waiting request from %d...\n", client_socket);
    read_from_client(client_socket, request, sizeof(request), STRING);
    printf("Recieved request from %d: %s\n", client_socket, request);

    int translated_request = translate_request(request);

    switch (translated_request) {
      case REGISTER: 
        register_handler(client_socket); 
        break;

      case LOGIN: 
        login_handler(
          client_socket, &logged_in, id, password, 
          sizeof(id), sizeof(password)
        ); 
        break;

      case ADD: 
        add_handler(client_socket, logged_in, id, password); 
        break;

      case DELETE: 
        delete_handler(client_socket, logged_in, id, password); 
        break;

      case DOWNLOAD: 
        download_handler(client_socket, logged_in); 
        break;

      case SEE: 
      case FIND: 
        search_handler(client_socket, logged_in);
        break;

      case LOGOUT:
        keep_handling = false;
        break;

      default: 
        printf("Invalid request!\n"); 
        break;
    }

    printf("\n");
  }

  close(client_socket);
  return NULL;
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

void register_handler(int clien_socket) {
  char success_message[25] = "Account created";
  char id[50];
  char password[50];
  char account_data[100];

  printf("Reading id & password...\n");

  read_from_client(clien_socket, id, sizeof(id), STRING);
  read_from_client(clien_socket, password, sizeof(password), STRING);

  strcpy(account_data, id);
  strcat(account_data, ":");
  strcat(account_data, password);
  strcat(account_data, "\n");

  FILE* fp = fopen("./akun.txt", "a");
  fputs(account_data, fp);
  fclose(fp);

  send_to_client(clien_socket, success_message, STRING);
}

void login_handler(
  int client_socket, bool* logged_in, char* id, char* password, 
  int id_size, int password_size
) {
  send_to_client(client_socket, logged_in, BOOLEAN);
  if (*logged_in) return;

  char success_message[50] = "Now you're logged in!";
  char failed_message[50] = "Incorrect id or password!";

  read_from_client(client_socket, id, id_size, STRING);
  read_from_client(client_socket, password, password_size, STRING);

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
      *logged_in = true;
      break;
    }
  }

  fclose(account_file);

  *logged_in
    ? send_to_client(client_socket, success_message, STRING)
    : send_to_client(client_socket, failed_message, STRING);
}

void add_handler(int client_socket, bool logged_in, const char* id, const char* password) {
  send_to_client(client_socket ,&logged_in, BOOLEAN);

  if (!logged_in) return;

  int client_file_status;
  read_from_client(client_socket, &client_file_status, sizeof(client_file_status), INTEGER);

  if (client_file_status == NOT_FOUND) return;

  int CHUNK_SIZE = 1 * BYTES;
  int total_recieved_size = 0;
  int bytes_read;
  char chunk[CHUNK_SIZE];
  char record_data[1024];
  int file_size;

  char publisher[50];
  char tahun[10];
  char file_name[50];
  char file_path[PATH_MAX];

  read_from_client(client_socket, publisher, sizeof(publisher), STRING);
  read_from_client(client_socket, tahun, sizeof(tahun), STRING);
  read_from_client(client_socket, file_name, sizeof(file_name), STRING);

  sprintf(file_path, "./FILES/%s", file_name);

  FILE* database_file = fopen("./files.tsv", "a");
  FILE* destination_file = fopen(file_path, "wb");

  if (database_file == NULL || destination_file == NULL) {
    printf("Error open file!\n");
    return;
  }

  sprintf(record_data, "%s\t%s\t%s\n", file_path, tahun, publisher);
  fputs(record_data, database_file);

  read_from_client(client_socket, &file_size, sizeof(file_size), INTEGER);

  double x;
  double y = (double) file_size;

  /* Proses utama menerima file dari client */
  do {
    bzero(chunk, CHUNK_SIZE);
    read(client_socket, &bytes_read, sizeof(int));
    read(client_socket, chunk, bytes_read);
    fwrite(chunk, 1, bytes_read, destination_file);

    total_recieved_size += bytes_read;
    x = (double) total_recieved_size;

    printf("recieved: %.1lf%%\r", 100 * x / y);
  } while (bytes_read >= CHUNK_SIZE);

  fclose(database_file);
  fclose(destination_file);

  printf("downloaded: 100%%\n");
  printf("File (%.1lf MB) was recieved from client.\n", x / 1000000);

  record_log(ADD, file_name, id, password);
}

void download_handler(int client_socket, bool logged_in) {
  send_to_client(client_socket, &logged_in, BOOLEAN);

  if (!logged_in) return;

  bool file_available = false;
  FILE* database_file = fopen("./files.tsv", "r");
  char file_path[100];
  char information[256];
  char requested_file[100];
  char records[3][100];

  read_from_client(client_socket, requested_file, sizeof(requested_file), STRING);
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

  send_to_client(client_socket, &file_available, BOOLEAN);
  if (!file_available) return;

  FILE* source_file = fopen(file_path, "rb");

  if (source_file == NULL) {
    printf("Failed opening file!\n");
    return;
  }

  int CHUNK_SIZE = 1 * BYTES;
  int total_read_size = 0;
  unsigned char chunk[CHUNK_SIZE];
  int file_size = (int) fsize(file_path);

  send_to_client(client_socket, &file_size, INTEGER);

  double x;
  double y = (double) file_size;

  while (!feof(source_file)) {
    int bytes_read = fread(chunk, 1, CHUNK_SIZE, source_file);
    send(client_socket, &bytes_read, sizeof(int), 0);
    send(client_socket, chunk, bytes_read, 0);

    total_read_size += bytes_read;
    x = (double) total_read_size;

    printf("sent: %.1lf%%\r", 100 * x / y);
  }
  
  fclose(source_file);
  printf("sent: 100%%\n");
  printf("File (%d bytes) was successfully sent!\n", total_read_size);
}

void delete_handler(int client_socket, bool logged_in, const char* id, const char* password) {
  send_to_client(client_socket, &logged_in, BOOLEAN);

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

  read_from_client(client_socket, requested_file, sizeof(requested_file), STRING);
  strcpy(filename, requested_file);

  sprintf(file_path, "./FILES/%s", requested_file);
  sprintf(new_file_path, "./FILES/old-%s", requested_file);

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

  send_to_client(client_socket, &file_available, BOOLEAN);

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
  record_log(DELETE, filename, id, password);
}

void search_handler(int client_socket, bool logged_in) {
  send_to_client(client_socket, &logged_in, BOOLEAN);
  
  if (!logged_in) return;

  char information[256];
  bool keep_read = false;
  FILE* database_file = fopen("./files.tsv", "r");

  do {
    keep_read = fgets(information, 256, database_file);
    information[strcspn(information, "\n")] = 0;

    send_to_client(client_socket, &keep_read, BOOLEAN);
    
    if (!keep_read) break;
    
    send_to_client(client_socket, information, STRING);
  } while (keep_read);

  fclose(database_file);
}

off_t fsize(const char *filename) {
  struct stat st; 

  if (stat(filename, &st) == 0) return st.st_size;
  return -1; 
}

void record_log(int mode, const char* filename, const char* id, const char* password) {
  FILE* log = fopen("./running.log", "a");
  char log_format[256];
  bzero(log_format, 256);

  switch (mode) {
    case ADD:
      sprintf(log_format, "Tambah : %s (%s:%s)\n", filename, id, password);
      break;

    case DELETE:
      sprintf(log_format, "Hapus : %s (%s:%s)\n", filename, id, password);
      break;

    default:
      printf("Unknown log mode!\n");
      break;
  }

  fputs(log_format, log);
  fclose(log);
}

void send_to_client(int client_socket, const void* data, int mode) {
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

void read_from_client(int client_socket, void* data, int data_size, int mode) {
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

void check_thread(int error, const char* error_message) {
  const int NOT_OCCUR = 0;
  if (error == NOT_OCCUR) return;

  perror(error_message);
  exit(EXIT_FAILURE);
}

void check(int result, char* error_message) {
  const int ERROR = -1;
  if (result != ERROR) return;

  perror(error_message);
  exit(EXIT_FAILURE);
}
