#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "assign02.h"
#include <time.h>
#include "ws2812.pio.h"
#include "hardware/watchdog.h"

char  current_character;
char* current_morse_string;
int morse_index=0;
int current_level;
int life;
char current_char;

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return  ((uint32_t) (r) << 8)  |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

void set_colour(uint8_t r, uint8_t g, uint8_t b){
    put_pixel(urgb_u32(r, g, b));
}

void asm_gpio_init(uint pin){
    gpio_init(pin);
}

void asm_gpio_set_dir(uint pin,bool out)
{
    gpio_set_dir(pin,out);
}

bool asm_gpio_get(uint pin) {
 return gpio_get(pin);
}

void asm_gpio_put(uint pin, bool value) {
    gpio_put(pin, value);
}

void asm_gpio_set_irq_fall(uint pin) {
     gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, true);
}

void asm_gpio_set_irq_rise(uint pin) {
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, true);
}
void asm_watchdog_update()
{
    watchdog_update();
}



char* find_morse()
{

}


void addString(char char_toAdd)
{
    current_morse_string[morse_index]=char_toAdd;
    printf("%c",char_toAdd);
    morse_index++;

}
bool compare_string(int input_length,int pattern_length,char* input_morse,char* pattern_morse)
{
   if(input_length!=pattern_length)       
   {
    return false;
   }
   for(int index=0;index<pattern_length;index++)
   {
    if(input_morse[index]!=pattern_morse[index])
    {
        return false;
    }
   }
   return true;
}
void change_life(bool upanddown)
{
  if(upanddown == true) 
  {
    if(life<3)
    {
        life++;
    }
  }
  else 
  {
    if(life>0)
    {
        life--;
    }
  }
   if(life==3)set_color(3);
   else if(life==2)set_color(2);
   else if(life==1)set_color(1);
}


void clear()
{
     morse_index=0;
    for(int i=0;i<50;i++)
    {
        current_morse_string[i]=0;
    }
}



char Level_1()
{
    char current_char;
    int random=rand()%36;
    if(random == 0) {
            current_char = 'A';
        }
        else if(random == 1) {
            current_char = 'B';
        }
        else if(random == 2) {
            current_char = 'C';
        }
        else if(random == 3) {
            current_char = 'D';
        }
        else if(random == 4) {
            current_char = 'E';
        }
        else if(random == 5) {
            current_char = 'F';
        }
        else if(random == 6) {
            current_char = 'G';
        }
        else if(random == 7) {
            current_char = 'H';
        }
        else if(random == 8) {
            current_char = 'I';
        }
        else if(random == 9) {
            current_char = 'J';
        }
        else if(random == 10) {
            current_char = 'K';
        }
        else if(random == 11) {
            current_char = 'L';
        }
        else if(random == 12) {
            current_char = 'M';
        }
        else if(random == 13) {
            current_char = 'N';
        }
        else if(random == 14) {
            current_char = 'O';
        }
        else if(random == 15) {
            current_char = 'P';
        }
        else if(random == 16) {
            current_char = 'Q';
        }
        else if(random == 17) {
            current_char = 'R';
        }
        else if(random == 18) {
            current_char = 'S';
        }
        else if(random == 19) {
            current_char = 'T';
        }
        else if(random == 20) {
            current_char = 'U';
        }
        else if(random == 21) {
            current_char = 'V';
        }
        else if(random == 22) {
            current_char = 'W';
        }
        else if(random == 23) {
            current_char = 'X';
        }
        else if(random == 24) {
            current_char = 'Y';
        }
        else if(random == 25) {
            current_char = 'Z';
        }
        else if(random == 26) {
            current_char = '0';
        }
        else if(random == 27) {
            current_char = '1';
        }
        else if(random == 28) {
            current_char = '2';
        }
        else if(random == 29) {
            current_char = '3';
        }
        else if(random == 30) {
            current_char = '4';
        }
        else if(random == 31) {
            current_char = '5';
        }
        else if(random == 32) {
            current_char = '6';
        }
        else if(random == 33) {
            current_char = '7';
        }
        else if(random == 34) {
            current_char = '8';
        }
        else if(random == 35) {
            current_char = '9';
        }
        else 
        {
            current_char = '?';
            printf("Invalid!\n");
        }
        printf("The next character is %c\n",current_char);
        return current_char;
}

char Level_2(){
    char current_char;
    int random = rand()%18;
 if(random ==0){
 current_char = 'TO';
 
 }else if(random ==1){
 current_char = 'ME';
 
 }else if(random==2){
 current_char = 'MOM';
  
}else if(random==3){
 current_char = 'NOW';
 
 }else if(random==4){ 
 current_char = 'DOT';
  
 }else if(random==5){
current_char =  'DOG';
  
 }else if(random==6){
 current_char = 'CAT';
  
 }else if(random==7){
 current_char = 'DAD';
  
}else if(random==8){
 current_char = 'TOM';
  
 }else if(random==9){
 current_char = 'WILSON';
  
 }else if(random==10){
 current_char = 'DIMA';
  
 }else if(random==11){
 current_char = 'KOSTYA';
  
 }else if(random==12){

 current_char = 'FISH';
  
 }else if(random==13){
 current_char = 'PICO';
  
 }else if(random==14){
 current_char = 'ALARM';
  
 }else if(random==15){

 current_char = 'IRELAND';
 
 }else if(random==16){ 
 current_char = 'TRINITY';
 
 }else if(random==17){
 current_char = 'COLLEGE';
 
 }else{
 current_char = '?';
            printf("Invalid!\n");
        }
        printf("The next character is %c\n",current_char);
        return current_char;
}








void main_asm();

/*
 * Main entry point for the code - simply calls the main assembly function.
 */
int main() {

    stdio_init_all();
    current_level=0;
    srand(time(NULL));            //generate a seed here
    current_morse_string = (char*) calloc(50,sizeof(char));
    if (watchdog_caused_reboot()) {
        printf("Time out! Game has been reset\n\n");
    }

    printf("██     ██ ███████ ██       ██████  ██████  ███    ███ ███████  \n");
    printf("██     ██ ██      ██      ██      ██    ██ ████  ████ ██       \n");
    printf("██  █  ██ █████   ██      ██      ██    ██ ██ ████ ██ █████    \n");
    printf("██ ███ ██ ██      ██      ██      ██    ██ ██  ██  ██ ██         \n");
    printf(" ███ ███  ███████ ███████  ██████  ██████  ██      ██ ███████  \n");
    printf("                                                                 \n");
    printf("                                                                 \n");
    printf(" ██████  ██████   ██████  ██    ██ ██████      ██████   ██████    \n");
    printf("██       ██   ██ ██    ██ ██    ██ ██   ██         ██   ██  ██   \n");
    printf("██   ███ ██████  ██    ██ ██    ██ ██████      ██████   ██  ██    \n");
    printf("██    ██ ██   ██ ██    ██ ██    ██ ██              ██   ██  ██    \n");
    printf(" ██████  ██   ██  ██████   ██████  ██          ██████  ███████     \n");
    printf("\nInstructions on how to Play:\n");
    printf("An alphanumeric character (A-Z/0-9) will be presented to you\n");
    printf("Enter the correct Morse Code sequence to progress");
    printf("Please choose a level for the game:");

    printf("Level #1: Individual characters with their equivalent Morse code provided.\n");
    printf("Level #2: Individual characters without their equivalent Morse code provided.\n");
    printf("Level #3: Individual words with their equivalent Morse code provided.\n");
    printf("Level #4: Individual words without their equivalent Morse code provided.");
    main_asm();


    return(0);
}
