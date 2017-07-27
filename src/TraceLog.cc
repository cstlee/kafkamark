/* Copyright (c) 2017, Stanford University
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "TraceLog.h"

#include <stdarg.h>

#include "PerfUtils/Cycles.h"

using PerfUtils::Cycles;

namespace Kafkamark {

FILE* TraceLog::outfile = NULL;

/**
 * Record an event to the Trace Log.
 *
 * \param format
 *      'printf'-style format string for the event message to be printed.
 * \param ...
 *      Arguments of the provided format string.
 */
void
TraceLog::record(const char *format, ...)
{
    uint64_t timestamp = Cycles::rdtsc();
    va_list argp;
    va_start(argp, format);
    record_internal(timestamp, format, argp);
    va_end(argp);
}

/**
 * Record an event to the Trace Log with an associated timestamp.  Using this
 * function may result in event logs that are not in timestamp order.
 *
 * \param timestamp
 *      The TSC timestamp that should be associated with this event.
 * \param format
 *      'printf'-style format string for the event message to be printed.
 * \param ...
 *      Arguments of the provided format string.
 */
void
TraceLog::record(uint64_t timestamp, const char *format, ...)
{
    va_list argp;
    va_start(argp, format);
    record_internal(timestamp, format, argp);
    va_end(argp);
}

/**
 * Set the path to the file that will contain the recorded events.
 *
 * \param filePath
 *      The path of the file that will contain the recorded events.
 */
void
TraceLog::setOutputFilePath(const char *filePath) {
    outfile = fopen(filePath, "w");
}

/**
 * Ensure that any buffered record events are written out to the file.
 */
void
TraceLog::flush() {
    if (outfile != NULL) {
        fflush(outfile);
    }
}

/**
 * Internal helper function for record that does the actual output work.
 */
void
TraceLog::record_internal(uint64_t timestamp, const char *format, va_list argp)
{
    FILE* output = outfile ? outfile : stdout;
    fprintf(output, "%lu|", timestamp);
    vfprintf(output, format, argp);
    fprintf(output, "\n");
}

}  // namespace Kafkamark
