#ifndef LORA_CONFIG_H
#define LORA_CONFIG_H

// ============================================================
// lora_config.h - Configurações de hardware e rádio LoRa
// ============================================================

// Pinos de conexão LoRa (ajuste conforme sua montagem)
#define CS_PIN     9
#define RESET_PIN  8
#define IRQ_PIN    7

// Configuração do rádio
#define LORA_FREQUENCY  433E6    // 433 MHz (ou 868E6 / 915E6)
#define LORA_SF         10       // Spreading Factor (7 a 12)
#define LORA_CR         5        // Coding Rate (5 = 4/5, 6 = 4/6, ... 8 = 4/8)
#define LORA_POWER      2        // Potência de transmissão (0 a 20, dependendo do módulo)

// Endereço de broadcast (comum a todos os nós)
#define BROADCAST       0xFF

#endif