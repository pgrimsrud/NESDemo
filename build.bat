C:\Users\pgrimsrud\NES\cc65\bin\cc65 -Oi test.c -o test.asm --add-source
C:\Users\pgrimsrud\NES\cc65\bin\ca65 test.asm
C:\Users\pgrimsrud\NES\cc65\bin\ca65 main.asm
C:\Users\pgrimsrud\NES\cc65\bin\ca65 asm4c.s
C:\Users\pgrimsrud\NES\cc65\bin\ld65 -C nes.cfg -o test.nes main.o test.o --mapfile test.map asm4c.o "C:\Users\pgrimsrud\NES\cc65\lib\nes.lib"
