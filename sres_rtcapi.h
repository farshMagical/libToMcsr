#ifndef __SKRES_RTC_API_H__
#define __SKRES_RTC_API_H__

#pragma once


// Объявление версии API
#ifndef RTC_API_VER_DEF
#	define RTC_API_VER_DEF	0x1a
#else
#	error "RTC API version already defined somewhere."	
#endif

// Проверка совместимости объявленной версии с требуемой
#ifndef NO_VER_CHECK
// #	include "sresrtc/sres_rtcapi_verchk.h"
#endif

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#  ifndef SRESDEF_WINDOWS
#    define SRESDEF_WINDOWS 1
#  endif
#else
#  ifndef SRESDEF_POSIX
#    define SRESDEF_POSIX 1
#  endif
#endif

#if !defined(SRESDEF_WINDOWS) && !defined(__stdcall)
#define __stdcall
#endif

#if defined(SRESDEF_WINDOWS)
#  if defined(SRESRTC_BUILD_DLL)
#    define SRESRTCAPI __declspec(dllexport) __stdcall
#  else
#    define SRESRTCAPI __declspec(dllimport) __stdcall 
#  endif
#elif defined(__GNUC__) && defined(SRESRTC_BUILD_DLL)
#  define SRESRTCAPI __attribute__ ((visibility ("default")))
#else
#  define SRESRTCAPI
#endif

#if defined(__cplusplus)
extern "C" {
#endif

//////////////////////////////////////////////////////////////////////////////
// Типы RTC API																//
//////////////////////////////////////////////////////////////////////////////

// Единицы измерения интервалов времени в наносекундах
typedef uint64_t sres_time_dura_t;

// Единицы измерения времени в наносекундах с 1 января 1970г. (UTC)
typedef uint64_t sres_stamp_t;

// Значение оцифрованных данных
typedef uint16_t sres_sample_t;

// Частота оцифровки в Гц
typedef double sres_frequency_t;

// Тип штампа времени для сэмплов
typedef sres_time_dura_t sres_sample_time_stamp_t;

//////////////////////////////////////////////////////////////////////////////
// Параметры эксперимента												    //
//////////////////////////////////////////////////////////////////////////////

/// Параметры эксперимента.
/// Передаются в пользовательску функцию перед началом эксперимента.
///
struct _sres_exp_info_t
{
	sres_frequency_t	freq;	// частота работы ацп в Гц.
	size_t				length;	// число фактичских отсчетов (строк) в эксперименте.
	int					method;	// метод осреднения (0 - без осреднения)
	size_t				stride; // шаг при осреднении (1 - без осреднения).
	size_t				tcount;	// число каналов АЦП.
	size_t				wcount; // число задействованных каналов АЦП.
	size_t				scount; // число отсечтов на канал.
	uint16_t*			chmap;	// карта активных каналов.
								// длина массива - tcount
								// 0 - канал отключен, в протвном случае 
								//	номер физического канала (нумерация с 1).
};

typedef struct _sres_exp_info_t sres_exp_info_t;

//////////////////////////////////////////////////////////////////////////////
// Прочие типы																//
//////////////////////////////////////////////////////////////////////////////

typedef int sres_result_t;

enum
{
	sres_rc_success = 0,
	sres_rc_not_success = 1,
};

/// Коды уведомлений.
enum
{
	sres_nf_prepare = 1,
	sres_nf_start,
	sres_nf_stop
	//sres_nf_raw_data, 
};

/////////////////////////////////////////////////////////////////////////////
/// Функции вызова модуля управления									   //
/////////////////////////////////////////////////////////////////////////////

/*
Выполнение функций:
	Все вызывы осуществляются в специальных потоках управления,	работа 
	которых не оказывает существенного влияния на работоспособность
	и производительность сервера.

Порядок вызовов :
	В проработке.
*/

/// Функция инициализации.
/// Вызывается ядром после загрузки модуля.
///
/// \param _InitData
///		Блок инициализации. Резерв.
/// \param _Context
///		Контекст пользовательской функции. Передается
///		в остальных вызовах как формальный параметр.
///
sres_result_t
SRESRTCAPI
sres_func_init(
	const void* _InitData,
	void** _Context
	);

/// Функция обработки уведомлений.
/// Вызывается ядром при изменени состояния оборудования,
/// перед передачей данных и т.п.
///
/// \param _Code
///		Номер уведомления.
/// \param _Info
///		Дополнительная информация. Формат зависит от номера уведомления.
///		для sres_nf_prepare это указатель на структуру sres_exp_info_t.
/// \param _Size
///		Размер блока данных, адресуемого переменной _Info.
/// \param _Context
///		Контекст пользовательской функции.
///
sres_result_t 
SRESRTCAPI
sres_func_notify(
	int _Code,
	const void* _Info,
	size_t _Size,
	void* _Context
	);

/// Функция обработки данных.
///
/// \param _SeqNo
///		Номер строки.
/// \param _Stamp
///		Метка времени.
/// \param _Samples
///		Массив с данными. Длина массива определяется полем tcount 
///			структуры sres_exp_info_t.
/// \param _Context
///		Контекст пользовательской функции.
///
sres_result_t 
SRESRTCAPI
sres_func_data(
	size_t _SeqNo,
	sres_sample_time_stamp_t _Stamp,
	const sres_sample_t* _Samples,
	void* _Context
	);

/// Функция завершения работы.
/// Вызывается ядром перед выгрузкой модуля.
///
/// \param _Context
///		Контекст пользовательской функции.
///
void
SRESRTCAPI
sres_func_done(
	void* _Context
	);

#if defined(__cplusplus)
}
#endif

#endif // __SKRES_RTC_API_H__
