// Vacalourabot.ino

#include "Vacalourabot.h"

#include <EEPROM.h>

// variables globais

// memoria para os movementos
static MoveProgram PROGRAMA = {
  point_of_view: POV_INICIAL,
  move_count: 0,
};

// memoria para os estados dos botóns
byte estado_boton_esquerda;
byte estado_boton_dereita;
byte estado_boton_adiante;
byte estado_boton_atras;
byte estado_boton_ir;
byte estado_boton_borrar;

void activarMotor(int motor, SENTIDO direccion)
{
  if (motor == MOTOR_DEREITA)
  {
      // motor dereita
      digitalWrite(PIN_MOTOR_DEREITA_A, direccion ? HIGH : LOW);
      digitalWrite(PIN_MOTOR_DEREITA_B, direccion ? LOW : HIGH);
  }
  else
  {
      // motor esquerda
      digitalWrite(PIN_MOTOR_ESQUERDA_A, direccion ? HIGH : LOW);
      digitalWrite(PIN_MOTOR_ESQUERDA_B, direccion ? LOW : HIGH);
  }
}

void desactivarMotor(int motor)
{
  if (motor == MOTOR_DEREITA)
  {
      // motor dereita
      digitalWrite(PIN_MOTOR_DEREITA_A, LOW);
      digitalWrite(PIN_MOTOR_DEREITA_B, LOW);
  }
  else
  {
      // motor esquerda
      digitalWrite(PIN_MOTOR_ESQUERDA_A, LOW);
      digitalWrite(PIN_MOTOR_ESQUERDA_B, LOW);
  }
}

void avanzar(byte unidades, SENTIDO direccion)
{
    // activamos os dous motores
    activarMotor(MOTOR_DEREITA, direccion);
    activarMotor(MOTOR_ESQUERDA, direccion);

    delay(unidades * PASO_MILISEGUNDOS);

    // desactivamos os dous motores
    desactivarMotor(MOTOR_DEREITA);
    desactivarMotor(MOTOR_ESQUERDA);
}

void xirar(XIRO xiro, int milisegundos)
{
   if (xiro == XIRO_DEREITA)
   {
      // xiro en sentido reloxo
      activarMotor(MOTOR_ESQUERDA, SENTIDO_ADIANTE);
      activarMotor(MOTOR_DEREITA, SENTIDO_ATRAS);
   }
   else
   {
     // xiro en sentido anti-reloxo
      activarMotor(MOTOR_DEREITA, SENTIDO_ADIANTE);
      activarMotor(MOTOR_ESQUERDA, SENTIDO_ATRAS);
   }

   delay(milisegundos);

   desactivarMotor(MOTOR_ESQUERDA);
   desactivarMotor(MOTOR_DEREITA);
}

void xirar90(int veces)
{
  // non facer nada cando veces sexa 0
  if (veces != 0)
  {
    // cálculo do xiro e cantidade
    XIRO xiro;
    int cantidade;

    // tipo de xiro e cantidade en positivo
    if (veces > 0)
    {
      xiro = XIRO_DEREITA;
      cantidade = veces;
    }
    else
    {
      xiro = XIRO_ESQUERDA;
      cantidade = -veces;
    }

    // executar os xiros
    while (cantidade-- > 0)
    {
      xirar(xiro, ANGULO_90_MILISEGUNDOS);
    }
  }
}

void eepromGravar()
{
  // gardamos na EEPROM cando haxa movementos que gardar
  if (PROGRAMA.move_count > 0)
  {
    // o primeiro dato é o número de movementos
    EEPROM.write(0, PROGRAMA.move_count);

    // e de seguido os movementos almacenados
    for (byte m = 0; m < PROGRAMA.move_count; m++)
    {
      EEPROM.write(m + 1, PROGRAMA.move_list[m]);
    }
  }
}

void eepromCargar()
{
  // o primeiro dato é o número de movementos
  PROGRAMA.move_count = EEPROM.read(0);

  if (PROGRAMA.move_count > MOVE_LIMIT)
  {
    // a memoria EEPROM está corrupta
    PROGRAMA.move_count = 0;
    return;
  }

  // e de seguido os movementos
  for (byte m = 0; m < PROGRAMA.move_count; m++)
  {
    PROGRAMA.move_list[m] = (MOVE)EEPROM.read(m + 1);
  }
}

void irPdvNeno()
{
  // supoñer que se está detrás da vacalourabot
  byte direccion_anterior = MOVE_FORWARD;

  // por cada un dos movementos
  for (byte m = 0; m < PROGRAMA.move_count; m++)
  {
    // calcular o xiro dende o punto de vista actual
    int xiro = PROGRAMA.move_list[m] - direccion_anterior;

    // axustar xiro para o lado máis curto
    if (abs(xiro) == 3) xiro /= -xiro;

    // executar o xiro
    xirar90(xiro);

    // avanzar un paso cara adiante
    avanzar(1, SENTIDO_ADIANTE);

    // actualizar dirección actual
    direccion_anterior = PROGRAMA.move_list[m];
  }
}

