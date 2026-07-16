# Protocolo de Rede *Multi-hop* Baseada em LoRa

Este projeto implementa uma rede de comunicação LoRa composta por uma estação base e até dez nós finais. A estação base envia requisições periódicas, e os nós finais respondem após intervalos configurados individualmente.

As requisições são propagadas por encaminhamento temporizado. No caminho de retorno, as respostas são encaminhadas pelos nós de menor endereço em direção à estação base. Cada encaminhamento acrescenta um **carimbo** à mensagem, permitindo registrar o caminho percorrido.

> Nesta versão, os nós finais enviam apenas seu carimbo identificador. A leitura de sensores ainda não está implementada no sketch `LoRaNode.ino`.

---

## 📁 Estrutura de arquivos

O projeto utiliza arquivos separados para centralizar as configurações do rádio, dos nós finais e da estação base.
* **lora_config.h :** Configurações compartilhadas do rádio LoRa, como pinos, frequência, fator de espalhamento (`SF`), taxa de codificação (`CR`), potência de transmissão e endereço de broadcast.
* **node_config.h :** - tabela com os endereços, intervalos de envio, intervalos de encaminhamento e carimbos dos nós finais. O nó compilado é selecionado pela constante `NODE_ID`.
* **base_config.h :** - configurações exclusivas da estação base, incluindo endereço local, endereço da rede, intervalo entre requisições e carimbo inicial.
* **LoRaNode.ino :** - sketch utilizado nos nós finais, com endereços entre `0xA0` e `0xA9`.
* **LoRaBase.ino :** - sketch utilizado na estação base, configurada com o endereço `0xFF`.

> Para compilar um nó final específico, altere somente o valor de `NODE_ID` em `node_config.h`. A estação base não utiliza `NODE_ID` nem o arquivo `node_config.h`.

---

## ⚙️ Arquivos de configuração

### `lora_config.h`

Define os parâmetros de hardware e rádio utilizados pela estação base e por todos os nós finais.

```cpp
#define CS_PIN          9
#define RESET_PIN       8
#define IRQ_PIN         7

#define LORA_FREQUENCY  433E6    // 433 MHz
#define LORA_SF         10       // Spreading Factor
#define LORA_CR         5        // Coding Rate 4/5
#define LORA_POWER      2        // Potência de transmissão

#define BROADCAST       0xFF
```

Altere esses valores de acordo com o módulo, a montagem do hardware e a regulamentação local aplicável.

---

### `node_config.h`

Centraliza as configurações específicas dos nós finais. O valor de `NODE_ID` deve estar entre `0` e `9`.

```cpp
#define NODE_ID 0   // Altere para selecionar o nó desejado
```

#### Identificação dos nós finais

| `NODE_ID` | Endereço | Carimbo |
|:---------:|:--------:|:-------:|
| `0` | `0xA0` | `"0"` |
| `1` | `0xA1` | `"1"` |
| `2` | `0xA2` | `"2"` |
| `3` | `0xA3` | `"3"` |
| `4` | `0xA4` | `"4"` |
| `5` | `0xA5` | `"5"` |
| `6` | `0xA6` | `"6"` |
| `7` | `0xA7` | `"7"` |
| `8` | `0xA8` | `"8"` |
| `9` | `0xA9` | `"9"` |

#### Temporização dos nós finais

Todos os tempos estão expressos em milissegundos.

| `NODE_ID` | Intervalo | Fwd. ida | Fwd. volta |
|:---------:|----------:|---------:|-----------:|
| `0` | 40.000 | 0 | 0 |
| `1` | 41.000 | 1.000 | 1.000 |
| `2` | 43.000 | 2.000 | 2.000 |
| `3` | 47.000 | 3.000 | 3.000 |
| `4` | 54.000 | 4.000 | 4.000 |
| `5` | 65.000 | 5.000 | 5.000 |
| `6` | 81.000 | 6.000 | 6.000 |
| `7` | 103.000 | 7.000 | 7.000 |
| `8` | 132.000 | 8.000 | 8.000 |
| `9` | 169.000 | 9.000 | 9.000 |

---

### `base_config.h`

Define os parâmetros utilizados exclusivamente pela estação base.

```cpp
#define LOCAL_ADDRESS   0xFF
#define NETWORK_ADDRESS 0x00
#define INTERVAL        3600000   // 1 hora, em milissegundos
#define CARIMBO         "ff"
```

A estação base envia uma nova requisição a cada `3.600.000 ms`, equivalentes a uma hora.

---

## 🛠️ Compilação e gravação

### Nó final

Exemplo para compilar o nó de endereço `0xA3`:

1. Abra `node_config.h`.
2. Defina `NODE_ID` como `3`.
3. Abra o sketch `LoRaNode.ino` na IDE Arduino.
4. Selecione a placa e a porta corretas.
5. Compile e carregue o código no dispositivo.

Repita o procedimento para cada nó, alterando `NODE_ID` antes de cada compilação.

### Estação base

1. Verifique os parâmetros em `base_config.h`.
2. Abra o sketch `LoRaBase.ino` na IDE Arduino.
3. Selecione a placa e a porta corretas.
4. Compile e carregue o código no dispositivo.

---

## 📦 Dependências

### Bibliotecas
* SPI 
* LoRa
* SD 
* RTClib


### Hardware

- placas Arduino ou compatíveis;
- módulos LoRa SX1276 ou SX1278;
- módulo RTC compatível com `RTC_DS1307` para a estação base;
- cartão SD e respectivo módulo para a estação base.

No sketch da estação base, o cartão SD utiliza o pino `10` como `CHIP_SELECT`, enquanto o módulo LoRa utiliza o pino `9` como `CS_PIN`.

---





