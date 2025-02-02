/// @file
/// @author Oleg Abrosimov <olegabrosimovnsk@gmail.com>
/// @copyright MIT

#ifndef H_IO_LOG_T
#define H_IO_LOG_T

#ifdef _IO_DEBUG_ENABLED
#define IO_DEBUG(L) L
#else
#define IO_DEBUG(L)
#endif

#endif // H_IO_LOG_T
