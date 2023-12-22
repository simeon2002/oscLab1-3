/**
 * \author BL
 * \file logger.h
 * \brief Header file for logger module
 */

#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <time.h>

#define BUFFER_SIZE 1000
#define LOG_FILE "gateway.log"

// status codes
#define FILE_OPENING_ERROR 1
#define FORK_FAILURE 2
#define LOG_WRITING_ERROR 3
#define FILE_CLOSING_ERROR 4
#define PIPE_CREATION_FAILURE 5
#define PIPE_READING_ERROR 6

/**
 * \brief Write a message to the log process.
 * \param msg The message to be written.
 * \return Status code.
 */
int write_to_log_process(char *msg);

/**
 * \brief Create the log process.
 * \return Status code.
 */
int create_log_process();

/**
 * \brief End the log process.
 * \return Status code.
 */
int end_log_process();

#endif
