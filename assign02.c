#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"
#include "hardware/watchdog.h"
#include "answers.h"

// The global variables used in the code
char seq_empty[1] = {'1'}; // empty sequence

char *sequence; // pointer to the sequence

char *answer;
char *answer_name;

char *answers[26];
char *answer_names[26];

char *answer_word[28];
char *answer_word_names[28];

int correct_answers = 0;
int incorrect_answers = 0;

int level = 0;
int wins = 0;
int lives = 3;

int seqLen = 0;

// Declare the main assembly code entry point.
void main_asm();
bool check_morse_code(char *seq, char *answer);
char *sequence_to_string();
void addMorse(char input);
void change_led_color(int your_lives);
void print_level();
void generate_answer();
void initialise_answers();
void end_the_game();
void add_morse_code(char code);
char instance_to_char(int instance);
void newSequence();
char sequence_to_letters(char *seq);
void reserve_space();
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

void asm_gpio_set_irq_rise(uint pin)
{
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, true);
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
void asm_gpio_set_irq_fall(uint pin)
{
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_FALL, true);
}

void watchdog_init()
{
    if (watchdog_caused_reboot())
    {
        printf("\nNo input last 9 seconds. Rebooted.\n");
    }
    watchdog_enable(0x7fffff, 1); // enable the watchdog timer to max time (approx 8.3 secs)
    watchdog_update();
}

static inline void put_pixel(uint32_t pixel_grb)
{
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b)
{
    return ((uint32_t)(r) << 8) |
           ((uint32_t)(g) << 16) |
           (uint32_t)(b);
}

void change_led_color(int your_lives)
{
    switch (your_lives)
    {
    case 0:
        put_pixel(urgb_u32(0xFF, 0x00, 0x00)); // Set the RGB LED color to red
        break;
    case 1:
        put_pixel(urgb_u32(0xFF, 0xA5, 0x00)); // Set the RGB LED color to orange
        break;
    case 2:
        put_pixel(urgb_u32(0x00, 0x00, 0xFF)); // Set the RGB LED color to blue
        break;
    case 3:
        put_pixel(urgb_u32(0x00, 0xFF, 0x00)); // Set the RGB LED color to green
        break;
    default:

        break;
    }
}

// Read the input morse code
void addMorse(char input)
{
    if (input != '\0')
    {
        add_morse_code(input);
    }
    else
    {
        // TODO: start the game
        if (level == 0)
        {
            if (check_morse_code(sequence, answer_word[19]))
            {
                level = 1;
            }
            else if (check_morse_code(sequence, answer_word[20]))
            {
                level = 2;
            }
            // else if (check_morse_code(sequence, answer_word[21])){
            //     level = 3;
            // }
            // else if (check_morse_code(sequence, answer_word[22])){
            //     level = 4;
            // }
            else
            {
                printf("Invalid input!\n");
                printf("Please try again!\n");
                printf("The sequence was: ");
                for(int i = 0; i < seqLen; i++){
                    printf("%s \n", sequence[i]);
                }
                printf("\n");
                printf("The translated input was: %s \n", sequence_to_letters(sequence_to_string()));
                newSequence();
                return;
            }

            newSequence();
            print_level();
        }
        else
        {
            printf("The sequence was: %s\n", sequence_to_string());
            // printf("The translated input was: %s\n", sequence_to_string(answer));
            if (check_morse_code(sequence, answer))
            {
                printf("Correct!\n");
                wins++;
                if (lives < 3)
                {
                    lives++;
                }
                correct_answers++;
                change_led_color(lives);
            }
            else
            {
                printf("Incorrect!\n");
                printf("The translated input was: %s\n", sequence_to_letters(sequence_to_string()));
                lives--;
                incorrect_answers++;
                change_led_color(lives);
            }
            if (level == 2 || level == 4)
            {
                printf("The Answer sequence is: %s", answer);
            }
            if (lives == 0)
            {
                printf("Game Over!\n");
                change_led_color(lives);
                end_the_game();
                return;
            }
            if (wins == 5)
            {
                printf("Next Level!\n");
                level++;
                if (level > 4)
                {
                    printf("You have reached the end! Congratulations!\n");
                    end_the_game();
                    return;
                }
                wins = 0;
            }
            newSequence();
            print_level();
        }
    }
}

