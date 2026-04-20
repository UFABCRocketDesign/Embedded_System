// pressets.h

#pragma once

#ifndef _PRESSETS_H
#define _PRESSETS_H

#ifdef USING_MODE

#if USING_MODE == MODE_DEFAULT

#define CURRENT_MODE_P2H_NORMAL	450	// Height (meters) to main parachute normal deployment
#define CURRENT_MODE_P2H_BACKUP	400	// Height (meters) to main parachute backup deployment

#define CURRENT_MODE_WUFheigh	50	// Min height (meters) to confirm liftoff

#define CURRENT_MODE_WUPSdelay	10	// Delay (seconds) to bypass possible transonic unstable pressure

#define CURRENT_MODE_PROJECT_NAME "Airuma"

#elif USING_MODE == MODE_ELEVADOR

#define CURRENT_MODE_P2H_NORMAL	15	// Height (meters) to main parachute normal deployment
#define CURRENT_MODE_P2H_BACKUP	10	// Height (meters) to main parachute backup deployment

#define CURRENT_MODE_WUFheigh	5	// Min height (meters) to confirm liftoff

#define CURRENT_MODE_WUPSdelay	3	// Delay (seconds) to bypass possible transonic unstable pressure

#define CURRENT_MODE_PROJECT_NAME "Lift"

#elif USING_MODE == MODE_ASPIRADOR

#define CURRENT_MODE_P2H_NORMAL	50	// Height (meters) to main parachute normal deployment
#define CURRENT_MODE_P2H_BACKUP	25	// Height (meters) to main parachute backup deployment

#define CURRENT_MODE_WUFheigh	5	// Min height (meters) to confirm liftoff

#define CURRENT_MODE_WUPSdelay	3	// Delay (seconds) to bypass possible transonic unstable pressure

#define CURRENT_MODE_PROJECT_NAME "Vacuum"

#else
#error: Modo selecionado não está disponível
#endif

#else
#error: É necessário selecionar um modo com #define USING_MODE
#endif


#endif // _PRESSETS_H
