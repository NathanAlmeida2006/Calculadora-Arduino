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

class Calculadora {
  private:
    String primeiroNumero;
    String segundoNumero;
    String stringEntrada;
    char operacao;
    bool segundoNumeroAtivo;
    bool porcentagemAplicadaPrimeiro;
    bool porcentagemAplicadaSegundo;

  public:
    Calculadora() {
      primeiroNumero = "";
      segundoNumero = "";
      stringEntrada = "";
      operacao = '\0';
      segundoNumeroAtivo = false;
      porcentagemAplicadaPrimeiro = false;
      porcentagemAplicadaSegundo = false;
    }

    void limparCalculadora() {
      primeiroNumero = "";
      segundoNumero = "";
      stringEntrada = "";
      operacao = '\0';
      segundoNumeroAtivo = false;
      porcentagemAplicadaPrimeiro = false;
      porcentagemAplicadaSegundo = false;
      lcd.clear();
    }

    void adicionarNumero(char tecla) {
      if (stringEntrada.length() >= 16) return;

      String &numeroAtual = segundoNumeroAtivo ? segundoNumero : primeiroNumero;
      bool &porcentagemAplicada = segundoNumeroAtivo ? porcentagemAplicadaSegundo : porcentagemAplicadaPrimeiro;

      if (porcentagemAplicada || (tecla == '.' && numeroAtual.indexOf('.') != -1) || (tecla == '0' && numeroAtual == "-")) return;
      if (tecla == '.' && numeroAtual == "-") numeroAtual = "-0";

      numeroAtual += tecla;
      atualizarDisplay();
    }

    void definirOperacao(char tecla) {
      if (primeiroNumero.length() > 0) {
        if (!segundoNumeroAtivo) segundoNumeroAtivo = true;
        operacao = tecla;
      }
      atualizarDisplay();
    }

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

    void exibirErro(String mensagem) {
      lcd.setCursor(0, 1);
      lcd.print(mensagem);
      delay(2000);
      limparCalculadora();
    }

    void alternarSinal() {
      String &numeroAtual = segundoNumeroAtivo ? segundoNumero : primeiroNumero;
      if (numeroAtual.startsWith("-")) numeroAtual.remove(0, 1);
      else numeroAtual = "-" + numeroAtual;

      if (numeroAtual == "-0" || numeroAtual == "-0." || numeroAtual == "-.") numeroAtual = "0";
      atualizarDisplay();
    }

    void aplicarPorcentagem() {
      String &numeroAtual = segundoNumeroAtivo ? segundoNumero : primeiroNumero;
      bool &porcentagemAplicada = segundoNumeroAtivo ? porcentagemAplicadaSegundo : porcentagemAplicadaPrimeiro;

      if (numeroAtual.length() > 0 && !porcentagemAplicada) {
        porcentagemAplicada = true;
        numeroAtual += "%";
      }
      atualizarDisplay();
    }

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

    void atualizarDisplay() {
      stringEntrada = primeiroNumero;
      if (segundoNumeroAtivo) stringEntrada += " " + String(operacao) + " " + segundoNumero;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(stringEntrada);
    }

    float converterNumero(String numero) {
      numero.replace("%", "");
      return numero.toFloat();
    }

    float realizarOperacao(float num1, float num2, char operacao) {
      switch (operacao) {
        case '+': return num1 + num2;
        case '-': return num1 - num2;
        case '*': return num1 * num2;
        case '/': return (num2 != 0) ? num1 / num2 : NAN;
        default: return NAN;
      }
    }
};

Calculadora calculadora;

void setup() {
  lcd.init();
  lcd.backlight();

  for (uint8_t i = 0; i < sizeof(pinosBotoes); ++i) {
    pinMode(pinosBotoes[i], INPUT_PULLUP);
  }

  calculadora.limparCalculadora();
}

void loop() {
  char tecla = teclado.getKey();

  if (digitalRead(pinosBotoes[0]) == LOW) {
    calculadora.limparCalculadora();
    debounce();
  }
  if (digitalRead(pinosBotoes[1]) == LOW) {
    calculadora.alternarSinal();
    debounce();
  }
  if (digitalRead(pinosBotoes[2]) == LOW) {
    calculadora.aplicarPorcentagem();
    debounce();
  }
  if (digitalRead(pinosBotoes[3]) == LOW) {
    calculadora.apagarCaractere();
    debounce();
  }

  if (tecla != NO_KEY) {
    if (ehDigitoOuDecimal(tecla)) {
      calculadora.adicionarNumero(tecla);
    } else if (ehOperador(tecla)) {
      calculadora.definirOperacao(tecla);
    } else if (tecla == '=') {
      calculadora.calcularResultado();
    }
  }
}

bool ehDigitoOuDecimal(char tecla) {
  return (tecla >= '0' && tecla <= '9') || (tecla == '.');
}

bool ehOperador(char tecla) {
  return tecla == '+' || tecla == '-' || tecla == '*' || tecla == '/';
}

void debounce() {
  delay(200);
}