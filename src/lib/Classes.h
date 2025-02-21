// Classes.h

#pragma once

#ifndef _CLASSES_h
#define _CLASSES_h

#define SERVO_MODE 0

// #include "Sens/Sens.h" // Base dos sensores
// #include "TriA/TriA.h" // Base dos sensores de 3 eixos
// #include "Baro/Baro.h" // Barometro BMP085
#include "Accel/Accel.h" // Acelerometro ADXL345
#include "Magn/Magn.h" // MagnetometroHMC5883
#include "Giro/Giro.h" // Giroscopio L3G4200D
#include "Helpful/Helpful.h" // Auxiliares de codigo
// #include "RoundArray/RoundArray.h" // Vetor rotativo
#include "MovingAverage/MovingAverage.h" // Media movel simples
#include "SerialFilter/SerialFilter.h" // Media movel em cascata
#include "GyGPS/GyGPS.h" // Auxiliar para GPS
#include "Apogeu/Apogeu.h" // Processamento de altitude e deteccao de apogeu
// #include "DuDeploy/DuDeploy.h" // Acionamento de paraquedas duplo
#include "MonoDeploy/MonoDeploy.h" // Acionamento de paraquedas simples
#include "SDCH/SDCH.h" // Auxiliar para gerenciamento de cartao SD
// #include "Term/Term.h" // Termometro LM35

// #include "MediaMovel/MediaMovel.h" // Media Movel Legado [substituida por MovingAverage]
// #include "ComProtocol/ComProtocol.h" // Protocolo de comunicacao entre placas [descontinuado]

#endif
