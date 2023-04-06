#include "string.h"

size_t strlen(const char *str)
{
    size_t retval;
    for(retval = 0; *str != '\0'; str++) retval++;
    return retval;
}

int strcmp(unsigned char* s1, unsigned  char* s2)
{
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

char* strpbrk(const char *s1, const char *s2)
{
    const char *p1, *p2;
    for (p1 = s1; *p1 != '\0'; p1++) {
        for (p2 = s2; *p2 != '\0'; p2++) {
            if (*p1 == *p2) {
                return (char *) p1;
            }
        }
    }
    return NULL;
}
size_t strcspn(const char *s, const char *reject) {
    const char *p = s;
    const char *r;

    // Iterate over the string until a matching character is found
    while (*p != '\0') {
        r = reject;
        while (*r != '\0') {
            if (*p == *r) {
                return p - s;
            }
            r++;
        }
        p++;
    }

    // If no matching character is found, return the length of the string
    return p - s;
}
size_t strspn(const char *s, const char *accept) {
    const char *p = s;
    const char *a;

    // Iterate over the string until a non-matching character is found
    while (*p != '\0') {
        a = accept;
        while (*a != '\0') {
            if (*p == *a) {
                break;
            }
            a++;
        }

        // If no match was found, return the current position in the string
        if (*a == '\0') {
            return p - s;
        }

        p++;
    }

    // If the end of the string is reached, return the length of the string
    return p - s;
}

// taken from glibc implementation
char* __strtok_r (char *s, const char *delim, char **save_ptr)
{
  char *end;
  if (s == NULL)
    s = *save_ptr;
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }
  /* Scan leading delimiters.  */
  s += strspn (s, delim);
  if (*s == '\0')
    {
      *save_ptr = s;
      return NULL;
    }
  /* Find the end of the token.  */
  end = s + strcspn (s, delim);
  if (*end == '\0')
    {
      *save_ptr = end;
      return s;
    }
  /* Terminate the token and make *SAVE_PTR point past it.  */
  *end = '\0';
  *save_ptr = end + 1;
  return s;
}

char* strtok (char *s, const char *delim)
{
  static char *olds;
  return __strtok_r (s, delim, &olds);
}

bool str_empty_or_whitespace(char* str)
{
     if (!str || !*str) {
        return true;
    }
    while (*str) {
        if (!isspace(*str)) {
            return false;
        }
        str++;
    }
    return true;
}

char* str_trimfront(char* str)
{
    while(*str)
    {
        if(!isspace(*str)) break;
        str++;
    }

    return str;
}

int atoi(const char *str)
{
    int result = 0;
    int sign = 1;
    int i = 0;

    // Check for leading sign (+/-)
    if (str[0] == '-') {
        sign = -1;
        i++;
    }
    else if (str[0] == '+') {
        i++;
    }

    // Iterate over string and build integer value
    for (; str[i] != '\0'; i++) {
        if (str[i] < '0' || str[i] > '9') {
            break;
        }
        result = result * 10 + (str[i] - '0');
    }

    return sign * result;
}

void itoa (char *buf, int base, int d)
{
  char *p = buf;
  char *p1, *p2;
  unsigned long ud = d;
  int divisor = 10;
  
  /*  If %d is specified and D is minus, put ‘-’ in the head. */
  if (base == 'd' && d < 0)
    {
      *p++ = '-';
      buf++;
      ud = -d;
    }
  else if (base == 'x')
    divisor = 16;

  /*  Divide UD by DIVISOR until UD == 0. */
  do
    {
      int remainder = ud % divisor;
      
      *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
  while (ud /= divisor);

  /*  Terminate BUF. */
  *p = 0;
  
  /*  Reverse BUF. */
  p1 = buf;
  p2 = p - 1;
  while (p1 < p2)
    {
      char tmp = *p1;
      *p1 = *p2;
      *p2 = tmp;
      p1++;
      p2--;
    }
}

bool isalpha(uint8_t c)
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

bool isalnum(uint8_t c) 
{
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'));
}

bool isspace(uint8_t c)
{
  return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v');
}

bool isdigit(uint8_t c)
{
  return (c >= '0' && c <= '9');
}

uint8_t tolower(uint8_t c) 
{
    return (c >= 'A' && c <= 'Z') ? c + ('a' - 'A') : c;
}

uint8_t toupper(uint8_t c) 
{
    return (c >= 'a' && c <= 'z') ? c - ('a' - 'A') : c;
}

long int strtol(const char *nptr, char **endptr, int base)
{
    char *s = (char*)nptr;
    char *end;
    long int value = 0;
    int negative = 0;

    // Skip leading whitespace
    s = str_trimfront(s);

    // Check for optional sign
    if (*s == '+') {
        s++;
    }
    else if (*s == '-') {
        negative = 1;
        s++;
    }

    // Determine the base if not specified
    if (base == 0) {
        if (*s == '0') {
            if (s[1] == 'x' || s[1] == 'X') {
                base = 16;
                s += 2;
            }
            else {
                base = 8;
                s++;
            }
        }
        else {
            base = 10;
        }
    }

    // Parse the number
    end = (char *)s;
    while (*end != '\0') {
        if (isdigit(*end)) {
            int digit = *end - '0';
            if (digit >= base) {
                break;
            }
            value = value * base + digit;
        }
        else if (isalpha(*end)) {
            int digit = tolower(*end) - 'a' + 10;
            if (digit >= base) {
                break;
            }
            value = value * base + digit;
        }
        else {
            break;
        }
        end++;
    }

    // Set the endptr if provided
    if (endptr != NULL) {
        *endptr = end;
    }

    return (negative != 0) ? -value : value;
}