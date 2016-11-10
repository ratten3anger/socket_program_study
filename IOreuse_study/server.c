#include "IO_reuse.h"


ssize_t readn(int fd, void *buf, size_t count){
    size_t nLeft = count;
    ssize_t nRead = 0;
    char *pBuf = (char *)buf;
    while (nLeft > 0){
        if ((nRead = read(fd, pBuf, nLeft)) < 0){
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else if (nRead == 0)
            return count-nLeft;

        nLeft -= nRead;
        pBuf += nRead;
    }
    return count;
}

ssize_t writen(int fd, const void *buf, size_t count){
    size_t nLeft = count;
    ssize_t nWritten = 0;
    char *pBuf = (char *)buf;
    while (nLeft > 0){
        if ((nWritten = write(fd, pBuf, nLeft)) < 0){
            if (errno == EINTR)
                continue;
            else
                return -1;
        }
        else if (nWritten == 0)
            continue;

        nLeft -= nWritten;
        pBuf += nWritten;
    }
    return count;
}


ssize_t recv_peek(int sockfd, void *buf, size_t len){
    while (1){
        int ret = recv(sockfd, buf, len, MSG_PEEK);
        if (ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}

ssize_t readline(int sockfd, void *buf, size_t maxline){
    int ret;
    int nRead = 0;
    int returnCount = 0;
    char *pBuf = (char *)buf;
    int nLeft = maxline;
    while (1){
        ret = recv_peek(sockfd, pBuf, nLeft);
        if (ret <= 0)
            return ret;
        nRead = ret;
        for (int i = 0; i < nRead; ++i)
            if (pBuf[i] == '\n'){

                ret = readn(sockfd, pBuf, i+1);
                if (ret != i+1)
                    exit(EXIT_FAILURE);
                return ret + returnCount;
            }

        ret = readn(sockfd, pBuf, nRead);;
        if (ret != nRead)
            exit(EXIT_FAILURE);
        pBuf += nRead;
        nLeft -= nRead;
        returnCount += nRead;
    }
    return -1;
}


int main(int argc,char *argv[]){

    struct sockaddr_in clientAddr;
	int listenfd,nReady,maxfd,connfd;

	setbuf(stdin,0);
	setbuf(stdout,0);

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1){
        perror("socket error");
    }

    //addr reuse
    int on = 1;
    if (setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on)) == -1){
        perror("setsockopt SO_REUSEADDR error");
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listenfd, (const struct sockaddr *)&addr, sizeof(addr)) == -1){
        perror("bind error");
    }

    if (listen(listenfd, BACKLOG) == -1){
        perror("listen error");
    }

    socklen_t addrLen;

    maxfd = listenfd;
    fd_set rset;
    fd_set allset;
    FD_ZERO(&rset);
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);

    //for connected clients
    int client[FD_SETSIZE];
    for (int i = 0; i < FD_SETSIZE; ++i){
        client[i] = -1;
    }
    int maxi = 0;

    while (1){
        rset = allset;
        nReady = select(maxfd+1, &rset, NULL, NULL, NULL);

        if (nReady == -1){
            if (errno == EINTR)
                continue;
            perror("select error");
        }
        else if (nReady == 0)
            continue;

        if (FD_ISSET(listenfd, &rset)){
            addrLen = sizeof(clientAddr);
            connfd = accept(listenfd, (struct sockaddr *)&clientAddr, &addrLen);
            if (connfd == -1)
                perror("accept error");

            int i;
            for (i = 0; i < FD_SETSIZE; ++i){
                if (client[i] < 0){
                    client[i] = connfd;
                    if (i > maxi)
                        maxi = i;
                    break;
                }
            }
            if (i == FD_SETSIZE){
                write(1,"[x]too many clients",strlen("too many clients"));
                exit(EXIT_FAILURE);
            }
			printf("[!]You got a connection: client's ip is:%s,port is %d\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port));		
            FD_SET(connfd, &allset);
            if (connfd > maxfd)
                maxfd = connfd;

            if (--nReady <= 0)
                continue;
        }
        for (int i = 0; i <= maxi; ++i)
            if ((client[i] != -1) && FD_ISSET(client[i], &rset)){
                char buf[512] = {0};
                char sendbuf[512] = {0};
                int readBytes = readline(client[i], buf, sizeof(buf));
                if (readBytes == -1){
                    perror("readline error");
                }
                else if (readBytes == 0){
                    write(1,"client connect closed...",strlen("client connect closed..."));
                    FD_CLR(client[i], &allset);
                    close(client[i]);
                    client[i] = -1;
                }

                sleep(1);
                printf("[*]Got msg from client : %s \n",buf);
                write(1,"[!]Say sth to client:",strlen("[!]Say sth to client:"));
                read(0,sendbuf,512);
                if (writen(client[i], sendbuf, strlen(sendbuf)) == -1){
                    perror("writen error");
                }

                if (--nReady <= 0)
                    break;
            }
    }
    close(listenfd);
}