// new empty sequence and reset the length
void newSequence()
{
    free(sequence);
    // for(int i = 0; i < seqLen; i++){
    //     sequence[i] = '\0';
    // }
    seqLen = 0;

}
// print the level
void print_level()
{
    generate_answer();
    if (level == 1)
    {   
        printf("Current diffuculty: 1.Easy\n");
        printf("The task is: %s\n", answer_name);
        printf("The required input is: %s\n", answer);
    }
    else if (level == 2)
    {
        printf("Current diffuculty: 2.Medium\n");
        printf("The task is: %s\n", answer_name);
    }
    else if (level == 3)
    {
        printf("Current diffuculty: 3.Hard\n");
        printf("The task is: %s\n", answer_name);
        printf("The required input is: %s\n", answer);
    }
    else if (level == 4)
    {
        printf("Current diffuculty: 4.Impossible\n");
        printf("The task is: %s\n", answer_name);
    }
}

// Generate random answer
void generate_answer()
{
    time_t t;
    srand((unsigned)time(&t));
    if (level == 1 || level == 2)
    {
        int random = rand() % 26;
        answer = answers[random];
        answer_name = answer_names[random];
    }
    else if (level == 3 || level == 4)
    {
        int random = rand() % 28;
        answer = answer_word[random];
        answer_name = answer_word_names[random];
    }
}

// reserve more space for the sequence
void reserve_space()
{
    char *new_sequence = calloc((seqLen + 1), sizeof(char));
    for (int i = 0; i < seqLen; i++)
    {
        new_sequence[i] = sequence[i];
    }
    free(sequence);
    sequence = new_sequence;
}

// initialise answers[] and answer_names[] from the file answers.h
void initialise_answers()
{
    sequence = calloc(1, sizeof(char));
    answers[0] = A;
    answers[1] = B;
    answers[2] = C;
    answers[3] = D;
    answers[4] = E;
    answers[5] = F;
    answers[6] = G;
    answers[7] = H;
    answers[8] = I;
    answers[9] = J;
    answers[10] = K;
    answers[11] = L;
    answers[12] = M;
    answers[13] = N;
    answers[14] = O;
    answers[15] = P;
    answers[16] = Q;
    answers[17] = R;
    answers[18] = S;
    answers[19] = T;
    answers[20] = U;
    answers[21] = V;
    answers[22] = W;
    answers[23] = X;
    answers[24] = Y;
    answers[25] = Z;
    answer_names[0] = "A";
    answer_names[1] = "B";
    answer_names[2] = "C";
    answer_names[3] = "D";
    answer_names[4] = "E";
    answer_names[5] = "F";
    answer_names[6] = "G";
    answer_names[7] = "H";
    answer_names[8] = "I";
    answer_names[9] = "J";
    answer_names[10] = "K";
    answer_names[11] = "L";
    answer_names[12] = "M";
    answer_names[13] = "N";
    answer_names[14] = "O";
    answer_names[15] = "P";
    answer_names[16] = "Q";
    answer_names[17] = "R";
    answer_names[18] = "S";
    answer_names[19] = "T";
    answer_names[20] = "U";
    answer_names[21] = "V";
    answer_names[22] = "W";
    answer_names[23] = "X";
    answer_names[24] = "Y";
    answer_names[25] = "Z";

    answer_word[0] = TO;
    answer_word[1] = ME;
    answer_word[2] = MOM;
    answer_word[3] = NOW;
    answer_word[4] = DOT;
    answer_word[5] = DOG;
    answer_word[6] = CAT;
    answer_word[7] = DAD;
    answer_word[8] = TOM;
    answer_word[9] = WILSON;
    answer_word[10] = DIMA;
    answer_word[11] = KOSTYA;
    answer_word[12] = FISH;
    answer_word[13] = PICO;
    answer_word[14] = ALARM;
    answer_word[15] = IRELAND;
    answer_word[16] = TRINITY;
    answer_word[17] = COLLEGE;
    answer_word[18] = ZERO;
    answer_word[19] = ONE;
    answer_word[20] = TWO;
    answer_word[21] = THREE;
    answer_word[22] = FOUR;
    answer_word[23] = FIVE;
    answer_word[24] = SIX;
    answer_word[25] = SEVEN;
    answer_word[26] = EIGHT;
    answer_word[27] = NINE;
    answer_word_names[0] = "TO";
    answer_word_names[1] = "ME";
    answer_word_names[2] = "MOM";
    answer_word_names[3] = "NOW";
    answer_word_names[4] = "DOT";
    answer_word_names[5] = "DOG";
    answer_word_names[6] = "CAT";
    answer_word_names[7] = "DAD";
    answer_word_names[8] = "TOM";
    answer_word_names[9] = "WILSON";
    answer_word_names[10] = "DIMA";
    answer_word_names[11] = "KOSTYA";
    answer_word_names[12] = "FISH";
    answer_word_names[13] = "PICO";
    answer_word_names[14] = "ALARM";
    answer_word_names[15] = "IRELAND";
    answer_word_names[16] = "TRINITY";
    answer_word_names[17] = "COLLEGE";
    answer_word_names[18] = "ZERO";
    answer_word_names[19] = "ONE";
    answer_word_names[20] = "TWO";
    answer_word_names[21] = "THREE";
    answer_word_names[22] = "FOUR";
    answer_word_names[23] = "FIVE";
    answer_word_names[24] = "SIX";
    answer_word_names[25] = "SEVEN";
    answer_word_names[26] = "EIGHT";
    answer_word_names[27] = "NINE";
}
// End the game and print the stats
void end_the_game()
{
    printf("Number of correct answers: %d\n", correct_answers);
    printf("Number of incorrect answers: %d\n", incorrect_answers);
    printf("Number of lives left: %d\n", lives);
    printf("Accuracy: %d\n", (correct_answers / (correct_answers + incorrect_answers)) * 100);
}

