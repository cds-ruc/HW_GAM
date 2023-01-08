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

int sock_fd = -1;
int init_client_socket(const char *ip, int port) {
  if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    printf("Socket Create Failure, ip: %s, port: %d\n", ip, port);
    return -1;
  }

  struct sockaddr_in serv_addr;//首先要指定一个服务端的ip地址+端口，表明是向哪个服务端发起请求
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = inet_addr(ip);//注意，这里是服务端的ip和端口
  serv_addr.sin_port = htons(port);

  if (connect(sock_fd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    printf("[create_connect] Socket Connect Failure, ip: %s, port: %d\n", ip, port);
    return -1;
  }
  return 0;
}

int client_broadcast_send(int a, int b) {
  if (sock_fd== -1) {
    return -1;
  }
  char buf[20];
  size_t buf_len = 8;
  memcpy(buf, &a, 4);
  memcpy(buf + 4, &b, 4);
  ssize_t send_bytes = send(sock_fd, buf, buf_len, 0);
  if (send_bytes <= 0) {
    if (send_bytes == 0) { // 远端关闭 eof
      printf("[client_send] read eof!");
    }
    sock_fd = -1;
    return -1;
  } else {
    if (send_bytes != ssize_t(buf_len)) {
      printf("[client_send] send fail, send_bytes = %d, expected len: %d", send_bytes, buf_len);
      exit(1);
      sock_fd = -1;
      return -1;
    }
    printf("[client_send] Socket Send Server Success\n");
  }

  ssize_t len = read(sock_fd, &buf, sizeof(buf));
  if (len <= 0) {
    printf("[client_broadcast_recv] read fail, len = %d, errno = %d", len, errno);
    sock_fd = -1;
  }
  printf("remote result = %d\n", *(int *)buf);

  return 0;
}

int main() {
  init_client_socket("192.168.0.100", 8008);
  client_broadcast_send(5, 6);
  return 0;
}