#include "utils.h"
uint16_t strtoint(const char *str)
{

    uint16_t num = 0;

    for (int i = 0; str[i] != '\0' && str[i] != '\n'; i++)
    {

        char c = str[i];
        if (c < '0' || c > '9')
            return -1;
        num = num * 10 + (c - '0');
        if (num > UINT16_MAX)
            return -1;
    }

    return num;
}
void get_input(const char *prompt, char *buf, size_t bufLen)
{
    if (prompt != NULL)
        write(STDOUT_FILENO, prompt, strlen(prompt));
    int rb = read(STDIN_FILENO, buf, bufLen);
    if (rb <= 0)
        exit(1);

    buf[rb - 1] = '\0';
}

void decode(char *msg, char **name, uint16_t *port, char **role, char **command, char **data)
{
    *role = strtok(msg, "|");
    *name = strtok(NULL, "|");
    *port = atoi(strtok(NULL, "|"));
    *command = strtok(NULL, "|");
    *data = strtok(NULL, "|");
}

void alarm_handler(int sig)
{
    return;
}

char *read_file()
{
    char *buf;
    int fd, nb, size, nbr;
    size = 100;
    nbr = 0;
    fd = open("recipes.json", O_RDONLY);
    if (fd == -1)
    {
        perror("Error opening file\n");
        exit(1);
    }
    buf = (char *)malloc((size) * sizeof(char));
    while ((nb = read(fd, buf + nbr, 100)) > 0)
    {
        if (nb == 100)
        {
            buf = (char *)realloc(buf, sizeof(char) * (size + 100));
            size += 100;
        }
        nbr += nb;
    }
    size = size + nb;
    buf[size] = '\0';

    return buf;
}
Recipe *read_json_file(int *num_recipes)
{
    Recipe *recipes;
    char *buf = read_file();
    cJSON *root = cJSON_Parse(buf);
    *num_recipes = cJSON_GetArraySize(root);
    recipes = (Recipe *)malloc(sizeof(Recipe) * (*num_recipes));
    for (int i = 0; i < *num_recipes; i++)
    {

        cJSON *obj = cJSON_GetArrayItem(root, i);
        recipes[i].num_ing = cJSON_GetArraySize(obj);
        recipes[i].ingredients = (Ingredient *)malloc(sizeof(Ingredient) * recipes[i].num_ing);
        strcpy(recipes[i].name, obj->string);
        int j;
        for (j = 0; j < recipes[i].num_ing; j++)
        {

            cJSON *prop = cJSON_GetArrayItem(obj, j);
            strcpy(recipes[i].ingredients[j].name, prop->string);
            recipes[i].ingredients[j].amount = prop->valueint;
        }
    }
    cJSON_Delete(root);
    return recipes;
}
