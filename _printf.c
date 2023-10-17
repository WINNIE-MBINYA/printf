#include "main.h"

void clean_up_va_list(va_list args, custom_buffer_t *output);
int process_printf(const char *format, va_list args, custom_buffer_t *output);
int custom_printf(const char *format, ...);

/**
 * clean_up_va_list - Performs cleanup operations for custom_printf.
 * @args: A va_list of arguments provided to custom_printf.
 * @output: A custom_buffer_t struct.
 */
void clean_up_va_list(va_list args, custom_buffer_t *output)
{
    va_end(args);
    write(1, output->start, output->len);
    free_custom_buffer(output);
}

/**
 * process_printf - Reads through the format string for custom_printf.
 * @format: Character string to print - may contain directives.
 * @output: A custom_buffer_t struct containing a buffer.
 * @args: A va_list of arguments.
 *
 * Return: The number of characters stored to output.
 */
int process_printf(const char *format, va_list args, custom_buffer_t *output)
{
    int i, width, precision, result = 0;
    char temp;
    unsigned char flags, length;
    unsigned int (*specifier_function)(va_list, custom_buffer_t *,
                                       unsigned char, int, int, unsigned char);

    for (i = 0; *(format + i); i++)
    {
        length = 0;
        if (*(format + i) == '%')
        {
            temp = 0;
            flags = handle_custom_flags(format + i + 1, &temp);
            width = handle_custom_width(args, format + i + temp + 1, &temp);
            precision = handle_custom_precision(args, format + i + temp + 1,
                                                &temp);
            length = handle_custom_length(format + i + temp + 1, &temp);

            specifier_function = handle_custom_specifiers(format + i + temp + 1);
            if (specifier_function != NULL)
            {
                i += temp + 1;
                result += specifier_function(args, output, flags, width, precision, length);
                continue;
            }
            else if (*(format + i + temp + 1) == '\0')
            {
                result = -1;
                break;
            }
        }
        result += _custom_memcpy(output, (format + i), 1);
        i += (length != 0) ? 1 : 0;
    }
    clean_up_va_list(args, output);
    return (result);
}

/**
 * custom_printf - Outputs a formatted string.
 * @format: Character string to print - may contain directives.
 *
 * Return: The number of characters printed.
 */
int custom_printf(const char *format, ...)
{
    custom_buffer_t *output;
    va_list args;
    int result;

    if (format == NULL)
        return (-1);

    output = init_custom_buffer();
    if (output == NULL)
        return (-1);

    va_start(args, format);

    result = process_printf(format, args, output);

    return (result);
}

