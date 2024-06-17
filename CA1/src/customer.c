#include "customer.h"

char *get_unique_name(Customer *cst)
{
    get_input(ENTER_NAME_INPUT, cst->name, NAME_SIZE);
    int duplicate = 0;
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    sprintf(buf, "customer|%s|%d|get-name|", cst->name, cst->port);
    sendto(cst->brcinfo.fd, buf, strlen(buf), 0, (struct sockaddr *)&cst->brcinfo.addr, sizeof(cst->brcinfo.addr));
    while (1)
    {
        signal(SIGALRM, alarm_handler);
        siginterrupt(SIGALRM, 1);
        memset(buf, '\0', MSG_BUF_LEN);
        alarm(1);
        int client_fd = accept_tcp_client(cst->tcpinfo.fd);
        alarm(0);
        if (recv(client_fd, buf, MSG_BUF_LEN, 0) < 0)
            break;
        char *name, *role, *command, *data;
        uint16_t port;
        decode(buf, &name, &port, &role, &command, &data);
        if (!strcmp(name, cst->name))
            duplicate = 1;
    }
    if (duplicate)
        return NULL;
    return cst->name;
}
void show_restaurants(Customer *cst)
{
    char buf[MSG_BUF_LEN];
    char written[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    sprintf(buf, "customer|%s|%d|get-restaurant|", cst->name, cst->port);
    sendto(cst->brcinfo.fd, buf, strlen(buf), 0, (struct sockaddr *)&cst->brcinfo.addr, sizeof(cst->brcinfo.addr));
    while (1)
    {
        signal(SIGALRM, alarm_handler);
        siginterrupt(SIGALRM, 1);
        memset(buf, '\0', MSG_BUF_LEN);
        memset(written, '\0', MSG_BUF_LEN);
        alarm(1);
        int client_fd = accept_tcp_client(cst->tcpinfo.fd);
        alarm(0);
        if (recv(client_fd, buf, MSG_BUF_LEN, 0) < 0)
            break;
        char *name, *role, *command, *data;
        uint16_t port;
        decode(buf, &name, &port, &role, &command, &data);
        sprintf(written, "%s retaurant with port %d\n", name, port);
        write(STDOUT_FILENO, written, MSG_BUF_LEN);
    }
}

void send_name(Customer *cst, uint16_t port)
{
    int tcp_fd = connect_tcp_client(port);
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    sprintf(msg, "customer|%s|%d|send-name|", cst->name, cst->port);
    send(tcp_fd, msg, MSG_BUF_LEN, 0);
    close(tcp_fd);
}

void get_order_answer(char buf[MSG_BUF_LEN], int nb)
{
    char written[MSG_BUF_LEN];
    memset(written, '\0', MSG_BUF_LEN);
    if (nb <= 0)
    {
        write(STDOUT_FILENO, TIME_OUT_MSG, strlen(TIME_OUT_MSG));
        return;
    }
    char *name, *role, *command, *data;
    uint16_t port;
    decode(buf, &name, &port, &role, &command, &data);

    if (!strcmp(command, "accept-food-request"))
        sprintf(written, "%s %s", name, ACCEPT_FOOD_MSG);
    else if (!strcmp(command, "reject-food-request"))
        sprintf(written, "%s %s", name, REJECT_FOOD_MSG);
    write(STDOUT_FILENO, written, MSG_BUF_LEN);
}

void order_food(Customer *cst, int server_fd, char *food_name)
{
    write(STDIN_FILENO, "waiting for the restaurant's response\n", 38);
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    sprintf(buf, "customer|%s|%d|order-food|%s", cst->name, cst->port, food_name);
    send(server_fd, buf, MSG_BUF_LEN, 0);
    memset(buf, '\0', MSG_BUF_LEN);

    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    alarm(120);
    int nb = recv(server_fd, buf, MSG_BUF_LEN, 0);
    alarm(0);
    get_order_answer(buf, nb);
}

void submit_order_food(Customer *cst)
{
    char name[NAME_SIZE], buf[MSG_BUF_LEN];
    uint16_t port;
    memset(name, '\0', MSG_BUF_LEN);
    memset(buf, '\0', MSG_BUF_LEN);
    get_input(NAME_FOOD_INPUT, name, MSG_BUF_LEN);
    get_input(RES_PORT_INPUT, buf, NAME_SIZE);
    port = strtoint(buf);
    logInfo(cst->file_fd, name);
    logInfo(cst->file_fd, "from");
    logInfo(cst->file_fd, buf);
    int server_fd = connect_tcp_client(port);
    if (server_fd >= 0)
        order_food(cst, server_fd, name);
    else
        logError(cst->file_fd, "Error in connecting to server");
    close(server_fd);
}

void brinput(Customer *cst)
{
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    char written[MSG_BUF_LEN];
    memset(written, '\0', MSG_BUF_LEN);
    recv(cst->brcinfo.fd, msg, MSG_BUF_LEN, 0);
    char *name, *role, *command, *data;
    uint16_t port;
    decode(msg, &name, &port, &role, &command, &data);
    if (!strcmp(role, "customer"))
        return;
    sprintf(written, "%s with port %d : %s", name, port, command);
    logMsg(cst->file_fd, written);
    memset(written, '\0', MSG_BUF_LEN);
    if (!strcmp(command, "open-restaurant"))
    {
        sprintf(written, "%s %s", name, OPEN_RESTAURANT_MSG);
        write(STDOUT_FILENO, written, MSG_BUF_LEN);
    }
    else if (!strcmp(command, "get-name"))
    {
        send_name(cst, port);
    }
    else if (!strcmp(command, "close-restaurant"))
    {
        sprintf(written, "%s %s", name, CLOSE_RESTAURANT_MSG);
        write(STDOUT_FILENO, written, MSG_BUF_LEN);
    }
    logNormal(cst->file_fd, "Done!");
}

void show_menu(Recipe *recipes, int *num_food)
{
    char buf[MSG_BUF_LEN];
    write(STDOUT_FILENO, "\n", strlen("\n"));
    for (int i = 0; i < (*num_food); i++)
    {
        memset(buf, '\0', MSG_BUF_LEN);
        sprintf(buf, "%d. %s\n", i + 1, recipes[i].name);
        write(STDOUT_FILENO, buf, MSG_BUF_LEN);
    }
    write(STDOUT_FILENO, "\n", strlen("\n"));
}

void clinput(Customer *cst, Recipe *recipes, int *num_food)
{
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    read(STDIN_FILENO, msg, MSG_BUF_LEN);
    logInput(cst->file_fd, msg);
    if (!strcmp(msg, SHOW_RESTAURANTS))
        show_restaurants(cst);

    else if (!strcmp(msg, SHOW_MENU))
        show_menu(recipes, num_food);

    else if (!strcmp(msg, ORDER_FOOD))
    {
        submit_order_food(cst);
        return;
    }
    logNormal(cst->file_fd, "Done!");
}

void interface(Customer *cst, Recipe *recipes, int *num_food)
{
    char msg[MSG_BUF_LEN];

    FDSet fdset;
    fdset.max = 0;
    FD_ZERO(&fdset.master);
    set_fd(STDIN_FILENO, &fdset);
    set_fd(cst->brcinfo.fd, &fdset);
    set_fd(cst->tcpinfo.fd, &fdset);
    while (1)
    {
        write(STDOUT_FILENO, ">> ", 3);
        memset(msg, '\0', MSG_BUF_LEN);
        fdset.working = fdset.master;
        select(fdset.max + 1, &fdset.working, NULL, NULL, NULL);
        for (int i = 0; i <= fdset.max; i++)
        {
            if (!FD_ISSET(i, &fdset.working))
                continue;
            if (i != STDIN_FILENO)
                write(STDOUT_FILENO, "\x1B[2K\r", 5);
            if (i == STDIN_FILENO)
                clinput(cst, recipes, num_food);
            else if (i == cst->brcinfo.fd)
                brinput(cst);
        }
    }
}

int main(int argc, const char *argv[])
{
    Customer cst;
    int num_food;
    Recipe *recipes = read_json_file(&num_food);
    cst.port = strtoint(argv[1]);
    cst.brcinfo.fd = connect_udp(cst.port, &cst.brcinfo.addr);
    cst.port = find_unused_port();
    cst.tcpinfo.fd = connect_tcp_server(cst.port, 5);
    while (1)
    {
        if (get_unique_name(&cst) != NULL)
            break;
    }
    cst.file_fd = open_log_file(cst.name);
    write(STDOUT_FILENO, CUSTOMER_WELCOME, strlen(CUSTOMER_WELCOME));
    interface(&cst, recipes, &num_food);
    close(cst.file_fd);
}
