#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Definições de constantes para o Keypad e LCD
const uint8_t LINHAS = 4;
const uint8_t COLUNAS = 4;
char teclas[LINHAS][COLUNAS] = {
  { '1', '2', '3', '+' },
  { '4', '5', '6', '-' },
  { '7', '8', '9', '*' },
  { '.', '0', '=', '/' }
};

// Definição dos pinos das colunas e linhas do teclado
uint8_t pinosColunas[COLUNAS] = { 9, 8, 7, 6 };
uint8_t pinosLinhas[LINHAS] = { 13, 12, 11, 10 };

// Pinos dos botões adicionais
uint8_t pinosBotoes[] = { 5, 4, 3, 2 };

// Instanciando Keypad e LCD
Keypad teclado = Keypad(makeKeymap(teclas), pinosLinhas, pinosColunas, LINHAS, COLUNAS);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Variáveis globais para armazenar entradas e estado
String primeiroNumero = "", segundoNumero = "", stringEntrada = "";
char operacao = '\0';
bool segundoNumeroAtivo = false, porcentagemAplicadaPrimeiro = false, porcentagemAplicadaSegundo = false;

void setup() {
  // Inicializa o LCD
  lcd.init();
  lcd.backlight();
  
  // Configura os pinos dos botões adicionais como entrada com pull-up interno
  for (uint8_t i = 0; i < sizeof(pinosBotoes); ++i) {
    pinMode(pinosBotoes[i], INPUT_PULLUP);
  }

  // Limpa a calculadora para o estado inicial
  limparCalculadora();
}

void loop() {
  // Lê a tecla pressionada no teclado
  char tecla = teclado.getKey();
  
  // Lida com a entrada dos botões físicos
  lidarComPressaoBotao();
  
  // Processa a tecla pressionada no teclado
  processarTecla(tecla);
}

// Função para lidar com a entrada dos botões físicos
void lidarComPressaoBotao() {
  if (digitalRead(pinosBotoes[0]) == LOW) { limparCalculadora(); debounce(); }
  if (digitalRead(pinosBotoes[1]) == LOW) { alternarSinal(); debounce(); }
  if (digitalRead(pinosBotoes[2]) == LOW) { aplicarPorcentagem(); debounce(); }
  if (digitalRead(pinosBotoes[3]) == LOW) { apagarCaractere(); debounce(); }
}

// Função para processar a tecla pressionada no teclado
void processarTecla(char tecla) {
  if (tecla != NO_KEY) {
    if (ehDigitoOuDecimal(tecla)) { adicionarNumero(tecla); }
    else if (ehOperador(tecla)) { definirOperacao(tecla); }
    else if (tecla == '=') { calcularResultado(); }
  }
}

// Verifica se a tecla pressionada é um dígito ou ponto decimal
bool ehDigitoOuDecimal(char tecla) {
  return (tecla >= '0' && tecla <= '9') || (tecla == '.');
}

// Adiciona o número à entrada correspondente
void adicionarNumero(char tecla) {
  if (stringEntrada.length() >= 16) return; // Impede a adição de caracteres se o limite for atingido
  
  String &numeroAtual = segundoNumeroAtivo ? segundoNumero : primeiroNumero;
  bool &porcentagemAplicada = segundoNumeroAtivo ? porcentagemAplicadaSegundo : porcentagemAplicadaPrimeiro;

  if (porcentagemAplicada || (tecla == '.' && numeroAtual.indexOf('.') != -1) || (tecla == '0' && numeroAtual == "-")) return;
  if (tecla == '.' && numeroAtual == "-") numeroAtual = "-0";

  numeroAtual += tecla;
  atualizarDisplay();
}

// Verifica se a tecla pressionada é um operador
bool ehOperador(char tecla) {
  return tecla == '+' || tecla == '-' || tecla == '*' || tecla == '/';
}

// Define a operação matemática a ser realizada
void definirOperacao(char tecla) {
  if (primeiroNumero.length() > 0) {
    if (!segundoNumeroAtivo) segundoNumeroAtivo = true;
    operacao = tecla;
  }
  atualizarDisplay();
}

