#include "supplier.h"


void show_suppliers(Supplier *spl, uint16_t port)
{
    int tcp_fd = connect_tcp_client(port);
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    sprintf(msg, "supplier|%s|%d|show-supplier|", spl->name, spl->port);
    send(tcp_fd, msg, MSG_BUF_LEN, 0);
    close(tcp_fd);
}
void brinput(Supplier *spl)
{
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    recv(spl->brcinfo.fd, msg, MSG_BUF_LEN, 0);
    char *name, *role, *command, *data;
    uint16_t port;
    decode(msg, &name, &port, &role, &command, &data);
    if (!strcmp(role, "supplier"))
        return;
    if (!strcmp(command, "get-supplier"))
        show_suppliers(spl, port);
    else if (!strcmp(command, "get-name"))
    {
        send_name(spl, port);
    }
}

void answer_request(Supplier *spl)
{

    if (spl->order != WAITING)
        return;
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);

    get_input("your answer is (yes/no)? ", msg, MSG_BUF_LEN);
    if (!strcmp(msg, "no"))
    {
        memset(msg, '\0', MSG_BUF_LEN);
        sprintf(msg, "supplier|%s|%d|reject-request|", spl->name, spl->port);
        spl->order = REJECT;
    }
    if (!strcmp(msg, "yes"))
    {
        memset(msg, '\0', MSG_BUF_LEN);
        sprintf(msg, "supplier|%s|%d|accept-request|", spl->name, spl->port);
        spl->order = ACCEPT;
    }

    send(spl->order_fd, msg, MSG_BUF_LEN, 0);
}

void send_name(Supplier *spl, uint16_t port)
{

    int tcp_fd = connect_tcp_client(port);
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    sprintf(msg, "supplier|%s|%d|send-name|", spl->name, spl->port);
    send(tcp_fd, msg, MSG_BUF_LEN, 0);
    close(tcp_fd);
}
char *get_unique_name(Supplier *spl)
{
    get_input(ENTER_NAME_INPUT, spl->name, NAME_SIZE);
    int duplicate = 0;
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    sprintf(buf, "supplier|%s|%d|get-name|", spl->name, spl->port);
    sendto(spl->brcinfo.fd, buf, strlen(buf), 0, (struct sockaddr *)&spl->brcinfo.addr, sizeof(spl->brcinfo.addr));
    while (1)
    {
        signal(SIGALRM, alarm_handler);
        siginterrupt(SIGALRM, 1);
        memset(buf, '\0', MSG_BUF_LEN);
        alarm(1);
        int client_fd = accept_tcp_client(spl->tcpinfo.fd);
        alarm(0);
        if (recv(client_fd, buf, MSG_BUF_LEN, 0) < 0)
            break;
        char *name, *role, *command, *data;
        uint16_t port;
        decode(buf, &name, &port, &role, &command, &data);
        if (!strcmp(name, spl->name))
            duplicate = 1;
    }
    if (duplicate)
        return NULL;
    return spl->name;
}

void clinput(Supplier *spl)
{
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    read(STDIN_FILENO, msg, MSG_BUF_LEN);

    if (spl->order == WAITING && !strcmp(msg, "answer request\n"))
        answer_request(spl);
}

void request_ingredient(Supplier *spl)
{
    spl->order = WAITING;
    write(STDOUT_FILENO, "new request!\n", strlen("new request!\n"));
}
void cancel_request(Supplier *spl)
{
    spl->order = REJECT;
}

void tcpinput(Supplier *spl, FDSet *fdset)
{

    int client_fd = accept_tcp_client(spl->tcpinfo.fd);
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    recv(client_fd, buf, MSG_BUF_LEN, 0);
    char *name, *role, *command, *data;
    uint16_t port;
    decode(buf, &name, &port, &role, &command, &data);
    if (spl->order == WAITING)
    {
        memset(buf, '\0', MSG_BUF_LEN);
        sprintf(buf, "supplier|%s|%d|busy-restaurant|", spl->name, spl->port);
        send(client_fd, buf, MSG_BUF_LEN, 0);
        return;
    }

    spl->order_fd = client_fd;
    if (!strcmp(command, "request-ingredient"))
    {
        memset(buf, '\0', MSG_BUF_LEN);
        request_ingredient(spl);
        sprintf(buf, "%s with port %d : %s", name, port, command);
        logMsg(spl->file_fd, buf);
        set_fd(spl->order_fd,fdset);
    }
    logNormal(spl->file_fd,"Done!");
}

void interface(Supplier *spl)
{
    char msg[MSG_BUF_LEN];

    FDSet fdset;
    fdset.max = 0;
    FD_ZERO(&fdset.master);
    set_fd(STDIN_FILENO, &fdset);
    set_fd(spl->brcinfo.fd, &fdset);
    set_fd(spl->tcpinfo.fd, &fdset);
    while (1)
    {

        write(STDOUT_FILENO, ">> ", 3);
        memset(msg, '\0', MSG_BUF_LEN);
        fdset.working = fdset.master;
        select(fdset.max + 1, &fdset.working, NULL, NULL, NULL);
        for (int i = 0; i <= fdset.max; ++i)
        {
            if (!FD_ISSET(i, &fdset.working))
                continue;

            if (i != STDIN_FILENO)
                write(STDOUT_FILENO, "\x1B[2K\r", 5);

            if (i == STDIN_FILENO)
                clinput(spl);

            else if (i == spl->brcinfo.fd)
                brinput(spl);

            else if (i == spl->tcpinfo.fd)
                tcpinput(spl,&fdset);
            else
            {
                int nb = recv(i, msg, MSG_BUF_LEN, 0);
                if (nb <= 0)
                {
                    cancel_request(spl);
                    clear_fd(i, &fdset);
                }
            }
        }
    }
}

int main(int argc, const char *argv[])
{
    Supplier spl;
    spl.port = strtoint(argv[1]);
    spl.brcinfo.fd = connect_udp(spl.port, &spl.brcinfo.addr);
    spl.port = find_unused_port();
    spl.tcpinfo.fd = connect_tcp_server(spl.port, 1);
    while (1)
    {
        if (get_unique_name(&spl) != NULL)
            break;
    }
    spl.file_fd = open_log_file(spl.name);
    write(STDOUT_FILENO, SUPPLIER_WELCOME, strlen(SUPPLIER_WELCOME));
    interface(&spl);
    close(spl.file_fd);
}
