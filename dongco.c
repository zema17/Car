#include "stm32f1xx.h"

void delay_ms(uint32_t ms) {
    for (uint32_t i = 0; i < ms * 8000; i++) {
        __NOP();
    }
}

void PWM_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // Bật clock cho GPIOA
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;  // Bật clock cho TIM2

    // PA8 & PA9 làm PWM
    GPIOA->CRH &= ~(GPIO_CRH_MODE8 | GPIO_CRH_CNF8 | GPIO_CRH_MODE9 | GPIO_CRH_CNF9);
    GPIOA->CRH |= (GPIO_CRH_MODE8_1 | GPIO_CRH_CNF8_1 | GPIO_CRH_MODE9_1 | GPIO_CRH_CNF9_1);

    TIM2->PSC = 72 - 1;  // Chia tần số xuống 1 MHz
    TIM2->ARR = 1000;    // Chu kỳ PWM (tạo PWM 1 kHz)
    TIM2->CCR1 = 500;    // Độ rộng xung (50%)
    TIM2->CCR2 = 500;    

    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
    TIM2->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;
    TIM2->CR1 |= TIM_CR1_CEN;  // Bật Timer
}

void Motor_Init(void) {
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;  // Bật clock GPIOA

    // PA0 - PA3 là Output (IN1, IN2, IN3, IN4)
    GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0 | GPIO_CRL_MODE1 | GPIO_CRL_CNF1 |
                    GPIO_CRL_MODE2 | GPIO_CRL_CNF2 | GPIO_CRL_MODE3 | GPIO_CRL_CNF3);
    GPIOA->CRL |= (GPIO_CRL_MODE0_1 | GPIO_CRL_MODE1_1 | GPIO_CRL_MODE2_1 | GPIO_CRL_MODE3_1);
}

void Motor_Forward(void) {
    GPIOA->BSRR = GPIO_BSRR_BS0 | GPIO_BSRR_BR1;  // IN1 = 1, IN2 = 0 (Bánh trái)
    GPIOA->BSRR = GPIO_BSRR_BS2 | GPIO_BSRR_BR3;  // IN3 = 1, IN4 = 0 (Bánh phải)
}

void Motor_Backward(void) {
    GPIOA->BSRR = GPIO_BSRR_BR0 | GPIO_BSRR_BS1;  // IN1 = 0, IN2 = 1 (Bánh trái)
    GPIOA->BSRR = GPIO_BSRR_BR2 | GPIO_BSRR_BS3;  // IN3 = 0, IN4 = 1 (Bánh phải)
}

void Motor_Left(void) {
    GPIOA->BSRR = GPIO_BSRR_BR0 | GPIO_BSRR_BS1;  // IN1 = 0, IN2 = 1 (Bánh trái - lùi)
    GPIOA->BSRR = GPIO_BSRR_BS2 | GPIO_BSRR_BR3;  // IN3 = 1, IN4 = 0 (Bánh phải - tiến)
}

void Motor_Right(void) {
    GPIOA->BSRR = GPIO_BSRR_BS0 | GPIO_BSRR_BR1;  // IN1 = 1, IN2 = 0 (Bánh trái - tiến)
    GPIOA->BSRR = GPIO_BSRR_BR2 | GPIO_BSRR_BS3;  // IN3 = 0, IN4 = 1 (Bánh phải - lùi)
}

void Motor_Stop(void) {
    GPIOA->BSRR = GPIO_BSRR_BR0 | GPIO_BSRR_BR1;
    GPIOA->BSRR = GPIO_BSRR_BR2 | GPIO_BSRR_BR3;
    TIM2->CCR1 = 0;  // Dừng PWM bánh trái
    TIM2->CCR2 = 0;  // Dừng PWM bánh phải
}

int main(void) {
    PWM_Init();
    Motor_Init();

    Motor_Forward();  
    delay_ms(2000);  

    Motor_Left();  
    delay_ms(1000);

    Motor_Right();  
    delay_ms(1000);

    Motor_Backward();  
    delay_ms(2000);

    Motor_Stop();  

    while (1);
}
