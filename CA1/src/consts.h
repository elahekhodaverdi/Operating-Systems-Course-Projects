#ifndef CONSTS_H
#define CONSTS_H


#define NAME_SIZE 30
#define INGREDIENTS_SIZE 100
#define MSG_BUF_LEN 256
#define ROLE_SIZE 10
#define COMMAND_SIZE 64
#define DATA_SIZE 512


#define COMMAND_CUSTOMER_MSG '$' 
#define COMMAND_RESTAURANT_MSG '&' 
#define COMMAND_SUPPLIER_MSG '%' 


#define START_WORKING "start working\n"
#define CLOSE_RESTAURANT "break\n"
#define SHOW_INGREDIENTS "show ingredients\n"
#define SHOW_RECIPES "show recipes\n"
#define SHOW_SUPPLIERS "show suppliers\n"
#define SHOW_RESTAURANTS "show restaurants\n"
#define REQUEST_INGREDIENT "request ingredient\n"
#define REQUEST_LIST "show requests list\n"
#define REQUEST_HISTORY "show sales history\n"
#define ANSWER_REQUEST "answer request\n"
#define ORDER_FOOD "order food\n"
#define SHOW_MENU "show menu\n"


#define NEW_ORDER_MSG "new order!\n"
#define NEW_ING_REQ_MSG "new request ingredient!\n"
#define TIME_OUT_MSG "Time Out!\n"
#define OPEN_RESTAURANT_MSG "restaurant opened\n"
#define CLOSE_RESTAURANT_MSG "restaurant closed\n"
#define ACCEPT_FOOD_MSG "Restaurant accepted and your food is ready!\n"
#define REJECT_FOOD_MSG "Restaurant denied and cry about it!\n"

#define RES_PORT_INPUT "port of restaurant: "
#define NAME_FOOD_INPUT "name of food: "
#define ENTER_NAME_INPUT "Enter your name: "


#define CUSTOMER_WELCOME "Welcome as Customer\n"
#define SUPPLIER_WELCOME "Welcome as Supplier\n"
#define RESTAURANT_WELCOME "Welcome as Restaurant\n"

#define GET_ANSWER_REQUEST "your answer (yes/no): "
#define RESTAURANT "Restaurant"

#define RECIPES_FILE "recipes.json"

#endif