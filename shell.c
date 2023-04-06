#include "shell.h"
#include <string/string.h>
#include <memory/memory.h>
#include "multiboot.h"
#include "devices/devices.h"
#include "cpu/cpu.h"
#include "pmm.h"

#define SHELL_CMD_MAX_LEN 256
#define SHELL_CMD_MAX_ARGS 10

struct ShellState
{
    multiboot_info_t* mbi;
    unsigned char cmd[SHELL_CMD_MAX_LEN];
    unsigned char* args[SHELL_CMD_MAX_ARGS];
    uint32_t args_count;
    uint32_t cmd_len;
};

struct ShellState shell = {0};

void shell_parse_command()
{
    const char* delim = " ";
    char* token = strtok(shell.cmd, delim);
    shell.args_count = 0;
    while (!str_empty_or_whitespace(token) && shell.args_count < SHELL_CMD_MAX_ARGS)
    {
        shell.args[shell.args_count++] = token;
        token = strtok(0, delim);
    }
}

void shell_get_command()
{
    int i = 0;
    while (true)
    {
        // wait for key (KEYBOARD_STATUS_PORT)
        // NOTE: we are polling for keyboard input which is not a great idea for CPU usage
        // we should use IRQ for it
        uint16_t scancode = keyboard_wait_scancode();
        if(!(scancode & 0x80))
        {
            unsigned char key = keyboard_scancode_to_assci_us(scancode);            
            if(key == '\t')
            {
                continue;
            }
            else if(key == '\b')
            {
                if(i > 0)
                {
                    putch(key);
                    i--;
                }
                else
                {
                    continue;
                }
            }
            else if(key == '\n')
            {
                if(i > 0)
                {
                    shell.cmd[i] = '\0';
                    putch(key);
                    shell.cmd_len = i;
                    shell_parse_command();
                    return;
                }
            }
            else if(isalnum(key) || key == ' ' || key == '-' || key == '_')
            {
                if(key == ' ' && i == 0) continue;
                shell.cmd[i++] = key;
                putch(key);
            }
        }
        else
        {
            // a mod key is also pressed
        }
    }
}

void shell_listen()
{
    puts("\t~ Jin OS v1.0 ~\n");
    memset(shell.cmd, 0, SHELL_CMD_MAX_LEN);
    while (1)
    {
        puts(">");
        shell_get_command();
        if(shell.args_count == 0)
        {
            // if the user just pressed enter
            continue;
        };

        char* command = shell.args[0];
        
        if(strcmp(command, "help") == 0)
        {
            puts("Help Command....\n");
        }
        else if(strcmp(command, "cls") == 0)
        {
            cls();
        }
        else if(strcmp(command, "reset") == 0)
        {
            // send a reset from keyboard to reset pc
            outportb(0x64, 0xFE);
        }
        else if(strcmp(command, "meminfo") == 0)
        {
            uint32_t totalMemorySize = 1024 + shell.mbi->mem_lower + shell.mbi->mem_upper;
            printf("Total Memory: %d MB\n", totalMemorySize / 1024);

            printf("PMM regions initialized: %d blocks\n\tused blocks: %d\n\tfree blocks: %d\n",
		        pmm_get_block_count (),  pmm_get_use_block_count (), pmm_get_free_block_count () );

            if (CHECK_FLAG (shell.mbi->flags, 6))
            {
                for 
                (
                    multiboot_memory_map_t* mmap = (multiboot_memory_map_t *) shell.mbi->mmap_addr;
                    (unsigned long) mmap < (shell.mbi->mmap_addr + shell.mbi->mmap_length);
                    mmap = (multiboot_memory_map_t *) ((unsigned long) mmap + mmap->size + sizeof (mmap->size))
                )
                {
                    // only showing available memory 
                    if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
                    {
                        uint32_t* base = (uint32_t*)((uint32_t)(mmap->addr & 0xffffffff));
                        size_t size = (size_t)(mmap->len & 0xffffffff);
                        printf ("PMM REGION: base_addr = 0x%08x, length = 0x%08x\n", base, size);
                    }
                }
            }
        }
        else if (strcmp(command, "datetime") == 0)
        {
            uint8_t seconds = cmos_read(0x00);
            uint8_t minutes = cmos_read(0x02);
            uint8_t hours   = cmos_read(0x04);
            uint8_t day     = cmos_read(0x07);
            uint8_t month   = cmos_read(0x08);
            uint8_t year    = cmos_read(0x09);

            // convert the binary-coded decimal values to decimal
            seconds = (seconds & 0x0f) + ((seconds >> 4) * 10);
            minutes = (minutes & 0x0f) + ((minutes >> 4) * 10);
            hours   = (hours   & 0x0f) + ((hours   >> 4) * 10);
            day     = (day     & 0x0f) + ((day     >> 4) * 10);
            month   = (month   & 0x0f) + ((month   >> 4) * 10);
            year    = (year    & 0x0f) + ((year    >> 4) * 10);

            //    hours += 5;

            if (hours >= 24) 
            {
                hours -= 24;
                day++;
            } 
            else if (hours < 0)
            {
                hours += 24;
                day--;
            }

            printf("Time: %02d:%02d:%02d Date: %02d/%02d/%d\n", hours, minutes, seconds, day, month, year + 2000);
        }
        else if (strcmp(command, "showmem") == 0)
        {
            uint32_t* base = (uint32_t*)0x100000;
            if(shell.args_count > 1)
            {
                long int got = strtol(shell.args[1], 0, 0);
                if(got > 0)
                {
                    base = (uint32_t*)(got);
                }
            }

            printf("%s  %s                                      %s\n", "Address", "Hexadecimal", "ASCII"); // Header row
            for (int i = 0; i < 256; i++) { // Display 256 bytes
                if (i % 16 == 0) { // Start of a new row
                    printf("0x%x ", (unsigned int)(base + i)); // Display address in hexadecimal format
                }
                printf("%02x ", (unsigned char)*(base + i)); // Display byte value in hexadecimal format
                if ((i + 1) % 16 == 0) { // End of a row
                    printf(" "); // Add a space between hexadecimal and ASCII views
                    int j;
                    for (j = i - 15; j <= i; j++) { // Display ASCII characters for the current row
                        if (*(base + j) < 32 || *(base + j) > 126) { // Check if byte value is printable ASCII character
                            printf("."); // Display dot for non-printable character
                        } else {
                            printf("%c", *(base + j)); // Display ASCII character
                        }
                    }
                    printf("\n"); // Start a new line
                }
            }
        }
        else
        {
            puts("Unknown Command : ");
            putsn((unsigned char*)command, 75 - 18);
            puts("...\n");
        }

        // reset the command buffer
        memset(shell.cmd, 0, SHELL_CMD_MAX_LEN); 
    }
}

void shell_init(uint32_t* mbi)
{
    shell.mbi = (multiboot_info_t*)mbi;
    shell_listen();
}
