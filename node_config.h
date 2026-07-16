#ifndef NODE_CONFIG_H
#define NODE_CONFIG_H

// ==================== SELEÇÃO DO NÓ ====================
// Altere este número para o nó desejado (0 a 9)
#define NODE_ID 0   // <-- MUDE AQUI

// ==================== TABELA DOS NÓS FINAIS ====================
typedef struct {
  byte address;
  unsigned long interval;
  unsigned long fwdGo;
  unsigned long fwdReturn;
  const char* carimbo;
} NodeConfig;

const NodeConfig nodeTable[] = {
  { 0xA0, 40000, 0, 0, "0" },
  { 0xA1, 41000, 1000, 1000, "1" },
  { 0xA2, 43000, 2000, 2000, "2" },
  { 0xA3, 47000, 3000, 3000, "3" },
  { 0xA4, 54000, 4000, 4000, "4" },
  { 0xA5, 65000, 5000, 5000, "5" },
  { 0xA6, 81000, 6000, 6000, "6" },
  { 0xA7, 103000, 7000, 7000, "7" },
  { 0xA8, 132000, 8000, 8000, "8" },
  { 0xA9, 169000, 9000, 9000, "9" }
};

// ==================== VALIDAÇÃO E DEFINIÇÕES ====================
#if (NODE_ID < 0) || (NODE_ID > 9)
  #error "NODE_ID deve estar entre 0 e 9"
#endif

#define LOCAL_ADDRESS            nodeTable[NODE_ID].address
#define INTERVAL                 nodeTable[NODE_ID].interval
#define INTERVAL_FORWARD_GO      nodeTable[NODE_ID].fwdGo
#define INTERVAL_FORWARD_RETURN  nodeTable[NODE_ID].fwdReturn
#define CARIMBO                  nodeTable[NODE_ID].carimbo

#endif