char sequence_to_letters(char *seq)
{

    size_t seqLeng = strlen(seq);
    int i = (int)seqLeng - 1;
    int num_letters = 26;
    char output;
    for (int l = 0; l < i; l++)
    {
        char temp[5] = "";
        int k = 0;
        while (seq[l] != ' ' && seq[l] != '\0')
        {
            temp[k++] = seq[l++];
        }
        for (int j = 0; j < num_letters; j++)
        {
            if (strcmp(temp, answers[j]) == 0)
            {
                output += *answer_names[j];
                break;
            }
        }
    }
    return output;
}

// Add the input morse code to the sequence
void add_morse_code(char code)
{
    sequence[seqLen] = code;
    reserve_space();
    seqLen++;
    printf("%c", code);
    // if (sequence[0] == '1')
    // {
    //     char output[1];
    //     output[0] = code;
    //     sequence = output;
    //     seqLen = 1;
    //     return;
    // }
    // else
    // {
    //     char output[seqLen + 1];
    //     for (int i = 0; i < seqLen; i++)
    //     {
    //         output[i] = sequence[i];
    //     }
    //     output[seqLen] = code;
    //     sequence = output;
    //     seqLen++;
    //     return;
    // }
}

// Check if the sequence morse code is correct
bool check_morse_code(char *seq, char *answer)
{

    int answerLength = strlen(answer);
    if (seqLen != answerLength)
    {
        return false;
    }
    for (int i = 0; i < seqLen; i++)
    {
        if (seq[i] != answer[i])
        {
            return false;
        }
    }
    return true;
}

// Convert the morse code to a character
char instance_to_char(int instance)
{
    if (instance == 46)
    {
        return '.';
    }
    else if (instance == 45)
    {
        return '-';
    }
    else if (instance == 32)
    {
        return ' ';
    }
    else
    {
        return '\0';
    }
}

// Convert whole sequence to a string
char *sequence_to_string()
{
    char *output = malloc(seqLen + 1);
    for (int i = 0; i < seqLen; i++)
    {
        output[i] = instance_to_char(sequence[i]);
    }
    output[seqLen] = '\0';
    return output;    
}

// Main entry point of the application
int main()
{
    initialise_answers(); // Initialise the answers
    stdio_init_all();     // Initialise all basic IO
    watchdog_init();
    printf("......Morse Code game start......\n"); // Basic print to console

    printf("+---------------------------------------------------------------------------------------+\n");
    printf(":                                Assignment 2 by Group 30                               :\n");
    printf("+---------------------------------------------------------------------------------------+\n");
    printf("+---------------------------------------------------------------------------------------+\n");
    printf(":                                   Welcome to the                                      :\n");
    printf(":                                   Morse Code Game                                     :\n");
    printf("+---------------------------------------------------------------------------------------+\n");
    printf("The rules:\n");
    printf("Use GP21 button in order to enter Morse code sequence when asked\n");
    printf("If you get it correct you gain a life, if not - you lose one.\n");
    printf("LED is the indicator of lives' amount. Max lives - 3; when 0 is left - game over\n");
    printf("4 difficulty levels: \n");
    printf("1. Character and morse sequence displayed \n");
    printf("2. Only character is displayed \n");
    printf("3. Word and morse sequence displayed \n");
    printf("4. Only word is displayed \n");
    printf("To advance to the next level, get 5 right answers in a row.\n");
    printf("+---------------------------------------------------------------------------------------+\n");
    printf("You can choose the difficulty level by entering the sequence for the level number:\n");
    printf("1. Easy (hint: %s)\n", answer_word[19]);
    printf("2. Medium (hint: %s)\n", answer_word[20]);
    // printf("Enter the sequence for the level difficutly number\n");
    //  printf("3. Hard\n");
    //  printf("4. Impossible\n");
    watchdog_update();
    main_asm(); // Jump into the ASM code
    return 0;   // Application return code
}