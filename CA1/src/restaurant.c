#include "restaurant.h"

#include "udp.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include "utils.h"

void show_restaurants(Restaurant *rst, uint16_t port)
{
    int tcp_fd = connect_tcp_client(port);
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    sprintf(msg, "restaurant|%s|%d|show-restaurant|", rst->name, rst->port);
    send(tcp_fd, msg, MSG_BUF_LEN, 0);
    close(tcp_fd);
}

char *get_unique_name(Restaurant *rst)
{
    get_input(ENTER_NAME_INPUT, rst->name, NAME_SIZE);
    int duplicate = 0;
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    sprintf(buf, "restaurant|%s|%d|get-name|", rst->name, rst->port);
    sendto(rst->brcinfo.fd, buf, strlen(buf), 0, (struct sockaddr *)&rst->brcinfo.addr, sizeof(rst->brcinfo.addr));
    while (1)
    {
        signal(SIGALRM, alarm_handler);
        siginterrupt(SIGALRM, 1);
        memset(buf, '\0', MSG_BUF_LEN);
        alarm(1);
        int client_fd = accept_tcp_client(rst->tcpinfo.fd);
        alarm(0);
        if (recv(client_fd, buf, MSG_BUF_LEN, 0) < 0)
            break;
        char *name, *role, *command, *data;
        uint16_t port;
        decode(buf, &name, &port, &role, &command, &data);
        if (!strcmp(name, rst->name))
            duplicate = 1;
    }
    if (duplicate)
        return NULL;
    return rst->name;
}