void irPdvVacaloura()
{
  // por cada un dos movementos
  for (byte m = 0; m < PROGRAMA.move_count; m++)
  {
    // evalúa o tipo e procede
    switch (PROGRAMA.move_list[m])
    {
      case MOVE_RIGHT:
        xirar90(1);
        break;
      case MOVE_LEFT:
        xirar90(-1);
        break;
      case MOVE_FORWARD:
        avanzar(1, SENTIDO_ADIANTE);
        break;
      case MOVE_BACKWARD:
        avanzar(1, SENTIDO_ATRAS);
        break;
    }
  }
}

void ir()
{
  // só imos se hai movementos programados
  if (PROGRAMA.move_count > 0)
  {
    // gardamos o programa de movementos na EEPROM
    eepromGravar();

    // pequena pausa para dar tempo a soltar o botón
    delay(1000);

    // evaluar o punto de vista para execución de movementos
    if (PROGRAMA.point_of_view == POV_VACALOURA)
    {
      // punto de vista da vacaloura
      irPdvVacaloura();
    }
    else
    {
      // punto de vista do neno
      irPdvNeno();
    }
  }
}

void memoria_borrar()
{
  PROGRAMA.move_count = 0;
}


void memorizar(MOVE movemento)
{
  if (PROGRAMA.move_count < MOVE_LIMIT)
  {
    PROGRAMA.move_list[PROGRAMA.move_count] = movemento;
    PROGRAMA.move_count++;
  }
  else
  {
    ir();
  }
}

void setup()
{
    // configurar motores
    pinMode(PIN_MOTOR_ESQUERDA_A, OUTPUT);
    pinMode(PIN_MOTOR_ESQUERDA_B, OUTPUT);
    pinMode(PIN_MOTOR_DEREITA_A, OUTPUT);
    pinMode(PIN_MOTOR_DEREITA_B, OUTPUT);

    // configurar botóns
    pinMode(PIN_BOTON_ESQUERDA, INPUT);
    pinMode(PIN_BOTON_DEREITA, INPUT);
    pinMode(PIN_BOTON_ADIANTE, INPUT);
    pinMode(PIN_BOTON_ATRAS, INPUT);
    pinMode(PIN_BOTON_IR, INPUT);
    pinMode(PIN_BOTON_BORRAR, INPUT);

    // inicializar os estados dos botóns
    estado_boton_esquerda = LOW;
    estado_boton_dereita = LOW;
    estado_boton_adiante = LOW;
    estado_boton_atras = LOW;
    estado_boton_ir = LOW;
    estado_boton_borrar = LOW;

    // inicializar memoria
    memoria_borrar();

    // cargar os movementos logo dun apagado
    eepromCargar();
}



void loop(){

  // leer os botóns
  if (digitalRead(PIN_BOTON_ESQUERDA) ^ estado_boton_esquerda)
  {
    estado_boton_esquerda = !estado_boton_esquerda;
    if (estado_boton_esquerda == HIGH)
    {
      memorizar(MOVE_LEFT);
    }
  }

  if (digitalRead(PIN_BOTON_DEREITA) ^ estado_boton_dereita)
  {
    estado_boton_dereita = !estado_boton_dereita;
    if (estado_boton_dereita == HIGH)
    {
      memorizar(MOVE_RIGHT);
    }
  }

  if (digitalRead(PIN_BOTON_ADIANTE) ^ estado_boton_adiante)
  {
    estado_boton_adiante = !estado_boton_adiante;
    if (estado_boton_adiante == HIGH)
    {
      memorizar(MOVE_FORWARD);
    }
  }

  if (digitalRead(PIN_BOTON_ATRAS) ^ estado_boton_atras)
  {
    estado_boton_atras = !estado_boton_atras;
    if (estado_boton_atras == HIGH)
    {
      memorizar(MOVE_BACKWARD);
    }
  }

  if (digitalRead(PIN_BOTON_BORRAR) ^ estado_boton_borrar)
  {
    estado_boton_borrar = !estado_boton_borrar;
    if (estado_boton_borrar == HIGH)
    {
      memoria_borrar();
    }
  }

  if (digitalRead(PIN_BOTON_IR) ^ estado_boton_ir)
  {
    estado_boton_ir = !estado_boton_ir;
    if (estado_boton_ir == HIGH)
    {
      // tempiño para que solten o botón antes de arrancar
      delay(500);

      // executa os movementos
      ir();
    }
  }

}
