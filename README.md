# Calculadora com LCD e Keypad para Arduino

Este projeto implementa uma calculadora completa usando um Arduino, um display LCD I2C, um teclado matricial (Keypad) e botões adicionais para operações específicas. A calculadora é capaz de realizar operações matemáticas básicas, porcentagens, e alternar sinais, exibindo os resultados no display LCD.

## Componentes Necessários

- Arduino (Uno, Mega, etc.)
- Display LCD 16x2 com interface I2C
- Keypad 4x4 (matricial) com 16 teclas
- 4 botões adicionais para funções da calculadora
- Cabos jumpers

## Instalação

1. **Conecte os Componentes**
   - Conecte o display LCD aos pinos I2C do Arduino.
   - Conecte as linhas e colunas do teclado matricial aos pinos 9, 8, 7, 6 para as colunas e 13, 12, 11, 10 para as linhas.
   - Conecte os botões adicionais aos pinos digitais 5, 4, 3 e 2.

2. **Inclua as Bibliotecas**
   Certifique-se de instalar as bibliotecas `Wire.h`, `LiquidCrystal_I2C.h` e `Keypad.h`, necessárias para o funcionamento do LCD e do teclado matricial.

3. **Carregue o Código**
   Copie o código e carregue-o no Arduino usando a IDE do Arduino.

## Funcionamento do Código

1. **Configuração do Keypad e LCD**
   - O teclado matricial é configurado com 4 linhas e 4 colunas, com um mapa de teclas para cada posição. O LCD é configurado para exibir 2 linhas e 16 colunas de texto.

2. **Estrutura da Calculadora**
   - A classe `Calculadora` controla o estado e as operações. Ela armazena os números, o operador, e permite manipulação do sinal e da porcentagem, além de controlar o display.

3. **Funções Principais**
   - **Operações Matemáticas**: As operações incluem adição, subtração, multiplicação, divisão e porcentagem.
   - **Funções Especiais**:
     - `limparCalculadora()`: limpa o display e redefine a calculadora.
     - `alternarSinal()`: alterna o sinal do número em edição.
     - `aplicarPorcentagem()`: aplica a operação de porcentagem ao número.
     - `apagarCaractere()`: apaga o último caractere digitado.
   - **Funções de Entrada**: Os métodos `adicionarNumero()`, `definirOperacao()` e `calcularResultado()` lidam com a entrada do usuário e executam as operações selecionadas.

4. **Exibição no LCD**
   - A tela LCD exibe a entrada e o resultado das operações. Mensagens de erro são mostradas em caso de operações inválidas, como divisão por zero.

## Pinagem

- **LCD**: I2C (endereço 0x27)
- **Keypad**:
  - Linhas: pinos 13, 12, 11, 10
  - Colunas: pinos 9, 8, 7, 6
- **Botões adicionais**: pinos 5, 4, 3 e 2

## Exemplo de Uso

1. Pressione os números e operadores no Keypad para inserir cálculos.
2. Utilize `=` para exibir o resultado.
3. Utilize os botões adicionais para limpar a calculadora, alternar o sinal, aplicar porcentagem e apagar caracteres.

## Funções do Código

- `setup()`: Inicializa o LCD e define os pinos dos botões adicionais.
- `loop()`: Monitora as entradas do Keypad e dos botões adicionais, executando as operações conforme as teclas pressionadas.
- `ehDigitoOuDecimal()`: Verifica se a tecla pressionada é um número ou ponto decimal.
- `ehOperador()`: Verifica se a tecla pressionada é um operador matemático.

## Notas

- **Debounce**: A função `debounce()` é usada para evitar leituras múltiplas dos botões adicionais.
- **Limite de Entrada**: A calculadora limita a entrada a 16 caracteres no LCD.
  
## Visualização do projeto

Link para o projeto: [https://wokwi.com/projects/402163073083375617]

![image](https://github.com/user-attachments/assets/b8625cf9-b9b8-45d7-b01e-360ec293980b)

## Contato:

Se você tiver alguma dúvida ou sugestão, sinta-se à vontade para abrir uma issue no repositório do GitHub ou enviar um e-mail para: 
[nathanfelipi2006@gmail.com].

---

Desenvolvido com ❤️ por [Nathan Felipi Almeida].
