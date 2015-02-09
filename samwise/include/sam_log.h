/*  =========================================================================

    sam_log - logger facility for samwise

    This Source Code Form is subject to the terms of the MIT
    License. If a copy of the MIT License was not distributed with
    this file, You can obtain one at http://opensource.org/licenses/MIT

    =========================================================================
*/
/**

   \brief sam_log - logger facility for samwise

    This is the interface for a logging facility that
    handles different log levels and different
    output channels.

*/


#ifndef _SAM_LOG_H_
#define _SAM_LOG_H_


/// The different severity levels for logging
typedef enum {
    SAM_LOG_LVL_ERROR,   ///< just output severe errors
    SAM_LOG_LVL_INFO,    ///< to get informed upon important events
    SAM_LOG_LVL_TRACE    ///< to track the program flow
} sam_log_lvl_t;


#define SAM_LOG_LVL_TRACE_REPR  "trace"
#define SAM_LOG_LVL_INFO_REPR   "info"
#define SAM_LOG_LVL_ERROR_REPR  "error"


/// The different handler options
typedef enum {
    SAM_LOG_HANDLER_STD  ///< stdout and stderr handler
} sam_log_handler_t;


/// State for sam_log instances
typedef struct sam_log_t_ {
    char *endpoint;     ///< pull socket endpoint
    zactor_t *actor;    ///< logging thread
} sam_log_t;


/// The state of a log thread
typedef struct sam_log_inner_t_ {
    zsock_t *pll;             ///< socket accepting log requests
    const char *line_fmt;     ///< the output format for messages
    const char *date_fmt;     ///< format for timestamps
    struct handler_ {         ///< references to handlers per level
        zlist_t *trace;
        zlist_t *info;
        zlist_t *error;
    } handler;
} sam_log_inner_t;


/// The state of a logger
typedef struct sam_logger_t_ {
    zsock_t *psh;            ///< socket pushing log requests
    char *name;              ///< identifier for the logger
} sam_logger_t;


// Everything above gets cut. Prevents
// buffer overflows, since no heap is used.
#define SAM_LOG_LINE_MAXSIZE 256
#define SAM_LOG_DATE_MAXSIZE 16


//  --------------------------------------------------------------------------
/// @brief   Create a new logger
/// @param   endpoint Optional endpoint name
/// @return  Pointer to the logger state
CZMQ_EXPORT sam_log_t *
sam_log_new (char *endpoint);


//  --------------------------------------------------------------------------
/// @brief   Free's all memory of the logger state
CZMQ_EXPORT void
sam_log_destroy (sam_log_t **logger);


//  --------------------------------------------------------------------------
/// @brief   Add a function posing as a callback for a severity
/// @param   logger Log facility
/// @param   lvl Call handler for all severities up to <lvl>
/// @param   handler The callback function
CZMQ_EXPORT void
sam_log_add_handler (
    sam_log_t *log,
    sam_log_lvl_t lvl,
    sam_log_handler_t handler);


//  --------------------------------------------------------------------------
/// @brief   Remove a function registered for the lvl and lower
/// @param   logger Log facility
/// @param   lvl Call handler for all severities down to <lvl>
/// @param   handler The callback function
CZMQ_EXPORT void
sam_log_remove_handler (
    sam_log_t *log,
    sam_log_lvl_t lvl,
    sam_log_handler_t handler);


//  --------------------------------------------------------------------------
/// @brief   Logs to stdout and stderr
/// @param   lvl Severity level
/// @param   line The log line
CZMQ_EXPORT void
sam_log_handler_std (sam_log_lvl_t lvl, const char *line);


//  --------------------------------------------------------------------------
/// @brief   Retrieves the logging endpoint
/// @param   log Log facility
/// @param   line The log line
CZMQ_EXPORT char *
sam_log_endpoint (sam_log_t *log);


//  --------------------------------------------------------------------------
/// @brief   short description of the functions purpose
/// @param   args some arguments
/// @param   argc argc size of the argument vector
CZMQ_EXPORT void
sam_log_test ();


// shortcuts
#define sam_log_trace(logger, msg)                    \
    sam_logger_send (logger, SAM_LOG_LVL_TRACE, msg);

#define sam_log_info(logger, msg)                     \
    sam_logger_send (logger, SAM_LOG_LVL_INFO, msg);

#define sam_log_error(logger, msg)                    \
    sam_logger_send (logger, SAM_LOG_LVL_ERROR, msg);

#define sam_log_tracef(logger, msg, ...)                            \
    sam_logger_sendf (logger, SAM_LOG_LVL_TRACE, msg, __VA_ARGS__);

#define sam_log_infof(logger, msg, ...)                             \
    sam_logger_sendf (logger, SAM_LOG_LVL_INFO,  msg, __VA_ARGS__);

#define sam_log_errorf(logger, msg, ...)                            \
    sam_logger_sendf (logger, SAM_LOG_LVL_ERROR, msg, __VA_ARGS__);


#endif