void show_suppliers(Restaurant *rst)
{
    char buf[MSG_BUF_LEN];
    char written[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    sprintf(buf, "restaurant|%s|%d|get-supplier|", rst->name, rst->port);
    sendto(rst->brcinfo.fd, buf, strlen(buf), 0, (struct sockaddr *)&rst->brcinfo.addr, sizeof(rst->brcinfo.addr));
    while (1)
    {
        signal(SIGALRM, alarm_handler);
        siginterrupt(SIGALRM, 1);
        memset(buf, '\0', MSG_BUF_LEN);
        memset(written, '\0', MSG_BUF_LEN);
        alarm(1);
        int client_fd = accept_tcp_client(rst->tcpinfo.fd);
        alarm(0);
        if (recv(client_fd, buf, MSG_BUF_LEN, 0) < 0)
            break;
        char *name, *role, *command, *data;
        uint16_t port;
        decode(buf, &name, &port, &role, &command, &data);
        sprintf(written, "%s supplier with port %d\n", name, port);
        write(STDOUT_FILENO, written, MSG_BUF_LEN);
    }
}

void start_working(Restaurant *rst)
{
    char send[MSG_BUF_LEN];
    memset(send, '\0', MSG_BUF_LEN);
    rst->state = OPENED;
    sprintf(send, "restaurant|%s|%d|open-restaurant|", rst->name, rst->port);
    broadcast_msg(rst->brcinfo.fd, &rst->brcinfo.addr, send);
}

void end_working(Restaurant *rst)
{
    char send[MSG_BUF_LEN];
    memset(send, '\0', MSG_BUF_LEN);
    rst->state = CLOSED;
    sprintf(send, "restaurant|%s|%d|close-restaurant|", rst->name, rst->port);
    broadcast_msg(rst->brcinfo.fd, &rst->brcinfo.addr, send);
}

int send_request_ingredient(Restaurant *rst, uint16_t port)
{
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    int server_fd = connect_tcp_client(port);
    sprintf(buf, "restaurant|%s|%d|request-ingredient|", rst->name, rst->port);
    send(server_fd, buf, MSG_BUF_LEN, 0);
    return server_fd;
}

void add_ingredient(Restaurant *rst, char *name, int amount)
{

    int cnt = rst->num_ing;
    for (int i = 0; i < cnt; i++)
    {
        if (!strcmp(rst->ingredients[i].name, name))
        {
            rst->ingredients->amount += amount;
            return;
        }
    }
    if (rst->num_ing == 0)
        rst->ingredients = (Ingredient *)malloc(sizeof(Ingredient) * 1);
    else
        rst->ingredients = (Ingredient *)realloc(rst->ingredients, sizeof(Ingredient) * (cnt + 1));
    strcpy(rst->ingredients[cnt].name, name);
    rst->ingredients[cnt].amount = amount;
    rst->num_ing += 1;
}
void answer_ingredient(Restaurant *rst, int server_fd, char *name, uint16_t port, int amount)
{
    char buf[MSG_BUF_LEN], written[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    memset(written, '\0', MSG_BUF_LEN);

    signal(SIGALRM, alarm_handler);
    siginterrupt(SIGALRM, 1);
    alarm(90);
    int nb = recv(server_fd, buf, MSG_BUF_LEN, 0);
    alarm(0);
    if (nb <= 0)
    {
        write(STDOUT_FILENO, "Time Out!\n", strlen("Time Out!\n"));
        return;
    }
    char *name_s, *role, *command, *data;
    uint16_t port_s;
    decode(buf, &name_s, &port_s, &role, &command, &data);

    if (!strcmp(command, "reject-request"))
        sprintf(written, "%s rejected request!\n", name_s);
    else if (!strcmp(command, "busy-restaurant"))
        sprintf(written, "%s is busy!\n", name_s);
    else if (!strcmp(command, "accept-request"))
    {
        sprintf(written, "%s accepted request!\n", name_s);
        add_ingredient(rst, name, amount);
    }
    else
        return;
    write(STDOUT_FILENO, written, MSG_BUF_LEN);
}

void request_ingredient(Restaurant *rst)
{
    char name[NAME_SIZE], buf[MSG_BUF_LEN];
    uint16_t port;
    int amount, server_fd;
    memset(name, '\0', MSG_BUF_LEN);
    memset(buf, '\0', MSG_BUF_LEN);
    get_input("port of supplier: ", buf, MSG_BUF_LEN);
    port = strtoint(buf);
    get_input("name of ingredient: ", name, NAME_SIZE);
    get_input("number or ingredient: ", buf, MSG_BUF_LEN);
    amount = strtoint(buf);
    server_fd = send_request_ingredient(rst, port);
    answer_ingredient(rst, server_fd, name, port, amount);
    close(server_fd);
}

void send_name(Restaurant *rst, uint16_t port)
{
    int tcp_fd = connect_tcp_client(port);
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    sprintf(msg, "restaurant|%s|%d|send-name|", rst->name, rst->port);
    send(tcp_fd, msg, MSG_BUF_LEN, 0);
    close(tcp_fd);
}
void show_order_queue(Restaurant *rst)
{
    char buf[MSG_BUF_LEN];
    for (int i = 0; i < rst->num_order; i++)
    {
        memset(buf, '\0', MSG_BUF_LEN);
        if (rst->orders[i].state == WAITING)
        {
            sprintf(buf, "%s %d %s\n", rst->orders[i].name, rst->orders[i].port, rst->orders[i].food_name);
            write(STDOUT_FILENO, buf, MSG_BUF_LEN);
        }
    }
}
void show_order_history(Restaurant *rst)
{
    char buf[MSG_BUF_LEN];
    for (int i = 0; i < rst->num_order; i++)
    {
        memset(buf, '\0', MSG_BUF_LEN);
        if (rst->orders[i].state == REJECT)
            sprintf(buf, "%s %s rejected!\n", rst->orders[i].name, rst->orders[i].food_name);
        else
            sprintf(buf, "%s %s accepted!\n", rst->orders[i].name, rst->orders[i].food_name);
        write(STDOUT_FILENO, buf, MSG_BUF_LEN);
    }
}

void brinput(Restaurant *rst)
{

    char msg[MSG_BUF_LEN], written[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    memset(written, '\0', MSG_BUF_LEN);
    recv(rst->brcinfo.fd, msg, MSG_BUF_LEN, 0);
    char *name, *role, *command, *data;
    uint16_t port;
    decode(msg, &name, &port, &role, &command, &data);
    if (!strcmp(role, "restaurant") || (!strcmp(command, "get-restaurant") && rst->state == CLOSED))
        return;
    sprintf(written, "%s with port %d : %s", name, port, command);
    logMsg(rst->file_fd, written);
    if (!strcmp(command, "get-restaurant"))
        show_restaurants(rst, port);
    else if (!strcmp(command, "get-name"))
    {
        send_name(rst, port);
    }
    logNormal(rst->file_fd, "Done!");
}

void show_ingredients(Ingredient *ingredients, int *num_ing)
{
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    for (int i = 0; i < (*num_ing); i++)
    {
        sprintf(buf, "\n%s %d", ingredients[i].name, ingredients[i].amount);
        write(STDOUT_FILENO, buf, strlen(buf));
    }
    write(STDOUT_FILENO, "\n\n", strlen("\n\n"));
}
void show_recipes(Recipe *recipes, int *num_recipes)
{
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    for (int i = 0; i < (*num_recipes); i++)
    {
        write(STDOUT_FILENO, "\n", strlen("\n"));
        write(STDOUT_FILENO, recipes[i].name, strlen(recipes[i].name));
        write(STDOUT_FILENO, "\n", strlen("\n"));
        for (int j = 0; j < recipes[i].num_ing; j++)
        {
            write(STDOUT_FILENO, "     ", strlen("     "));
            write(STDOUT_FILENO, recipes[i].ingredients[j].name, strlen(recipes[i].ingredients[j].name));
            sprintf(buf, "   :  %d", recipes[i].ingredients[j].amount);
            write(STDOUT_FILENO, buf, strlen(buf));
            write(STDOUT_FILENO, "\n", strlen("\n"));
        }
        write(STDOUT_FILENO, "\n", strlen("\n"));
    }
}

int find_order(Restaurant *rst, uint16_t port)
{
    for (int i = 0; i < rst->num_order; i++)
    {
        if (rst->orders[i].port == port && rst->orders[i].state == WAITING)
            return i;
    }
    return -1;
}

int find_recipe(Recipe *recipes, int *num_recipes, char *name)
{
    for (int i = 0; i < *num_recipes; i++)
    {
        if (!strcmp(recipes[i].name, name))
            return i;
    }
    return -1;
}
void decrease_ingredient(Restaurant *rst, Ingredient ingredient)
{
    for (int i = 0; i < rst->num_ing; i++)
    {
        if (!strcmp(rst->ingredients[i].name, ingredient.name))
        {
            rst->ingredients[i].amount -= ingredient.amount;
        }
    }
}
int enough_ingredient(Restaurant *rst, Ingredient ingredient)
{
    for (int i = 0; i < rst->num_ing; i++)
    {

        if (!strcmp(rst->ingredients[i].name, ingredient.name))
            return rst->ingredients[i].amount >= ingredient.amount;
    }
    return 0;
}
int enough_ingredients(Restaurant *rst, Recipe *recipes, int *num_recipes, int index_order)
{
    int idx_rcp = find_recipe(recipes, num_recipes, rst->orders[index_order].food_name);
    if (idx_rcp == -1 || rst->num_ing == 0)
        return 0;

    for (int i = 0; i < recipes[idx_rcp].num_ing; i++)
    {
        if (!enough_ingredient(rst, recipes[idx_rcp].ingredients[i]))
            return 0;
        
    }

    for (int i = 0; i < recipes[idx_rcp].num_ing; i++)
        decrease_ingredient(rst, recipes[idx_rcp].ingredients[i]);
    return 1;
}

void answer_request(Restaurant *rst, Recipe *recipes, int *num_recipes, FDSet *fdset)
{
    char buf[MSG_BUF_LEN];
    memset(buf, '\0', MSG_BUF_LEN);
    get_input("port of request: ", buf, MSG_BUF_LEN);
    uint16_t port = strtoint(buf);
    memset(buf, '\0', MSG_BUF_LEN);
    get_input("your answer is (yes/no)? ", buf, MSG_BUF_LEN);
    int idx = find_order(rst, port);
    if (idx < 0)
    {
        write(STDOUT_FILENO, "port not found!\n", strlen("port not found!\n"));
        logError(rst->file_fd, "port not found!");
        return;
    }
    if (!strcmp(buf, "no"))
    {
        memset(buf, '\0', MSG_BUF_LEN);
        sprintf(buf, "restaurant|%s|%d|reject-food-request|", rst->name, rst->port);
        rst->orders[idx].state = REJECT;
    }
    else if (!strcmp(buf, "yes") && !enough_ingredients(rst, recipes, num_recipes, idx))
    {
        memset(buf, '\0', MSG_BUF_LEN);
        sprintf(buf, "restaurant|%s|%d|reject-food-request|", rst->name, rst->port);
        rst->orders[idx].state = REJECT;
        write(STDOUT_FILENO, "you don't have enough ingredients\n", 35);
        logNormal(rst->file_fd, "Not enough ingredient!");
    }
    else if (!strcmp(buf, "yes"))
    {
        memset(buf, '\0', MSG_BUF_LEN);
        sprintf(buf, "restaurant|%s|%d|accept-food-request|", rst->name, rst->port);
        rst->orders[idx].state = ACCEPT;
    }
    send(rst->orders[idx].fd, buf, MSG_BUF_LEN, 0);
    logNormal(rst->file_fd, "Done");
    clear_fd(rst->orders[idx].fd, fdset);

}

void clinput(Restaurant *rst, Recipe *recipes, int *num_recipes, FDSet *fdset)
{
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    read(STDIN_FILENO, msg, MSG_BUF_LEN);
    logInput(rst->file_fd, msg);
    if (rst->state == CLOSED && strcmp(msg, START_WORKING))
    {
        logError(rst->file_fd, "not allowed!(closed)");
        write(STDOUT_FILENO, "not allowed!(closed)\n", strlen("not allowed!(closed)\n"));
        return;
    }

    if (!strcmp(msg, START_WORKING) && rst->state == CLOSED)
        start_working(rst);

    if (!strcmp(msg, CLOSE_RESTAURANT))
        end_working(rst);

    else if (!strcmp(msg, SHOW_SUPPLIERS))
        show_suppliers(rst);

    else if (!strcmp(msg, REQUEST_INGREDIENT))
        request_ingredient(rst);

    else if (!strcmp(msg, SHOW_RECIPES))
        show_recipes(recipes, num_recipes);

    else if (!strcmp(msg, SHOW_INGREDIENTS))
        show_ingredients(rst->ingredients, &rst->num_ing);

    else if (!strcmp(msg, ANSWER_REQUEST))
    {
        answer_request(rst, recipes, num_recipes, fdset);
    }

    else if (!strcmp(msg, REQUEST_LIST))
        show_order_queue(rst);

    else if (!strcmp(msg, REQUEST_HISTORY))
        show_order_history(rst);
    logNormal(rst->file_fd, "Done!");
}
void add_order(Restaurant *rst, uint16_t port, int fd, char *name, char *food_name)
{

    if (rst->num_order == 0)
    {
        rst->orders = (OrderFood *)malloc(sizeof(OrderFood) * (1));
    }
    else
    {
        rst->orders = (OrderFood *)realloc(rst->orders, sizeof(OrderFood) * (rst->num_order + 1));
    }
    rst->orders[rst->num_order].fd = fd;
    rst->orders[rst->num_order].port = port;
    rst->orders[rst->num_order].state = WAITING;
    sprintf(rst->orders[rst->num_order].name, "%s", name);
    sprintf(rst->orders[rst->num_order].food_name, "%s", food_name);
    rst->num_order += 1;
    write(STDOUT_FILENO, NEW_ORDER_MSG, strlen(NEW_ORDER_MSG));
}
void tcpinput(Restaurant *rst, FDSet *fdset)
{
    int client_fd = accept_tcp_client(rst->tcpinfo.fd);
    char buf[MSG_BUF_LEN], written[MSG_BUF_LEN];
    memset(written, '\0', MSG_BUF_LEN);
    memset(buf, '\0', MSG_BUF_LEN);
    recv(client_fd, buf, MSG_BUF_LEN, 0);
    char *name, *role, *command, *data;
    uint16_t port;
    decode(buf, &name, &port, &role, &command, &data);
    sprintf(written, "%s with port %d : %s", name, port, command);
    logMsg(rst->file_fd, written);
    if (!strcmp(command, "order-food"))
    {
        add_order(rst, port, client_fd, name, data);
        set_fd(client_fd, fdset);
    }
    logNormal(rst->file_fd, "Done!");
}

void delete_order(Restaurant *rst, int fd)
{
    char msg[MSG_BUF_LEN];
    memset(msg, '\0', MSG_BUF_LEN);
    for (int i = 0; i < rst->num_order; i++)
    {
        if (rst->orders[i].fd == fd)
        {
            sprintf(msg, "%s canceled order!", rst->orders[i].name);
            logMsg(rst->file_fd, msg);
            rst->orders[i].state = REJECT;
            return;
        }
    }
}

void interface(Restaurant *rst, Recipe *recipes, int *num_recipes)
{
    char msg[MSG_BUF_LEN];

    FDSet fdset;
    fdset.max = 0;
    FD_ZERO(&fdset.master);
    set_fd(STDIN_FILENO, &fdset);
    set_fd(rst->brcinfo.fd, &fdset);
    set_fd(rst->tcpinfo.fd, &fdset);

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
                clinput(rst, recipes, num_recipes, &fdset);
            else if (i == rst->brcinfo.fd)
                brinput(rst);
            else if (i == rst->tcpinfo.fd)
                tcpinput(rst, &fdset);
            else
            {
                int nb = recv(i, msg, MSG_BUF_LEN, 0);
                if (nb <= 0)
                {
                    delete_order(rst, i);
                    clear_fd(i, &fdset);
                }
            }
        }
    }
}

int main(int argc, const char *argv[])
{
    Restaurant rest;
    rest.num_ing = 0;
    rest.num_order = 0;
    int num_recipes;
    Recipe *recipes = read_json_file(&num_recipes);
    rest.state = CLOSED;
    rest.port = strtoint(argv[1]);
    rest.brcinfo.fd = connect_udp(rest.port, &rest.brcinfo.addr);
    rest.port = find_unused_port();
    rest.tcpinfo.fd = connect_tcp_server(rest.port, 5);
    while (1)
    {
        if (get_unique_name(&rest) != NULL)
            break;
    }
    rest.file_fd = open_log_file(rest.name);
    write(STDOUT_FILENO, RESTAURANT_WELCOME, strlen(RESTAURANT_WELCOME));
    interface(&rest, recipes, &num_recipes);
    close(rest.file_fd);
}
