#include "hardware/regs/addressmap.h"
#include "hardware/regs/io_bank0.h"
#include "hardware/regs/timer.h"
#include "hardware/regs/m0plus.h"
.syntax unified
.cpu cortex-m0plus
.thumb
.global main_asm
.align 4
.equ  DFLT_TIME,0
.equ DFLT_STATE_STRT, 1 @ Specify the value to start flashing
.equ DFLT_STATE_STOP, 0 @ Specify the value to stop flashing
.equ GPIO_BTN_FALL_MSK, 0x00400000 @ Bit-22 for falling-edge event on GP21
.equ GPIO_BTN_RISE_MSK, 0x00800000 @ Bit-23 for rising-edge event on GP21
.equ GPIO_BTN, 21 @ Specify pin for the button
.equ GPIO_LED_PIN, 25 @ Specify pin for the built-in LED
.equ GPIO_DIR_IN, 0 @ Specify input direction for a GPIO pin
.equ GPIO_DIR_OUT, 1 @ Specify output direction for a GPIO pin
.equ GPIO_ISR_OFFSET, 0x74 @ GPIO is int #13 (vector table entry 29)
.equ ONE_SECOND, 500000  
.equ ONEHALF_SECOND,1500000   
.equ TWO_SECONDS, 2000000 

.equ ALRM_ISR_OFFSET, 0x40      
.equ DFLT_ALARM_TIME, 3000000   

.equ MORSE_MEMORY, 0x10000010   //Address where the morse string will be stored


@ Entry point to the ASM portion of the program
main_asm:
    movs    r0, #GPIO_BTN                // This value is the GPIO DOWN BTN on the PI PICO board
    bl      asm_gpio_init                                                         
    movs    r0, #GPIO_BTN             
    movs    r1, #GPIO_DIR_IN             
    bl      asm_gpio_set_dir             // Specify the GPIO pin as an output pin                                   
    movs    r0, #GPIO_BTN
    bl      asm_gpio_set_irq_rise        //Enables rising edge interrupts for the specified pin
    movs    r0, #GPIO_BTN
    bl      asm_gpio_set_irq_fall        //Enables falling edge interrupts for the specified pin
    @finishing setting up the buttons here
    @put the gpio interrupt handeler to the vector table
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)            
    ldr     r1, [r2]                        
    movs    r2, #GPIO_ISR_OFFSET            
    add     r2, r1                          
    ldr     r0, =gpio_isr   
    str     r0, [r2]      
    @put the timer interrupt handler to the vector table 
    ldr     r2, =(PPB_BASE + M0PLUS_VTOR_OFFSET)
    ldr     r1, [r2]
    movs    r2, #ALRM_ISR_OFFSET
    add     r2,r1
    ldr     r0, =timer_isr
    str     r0,[r2]

    @setting up in the nvic now
    ldr     r2,=(PPB_BASE+M0PLUS_NVIC_ICPR_OFFSET)
    ldr     r0,=0X2001
    str     r0,[r2]
    ldr     r2,=(PPB_BASE+M0PLUS_NVIC_ISER_OFFSET)
    str     r0,[r2]                       
    

wait_button:
    cmp     r6,#1
    beq     button_pressed
    b       wait_button

button_pressed:

    ldr     r2,=(TIMER_BASE+TIMER_INTE_OFFSET)
    ldr     r0,=1
    str     r0,[r2]
    ldr     r2,=(TIMER_BASE+TIMER_TIMELR_OFFSET)
    ldr     r0,[r2]       @read the current time
    ldr     r3,=DFLT_ALARM_TIME
    adds    r0,r3,r0
    ldr     r2,=(TIMER_BASE+TIMER_ALARM0_OFFSET)
    str     r0,[r2]  
    @finishing configuring all the interrupt here as well as nvic
    ldr     r7,=-1

    
loop:
  wfi 
  b       loop                                       @ Infinite loop