// Calcula o resultado da operação
void calcularResultado() {
  if (primeiroNumero.length() > 0 && segundoNumero.length() > 0) {
    float num1 = converterNumero(primeiroNumero);
    float num2 = converterNumero(segundoNumero);

    if (porcentagemAplicadaPrimeiro) num1 /= 100.0;
    if (porcentagemAplicadaSegundo) num2 = (num1 * num2) / 100.0;

    float resultado = realizarOperacao(num1, num2, operacao);
    exibirResultado(resultado);
  }
}

// Executa a operação matemática
float realizarOperacao(float num1, float num2, char operacao) {
  switch (operacao) {
    case '+': return num1 + num2;
    case '-': return num1 - num2;
    case '*': return num1 * num2;
    case '/': return (num2 != 0) ? num1 / num2 : NAN;
    default: return NAN;
  }
}

// Exibe o resultado da operação no LCD
void exibirResultado(float resultado) {
  if (!isnan(resultado)) {
    lcd.setCursor(0, 1);
    lcd.print(resultado);
    delay(3000);
    limparCalculadora();
  } else {
    exibirErro("Erro: Div/0");
  }
}

// Exibe uma mensagem de erro no LCD
void exibirErro(String mensagem) {
  lcd.setCursor(0, 1);
  lcd.print(mensagem);
  delay(2000);
  limparCalculadora();
}

// Limpa a calculadora para o estado inicial
void limparCalculadora() {
  primeiroNumero = segundoNumero = stringEntrada = "";
  segundoNumeroAtivo = porcentagemAplicadaPrimeiro = porcentagemAplicadaSegundo = false;
  operacao = '\0';
  lcd.clear();
}

// Alterna o sinal do número atual
void alternarSinal() {
  String &numeroAtual = segundoNumeroAtivo ? segundoNumero : primeiroNumero;
  if (numeroAtual.startsWith("-")) numeroAtual.remove(0, 1);
  else numeroAtual = "-" + numeroAtual;

  if (numeroAtual == "-0" || numeroAtual == "-0." || numeroAtual == "-.") numeroAtual = "0";
  atualizarDisplay();
}

// Aplica o sinal de porcentagem ao número atual
void aplicarPorcentagem() {
  String &numeroAtual = segundoNumeroAtivo ? segundoNumero : primeiroNumero;
  bool &porcentagemAplicada = segundoNumeroAtivo ? porcentagemAplicadaSegundo : porcentagemAplicadaPrimeiro;

  if (numeroAtual.length() > 0 && !porcentagemAplicada) {
    porcentagemAplicada = true;
    numeroAtual += "%";
  }
  atualizarDisplay();
}

// Remove o último caractere da entrada atual
void apagarCaractere() {
  String &numeroAtual = segundoNumeroAtivo ? segundoNumero : primeiroNumero;
  bool &porcentagemAplicada = segundoNumeroAtivo ? porcentagemAplicadaSegundo : porcentagemAplicadaPrimeiro;

  if (numeroAtual.length() > 0) {
    if (numeroAtual.endsWith("%")) porcentagemAplicada = false;
    numeroAtual.remove(numeroAtual.length() - 1);
  } else if (segundoNumeroAtivo) {
    segundoNumeroAtivo = false;
  } else if (operacao != '\0') {
    operacao = '\0';
  }
  atualizarDisplay();
}

// Atualiza o display LCD com a entrada atual
void atualizarDisplay() {
  stringEntrada = primeiroNumero;
  if (segundoNumeroAtivo) stringEntrada += " " + String(operacao) + " " + segundoNumero;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(stringEntrada);
}

// Converte uma string numérica para float, removendo o sinal de porcentagem, se necessário
float converterNumero(String numero) {
  numero.replace("%", "");
  return numero.toFloat();
}

// Função para debounce dos botões
void debounce() {
  delay(200); // Debounce delay
}
