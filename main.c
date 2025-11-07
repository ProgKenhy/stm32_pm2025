#include <stdint.h>
#include <stm32f10x.h>

void delay(uint32_t ticks) {
    for (uint32_t i = 0; i < ticks; i++) {
        __NOP();
    }
}

int main(void) {
    // Enable clocks for GPIOA and GPIOC
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPCEN;
    
    // PC0 - LED output (push-pull)
    GPIOC->CRL &= ~(GPIO_CRL_CNF0 | GPIO_CRL_MODE0);
    GPIOC->CRL |= GPIO_CRL_MODE0_0;
    
    // PA1 - Button A (Up) - input with pull-up
    GPIOA->CRL &= ~(GPIO_CRL_CNF1 | GPIO_CRL_MODE1);
    GPIOA->CRL |= GPIO_CRL_CNF1_1;
    GPIOA->ODR |= GPIO_ODR_ODR1;
    
    // PA2 - Button C (Down) - input with pull-up  
    GPIOA->CRL &= ~(GPIO_CRL_CNF2 | GPIO_CRL_MODE2);
    GPIOA->CRL |= GPIO_CRL_CNF2_1;
    GPIOA->ODR |= GPIO_ODR_ODR2;
    
    uint32_t delay_time = 1000000;
    const uint32_t min_delay = 15625;
    const uint32_t max_delay = 64000000;
    
    uint32_t last_debounce_time = 0;
    uint8_t last_button_state = 1;

    while (1) {
        // Чтение состояния кнопок
        uint8_t current_button_state = (GPIOA->IDR & (GPIO_IDR_IDR1 | GPIO_IDR_IDR2)) >> 1;
        
        // Обработка кнопок с антидребезгом
        if (current_button_state != last_button_state) {
            delay(5000); // Короткая задержка антидребезга
            current_button_state = (GPIOA->IDR & (GPIO_IDR_IDR1 | GPIO_IDR_IDR2)) >> 1;
            
            if ((current_button_state & 0x01) == 0) { // Кнопка A нажата
                if (delay_time > min_delay) {
                    delay_time /= 2;
                }
            }
            
            if ((current_button_state & 0x02) == 0) { // Кнопка C нажата
                if (delay_time < max_delay) {
                    delay_time *= 2;
                }
            }
            
            last_button_state = current_button_state;
        }
        
        // Мигание светодиода
        GPIOC->ODR ^= GPIO_ODR_ODR0;
        delay(delay_time);
    }
}