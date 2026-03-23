/*
 * LAB6_Digital2_nano.c
 *
 * Created: 17/3/2026 17:58:07
 * Author : yelen
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART/UART.h"

volatile uint8_t boton_presionado = 0;
volatile char caracter_a_enviar = 0;



//-----------------------------------------------
// CONFIGURACIÓN PIN CHANGE INTERRUPT
//-----------------------------------------------
void setup_pinchange(void) {
	// --- PUERTO B (PB0 a PB4) ---
	DDRB &= ~((1 << PINB0) | (1 << PINB1) | (1 << PINB2) | (1 << PINB3) | (1 << PINB4));
	PORTB |= (1 << PINB0) | (1 << PINB1) | (1 << PINB2) | (1 << PINB3) | (1 << PINB4); // Pull-ups

	// --- PUERTO C (PC4) ---
	DDRC &= ~(1 << PINC4);
	PORTC |= (1 << PINC4); // Pull-up

	// Habilitar interrupciones por cambio de pin
	// PCIE0 -> Puerto B, PCIE1 -> Puerto C
	PCICR |= (1 << PCIE0) | (1 << PCIE1);

	// Desenmascarar pines específicos
	// PCINT0-4 corresponden a PB0-PB4
	PCMSK0 |= (1 << PCINT0) | (1 << PCINT1) | (1 << PCINT2) | (1 << PCINT3) | (1 << PCINT4);
	// PCINT12 corresponde a PC4
	PCMSK1 |= (1 << PCINT12);
}

//-----------------------------------------------
// FUNCIÓN PRINCIPAL
//-----------------------------------------------
int main(void) {
	INIT_UART(8);
	setup_pinchange();
	sei();

	while(1) {
		if (boton_presionado) {
			WriteChar(caracter_a_enviar);
			//_delay_ms(150); // Antirebote
			boton_presionado = 0;
		}
	}
}

//-----------------------------------------------
// INTERRUPCIÓN PUERTO B (PB0 - PB4)
//-----------------------------------------------
ISR(PCINT0_vect) {
	uint8_t estado_b = PINB;

	if (!(estado_b & (1 << PINB0))) {
		caracter_a_enviar = 'U'; // ARRIBA
		boton_presionado = 1;
	}
	else if (!(estado_b & (1 << PINB1))) {
		caracter_a_enviar = 'D'; // ABAJO
		boton_presionado = 1;
	}
	else if (!(estado_b & (1 << PINB2))) {
		caracter_a_enviar = 'R'; // DERECHA
		boton_presionado = 1;
	}
	else if (!(estado_b & (1 << PINB3))) {
		caracter_a_enviar = 'L'; // IZQUIERDA
		boton_presionado = 1;
	}
	else if (!(estado_b & (1 << PINB4))) {
		caracter_a_enviar = 'B'; // BOTÓN B
		boton_presionado = 1;
	}
}

//-----------------------------------------------
// INTERRUPCIÓN PUERTO C (PC4)
//-----------------------------------------------
ISR(PCINT1_vect) {
	uint8_t estado_c = PINC;

	// Solo evaluamos el botón A en PC4
	if (!(estado_c & (1 << PINC4))) {
		caracter_a_enviar = 'A'; // BOTÓN A
		boton_presionado = 1;
	}
}