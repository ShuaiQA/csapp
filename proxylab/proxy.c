#include "cache.h"
#include "csapp.h"

void *thread(void *vargp);

void sigpipe_hander(int sig);

void do_it(int fd);

int parse_uri(char *uri, char *host, char *port, char *path);

void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg);

static const char *user_agent_hdr =
    "user-agent: mozilla/5.0 (x11; linux x86_64; rv:52.0) gecko/20100101 "
    "firefox/52.0\r\n";
static const char *accept_hdr =
    "accept: "
    "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
static const char *accept_encoding_hdr = "accept-encoding: gzip, deflate\r\n";
static const char *language_hdr =
    "accept-language: zh,en-us;q=0.7,en;q=0.3\r\n";
static const char *connection_hdr = "connection: close\r\n";
static const char *pxy_connection_hdr = "proxy-connection: close\r\n";
static const char *secure_hdr = "upgrade-insecure-requests: 1\r\n";

Cache *cache;

int main(int argc, char **argv) {
  int listenfd, connfd;
  pthread_t tid;
  /* Check command line args */
  if (argc != 2) {
    fprintf(stderr, "usage: %s <port>\n", argv[0]);
    exit(1);
  }
  Signal(SIGPIPE, sigpipe_hander);
  //服务器开始设置监听端口号
  cache = (Cache *)Malloc(sizeof(Cache));
  init_cache(cache);
  listenfd = Open_listenfd(argv[1]);
  //每次循环都等待一个来自客户端的连接请求，输出已连接客户端的域名和 IP
  //地址，并调用 do_it 函数为这些客户端服务
  while (1) {
    //自己对accept函数又进行了封装,该函数会建立连接,并且输出客户端的地址和端口号
    connfd = Accept(listenfd);
    Pthread_create(&tid, NULL, thread, &connfd);
    // do_it(connfd);
  }
}

void sigpipe_hander(int sig) {
  sio_puts("sigpipe_hander catch");
  return;
}

void do_it(int fd) {
  size_t n;
  char method[MAXLINE], url[MAXLINE], version[MAXLINE];
  char buf[MAXLINE], host[MAXLINE], port[MAXLINE], path[MAXLINE];
  char buf_server[MAX_OBJ];
  rio_t rio, rio_server;
  int proxy;
  /* Read request line and headers */
  Rio_readinitb(&rio, fd);                //结构体和fd进行绑定
  if (!Rio_readlineb(&rio, buf, MAXLINE)) //将写入fd的数据写入buf中
    return;
  sscanf(buf, "%s %s %s", method, url, version);
  if (strcasecmp(method, "GET")) {
    clienterror(fd, method, "501", "Not Implemented",
                "Proxy does not implement this method");
    return;
  }

  if (!parse_uri(url, host, port, path)) {
    clienterror(fd, url, "404", "Not found", "Request could not be parsed");
    return;
  }
  if (fund_url(cache, url, buf_server, fd) == 1) {
    close(fd);
    return;
  }
  printf("没有找到");
  //没有找到那么需要访问远程
  printf("%s,%s,%s\n", host, port, path);
  proxy = Open_clientfd(host, port); //设置代理与服务器进行连接
  Rio_readinitb(&rio_server, proxy); //获取客户端发送的数据到rio_server
  //添加头部信息
  sprintf(buf_server, "GET %s HTTP/1.0\r\nHost: %s\r\n%s%s%s%s%s%s%s\r\n", path,
          host, user_agent_hdr, accept_hdr, language_hdr, accept_encoding_hdr,
          connection_hdr, secure_hdr, pxy_connection_hdr);
  Rio_writen(proxy, buf_server, strlen(buf_server));
  char *content = Malloc(sizeof(char) * MAX_OBJ);
  size_t content_len = 0;
  while ((n = Rio_readlineb(&rio_server, buf_server, MAXLINE)) != 0) {
    strncpy(content + content_len, buf_server, n);
    content_len += n;
    Rio_writen(fd, buf_server, n);
  }
  if (content_len < MAX_OBJ) {
    printf("content store cache\n");
    insert_node(cache, url, content, content_len);
  }
  close(proxy);
  Free(content);
  close(fd);
}

/*
解析url成三个部分
url: http://localhost:8080/home.html
host:localhost
port:8080
path:home.html
*/
int parse_uri(char *uri, char *host, char *port, char *path) {
  const char *ptr;
  const char *tmp;
  char scheme[10];
  int len;
  int i;

  ptr = uri;

  tmp = strchr(ptr, ':');
  if (NULL == tmp)
    return 0;

  len = tmp - ptr;
  (void)strncpy(scheme, ptr, len);
  scheme[len] = '\0';
  for (i = 0; i < len; i++)
    scheme[i] = tolower(scheme[i]);
  if (strcasecmp(scheme, "http"))
    return 0;
  tmp++;
  ptr = tmp;

  for (i = 0; i < 2; i++) {
    if ('/' != *ptr) {
      return 0;
    }
    ptr++;
  }

  tmp = ptr;
  while ('\0' != *tmp) {
    if (':' == *tmp || '/' == *tmp)
      break;
    tmp++;
  }
  len = tmp - ptr;
  (void)strncpy(host, ptr, len);
  host[len] = '\0';

  ptr = tmp;

  if (':' == *ptr) {
    ptr++;
    tmp = ptr;
    /* read port */
    while ('\0' != *tmp && '/' != *tmp)
      tmp++;
    len = tmp - ptr;
    (void)strncpy(port, ptr, len);
    port[len] = '\0';
    ptr = tmp;
  } else {
    port = "80";
  }

  if ('\0' == *ptr) {
    strcpy(path, "/");
    return 1;
  }

  tmp = ptr;
  while ('\0' != *tmp)
    tmp++;
  len = tmp - ptr;
  (void)strncpy(path, ptr, len);
  path[len] = '\0';

  return 1;
}

void *thread(void *vargp) {
  int connfd = *((int *)vargp);
  Pthread_detach(pthread_self());
  do_it(connfd);
  return NULL;
}

/*
 * clienterror - returns an error message to the client
 */
/* $begin clienterror */
void clienterror(int fd, char *cause, char *errnum, char *shortmsg,
                 char *longmsg) {
  char buf[MAXLINE];

  /* Print the HTTP response headers */
  sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "Content-type: text/html\r\n\r\n");
  Rio_writen(fd, buf, strlen(buf));

  /* Print the HTTP response body */
  sprintf(buf, "<html><title>Tiny Error</title>");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<body bgcolor="
               "ffffff"
               ">\r\n");
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "%s: %s\r\n", errnum, shortmsg);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<p>%s: %s\r\n", longmsg, cause);
  Rio_writen(fd, buf, strlen(buf));
  sprintf(buf, "<hr><em>The Tiny Web server</em>\r\n");
  Rio_writen(fd, buf, strlen(buf));
}
/* $end clienterror */
