# Trabalho Prático 1 — Algoritmo do Banqueiro


Implementação multithreaded do **Algoritmo do Banqueiro** em C com Pthreads, conforme o Trabalho Prático 1 da disciplina de Sistemas Operacionais.

---

## Estrutura do projeto

```
banker-algorithm/
├── banker.c
├── .gitignore
└── README.md
```

---

## Pré-requisitos

Antes de compilar e executar, você precisa ter o **GCC com suporte a Pthreads** instalado.

### Windows
1. Acesse **winlibs.com**
2. Baixe o arquivo ZIP da versão mais recente — **Win64 (without LLVM/Clang/LLD/LLDB): Zip archive**
3. Extraia o ZIP em uma pasta fixa, por exemplo: `C:\mingw64`
4. Adicione o caminho da pasta `bin` ao PATH do sistema:
   - Aperte `Win + R`, digite `sysdm.cpl` e pressione Enter
   - Vá em **Avançado** → **Variáveis de Ambiente**
   - Em **Variáveis do sistema**, selecione **Path** → **Editar**
   - Clique em **Novo** e adicione o caminho da pasta `bin`, por exemplo:
     ```
     C:\mingw64\bin
     ```
   - Clique OK em tudo e **reinicie o computador**
5. Verifique a instalação abrindo um terminal e digitando:
   ```
   gcc --version
   ```

### Linux
```bash
sudo apt install gcc
```

### macOS
```bash
xcode-select --install
```

---

## Compilação

Abra o terminal na pasta onde está o arquivo `banker.c` e execute:

```bash
gcc -o banker banker.c -lpthread
```

---

## Execução

O programa recebe como argumentos o número de instâncias de cada tipo de recurso:

```bash
# Linux / macOS
./banker <rec1> <rec2> <rec3>

# Windows (PowerShell)
.\banker.exe <rec1> <rec2> <rec3>
```

### Exemplo com 3 tipos de recursos (10, 5 e 7 instâncias):

```bash
# Linux / macOS
./banker 10 5 7

# Windows (PowerShell)
.\banker.exe 10 5 7
```
---

## Referências

SILBERSCHATZ, Abraham; GALVIN, Peter B.; GAGNE, Greg. *Fundamentos de sistemas operacionais*. 9. ed. Rio de Janeiro: LTC, 2015.
- Seção 7.5.3 — Algoritmo do Banqueiro
- Seção 7.5.3.1 — Algoritmo de Segurança
- Seção 7.5.3.2 — Algoritmo de Solicitação de Recursos
- Seção 5.9.4 — Mutex Pthreads
