//
// Created by lianyu on 2023/1/4.
//

#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <exception>
#include <sstream>
#include <iomanip>
#include <string.h>
#include <unistd.h>

enum class RequestType : uint8_t {
  SYNC_INIT,
  SYNC_DEINIT,
  NONE
};

const int MAX_LISTEN_CONN = 512;
struct s_info {
  sockaddr_in s_addr;
  int fd;
  int peer_idx;
} ts[MAX_LISTEN_CONN];

const int BUFSIZE = 1024;

const int worker_count = 0;

bool serverSyncInit() {
  return true;
}

bool serverSyncDeinit() {

}

int plus(int a, int b) {
  return a + b;
}

void *connect_client(void *arg) {
  struct s_info *ts = (struct s_info *)arg;
  ssize_t size_len = 0;
  char buf[BUFSIZE];
  while (1) {
    size_len = read(ts->fd, buf, BUFSIZE);
    if (size_len <= 0) {
      if (size_len == 0) {
        printf("[connect_client] close");
      } else {
        printf("[connect_client] read error, errno = %d", errno);
      }
      close(ts->fd);
      pthread_exit(NULL);
    }
    RequestType request_type = *(RequestType *)buf;
    //判断同步启动 || 关闭
    if (request_type == RequestType::SYNC_INIT || request_type == RequestType::SYNC_DEINIT) {
      bool result;
      if (request_type == RequestType::SYNC_INIT) {
        result = serverSyncInit();
      } else {
        result = serverSyncDeinit();
      }
      int writen_bytes = write(ts->fd, &result, sizeof(result));
      if (writen_bytes < 0) {
        printf("[connect_client] server_socket sync error, errno = %d", errno);
      } else if (writen_bytes != 1) {
        printf("[connect_client] sync write fail, writen_bytes = %d, expected = %d", writen_bytes, 1);
      }
      continue;
    } else {
      int result = plus(*(int *)buf, *(int *)(buf + 4));
      int writen_bytes = write(ts->fd, &result, sizeof(result));
      if (writen_bytes < 0) {
        printf("[connect_client] server_socket sync error, errno = %d", errno);
      } else if (writen_bytes != 1) {
        printf("[connect_client] sync write fail, writen_bytes = %d, expected = %d", writen_bytes, 1);
      }
      size_len = read(ts->fd, buf, BUFSIZE);
      if (size_len <= 0) {
        if (size_len == 0) {
          printf("[connect_client] close");
        } else {
          printf("[connect_client] read error, errno = %d", errno);
        }
        close(ts->fd);
        pthread_exit(NULL);
      }
    }
  }
}

static void my_server_run(const char *ip, int port) {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket < 0) {
    printf("server_socket create error");
  } else {
    printf("server_socket create success");
  }
  int opt = 1;
  if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (void *)&opt, sizeof(opt)) < 0) {
    printf("set socket error");
  } else {
    printf("set socket success");
  }
  int on = 1;
  if (setsockopt(server_socket, IPPROTO_TCP, TCP_NODELAY, (char *)&on, sizeof(int)) < 0) {
    printf("set socket Close Nagle  error");
  }

  sockaddr_in addr, client;
  socklen_t len = sizeof(client);

  addr.sin_addr.s_addr =  htonl(INADDR_ANY);
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);

  if (bind(server_socket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
    printf("bind socket error, ip %s", ip);
  } else {
    printf("bind socket success");
  }

  if (listen(server_socket, 127) < 0) {
    printf("listen socket error, ip %s", ip);
  } else {
    printf("listen socket success");
  }

  int i = 0;
  while (1) {
    if (i >= MAX_LISTEN_CONN) {
      printf("listen fd exceed max listen num %d\n", MAX_LISTEN_CONN);
    }
    int client_fd = accept(server_socket, (struct sockaddr*)&client, &len);
    if (client_fd < 0) {
      printf("accept socket error, ip %s", ip);
    } else {
      printf("accept socket success, ip %s", ip);
    }
    ts[i].s_addr = client;
    ts[i].fd = client_fd;
    ts[i].peer_idx = -1;

    pthread_t tid;
    pthread_create(&tid, NULL, connect_client, (void *)&ts[i]);
    pthread_detach(tid);
    i++;
  }
}


int main() {
  my_server_run("192.168.0.100", 8008);
  return 0;
}