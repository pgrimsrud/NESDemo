.pushseg
.segment "MUSIC"
.export _music
_music:
    ;.incbin "starwars.nsf", $80 ;just need the data not the header
    ;.incbin "Z2.nsf", $80 ;just need the data not the header
    .incbin "test.nsf"
.popseg

.export _pMusicInit
_pMusicInit:
    jmp $8080
    rts
    
.export _pMusicPlay
_pMusicPlay:
    lda #0
    ldx #0
    jmp $8084
    rts