.thumb_func
timer_isr:  
   push     {r0-r5,lr} 
   ldr      r0,=0
   bl       addMorse      @add the terminate character means it has been two seconds
   ldr      r7,=-1    @reset for first release
  
   bl        read 

   @setting the flag bit for timer interrupt
   ldr      r1,=(TIMER_BASE+TIMER_INTR_OFFSET)
   movs     r2,#1
   str      r2,[r1]
   pop      {r0-r5,pc}



.thumb_func
gpio_isr:
     push     {r0-r5,lr}
     ldr      r6,=1
     bl       watchdog_update@update the watchdog to reset timer
     ldr      r0,=(IO_BANK0_BASE+IO_BANK0_PROC0_INTS2_OFFSET)
     ldr      r0,[r0]    @get the status of the gpio intrrupt
     ldr      r1,=GPIO_BTN_FALL_MSK   @r1 is the mask for button pressing
     ands     r0,r1,r0
     cmp      r0,r1       @compare the event with edge fall
     beq      button_press
     
     @from here means the button reached released
     ldr      r0,=(TIMER_BASE+TIMER_TIMELR_OFFSET)
     ldr      r0,[r0] @read the current time
     ldr      r1,=end_time
     str      r0,[r1]     @strore the end time to the shared memory
     ldr      r1,=start_time
     ldr      r1,[r1]
     subs     r0,r0,r1     @the duration time of button pressed
     @start to write morse code here
     
     ldr      r3,=-1
     cmp      r7,r3    
     beq      not_space
     ldr      r7,=interval
     ldr      r7,[r7]
     ldr      r3,=ONEHALF_SECOND
     cmp      r7,r3         @compare the interval between the button released until the next time button pressed
     bgt      space_input
not_space:
     ldr      r7,=0
     ldr      r3,=ONE_SECOND
     cmp      r0,r3
     bgt      add_dash        @if it's not longer than one second, add a dash
     ldr      r0,=46
     bl       addMorse
     b        reset_timer
add_dash:
     ldr       r0,=45
     bl        addMorse
reset_timer:
     ldr      r0,=(TIMER_BASE+TIMER_TIMELR_OFFSET)
     ldr      r0,[r0] @read the current time
     ldr      r1,=DFLT_ALARM_TIME
     add      r0,r0,r1
     ldr      r1,=(TIMER_BASE+TIMER_ALARM0_OFFSET)
     str      r0,[r1]
     b        end_write

space_input:
     ldr      r0,=32
     bl       addMorse
     ldr      r3,=start_time
     ldr      r3,[r3]
     ldr      r4,=end_time
     ldr      r4,[r4]
     subs     r0,r4,r3
     b        not_space
end_write:
     ldr      r1,=(TIMER_BASE+TIMER_TIMELR_OFFSET)
     ldr      r0,[r1]
     ldr      r1,=end_time  @record the button released time
     str      r0,[r1]
     ldr      r0,=GPIO_BTN_RISE_MSK
     ldr      r1,=(IO_BANK0_BASE+IO_BANK0_INTR2_OFFSET)
     str      r0,[r1]
     b        end_all

button_press:
     ldr      r1,=(TIMER_BASE+TIMER_TIMELR_OFFSET)
     ldr      r1,[r1]    @read the current start time
     ldr      r0,=start_time
     str      r1,[r0]    @store the start time of the button pressed

     ldr      r2,=end_time
     ldr      r2,[r2]    @read the last button released time
     subs     r1,r1,r2   @the time between the last time button released and next time button pressed
     ldr      r2,=interval @R2 is the interval time between the last time button pressed and the next time button pressed
     str      r1,[r2]    @store the interval time to the shared memory
     movs     r0,r1
     ldr      r0,=GPIO_BTN_FALL_MSK@r0 is the mask for clear pending bits for fall event
     ldr      r1,=(IO_BANK0_BASE+IO_BANK0_INTR2_OFFSET)
     str      r0,[r1]      
end_all:
     pop      {r0-r5,pc}


@ Set data alignment
.data
    .align 4
start_time: .word  DFLT_TIME
end_time:.word   DFLT_TIME
interval:.word   DFLT_TIME
