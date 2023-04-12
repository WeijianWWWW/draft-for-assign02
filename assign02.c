#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "assign02.h"

// The global variables used in the code
int sequence[];
char answer;

int correct_answers = 0;
int incorrect_answers = 0;

int level = 0;
int wins = 0;
int lives = 3;


// Declare the main assembly code entry point.
void main_asm();


// Initialise a GPIO pin – see SDK for detail on gpio_init()
void asm_gpio_init(uint pin)
{
    gpio_init(pin);
}


// Set direction of a GPIO pin – see SDK for detail on gpio_set_dir()
void asm_gpio_set_dir(uint pin, bool out)
{
    gpio_set_dir(pin, out);
}


// Get the value of a GPIO pin – see SDK for detail on gpio_get()
bool asm_gpio_get(uint pin)
{
    return gpio_get(pin);
}


// Set the value of a GPIO pin – see SDK for detail on gpio_put()
void asm_gpio_put(uint pin, bool value)
{
    gpio_put(pin, value);
}


// Enable falling-edge interrupt – see SDK for detail on gpio_set_irq_enabled()
void asm_gpio_set_irq(uint pin)
{
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, true);
}


// Read the input morse code
void read_morse_code(IDK_THE_TYPE_OF_INPUT){
    if(input != exit){
        add_morse_code(input, sequence);
    }
    else{
        printf("The sequence was: \n");
        printf("The translated input was: %s\n", sequence_to_string(answer));
        print_sequence_code(sequence);
        if(check_morse_code(sequence, answer)){
            printf("Correct!\n");
            wins++;
            lives++;
            correct_answers++;
            led better color
        }
        else{
            printf("Incorrect!\n");
            lives--;
            incorrect_answers++;
            led worse color
        }
        if (level == 2 || level == 4){
            printf("The Answer is: %s", sequence_to_string(answer));
        }
        if(lives == 0){
            printf("Game Over!\n");
            led = red
            end_the_game();
        }
        if(wins == 5){
            printf("Next Level!\n");
            level++;
            if(level > 4){
                printf("You have reached the end! Congratulations!\n");
                end_the_game();
            }
            wins = 0;
        }
        int sequence = [];
    }
}


// End the game and print the stats
void end_the_game(){
    printf("Number of correct answers: %d\n", correct_answers);
    printf("Number of incorrect answers: %d\n", incorrect_answers);
    printf("Number of lives left: %d\n", lives);
    printf("Accuracy: %d\n", (correct_answers / (correct_answers + incorrect_answers)) * 100);
}


// Add the input morse code to the sequence
void add_morse_code(int code, int sequence[])
{
    int output[sequence.length + 1];
    for (int i = 0; i < sequence.length; i++)
    {
        output[i] = sequence[i];
    }
    output[sequence.length] = code;
    return output;
}


// Check if the sequence morse code is correct
bool check_morse_code(int sequence[], int answer[]){
    if (sequence.length != answer.length){
        return false;
    }
    for (int i = 0; i < sequence.length; i++){
        if (sequence[i] != answer[i]){
            return false;
        }
    }
    return true;    
}

// Print the sequence morse code
void print_sequence_code(int sequence[]){
    printf(sequence_to_string(sequence));
    printf("\n");
}

// Convert the morse code to a character
char instance_to_char(int instance){
    if (instance == 0)
    {
        return '.';
    }
    else if(instance == 1)
    {
        return '-';
    }
    else if(instance == 2){
        return ' ';
    }
    else{
        return '';
    }
}


// Conver whole sequence to a string
char sequence_to_string(int sequence[]){
    char *output = "";
    for (int i = 0; i < sequence.length; i++){
        output += instance_to_char(sequence[i]);
    }
    return output;
}


// Compare the sequence to the answer
bool compare_string_to_string(char sequence, char answer){
    if(strcmp(sequence, answer) == 0){
        return true;
    }
    else{
        return false;
    }
}


// Main entry point of the application
int main()
{
    stdio_init_all();             // Initialise all basic IO
    printf("......Morse Code game start......\n"); // Basic print to console
    printf("Chose the difficulty\n")
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("Enter the sequence for the level difficutly number\n");
    // printf("3. Hard\n");
    // printf("4. Impossible\n");
    main_asm();                   // Jump into the ASM code
    return 0;                     // Application return code
}
