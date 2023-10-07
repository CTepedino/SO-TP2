GLOBAL readKey


section .text
readKey:
    in al, 0x60
    ret
