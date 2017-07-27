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

#ifndef KAFKAMARK_TRACELOG_H
#define KAFKAMARK_TRACELOG_H

#include <stdint.h>
#include <stdio.h>

namespace Kafkamark {

/**
 * Provides a unified logging interface.  This class is not thread-safe.
 */
class TraceLog {
public:
    static void record(const char *format, ...);
    static void record(uint64_t timestamp, const char *format, ...);

    static void setOutputFilePath(const char *filePath);
    static void flush();

private:
    TraceLog();
    static FILE* outfile;

    static void record_internal(uint64_t timestamp,
                                const char *format,
                                va_list argp);
};

}  // namespace Kafkamark

#endif  // KAFKAMARK_TRACELOG_H
