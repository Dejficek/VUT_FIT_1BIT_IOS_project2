#!/bin/bash

# testuje spravnou sekvenci cislovani akci a format vystupu (pouze syntax)
# bez zaruky, muze obsahovat chyby

outf=$1

# odstraneni bilych znaku
tr -d " \t" < ${outf} > $$

# test cislovani akci
# tiskne radky s chybnym cislovanim
cat $$ | awk -F":" ' { c++; if (c != $1) print NR, " => ", $1, " : chyba v cislovani akci"; } '

declare -a lines
lines[0]='^[1-9][0-9]*:HACK[1-9][0-9]*:starts$'
lines[1]='^[1-9][0-9]*:HACK[1-9][0-9]*:waits:[0-9][0-9]*:[0-9][0-9]*$'
lines[2]='^[1-9][0-9]*:HACK[1-9][0-9]*:boards:[0-9][0-9]*:[0-9][0-9]*$'
lines[3]='^[1-9][0-9]*:HACK[1-9][0-9]*:leavesqueue:[0-9][0-9]*:[0-9][0-9]*$'
lines[4]='^[1-9][0-9]*:HACK[1-9][0-9]*:memberexits:[0-9][0-9]*:[0-9][0-9]*$'
lines[5]='^[1-9][0-9]*:HACK[1-9][0-9]*:captainexits:[0-9][0-9]*:[0-9][0-9]*$'
lines[6]='^[1-9][0-9]*:HACK[1-9][0-9]*:isback$'

lines[7]='^[1-9][0-9]*:SERF[1-9][0-9]*:starts$'
lines[8]='^[1-9][0-9]*:SERF[1-9][0-9]*:waits:[0-9][0-9]*:[0-9][0-9]*$'
lines[9]='^[1-9][0-9]*:SERF[1-9][0-9]*:boards:[0-9][0-9]*:[0-9][0-9]*$'
lines[10]='^[1-9][0-9]*:SERF[1-9][0-9]*:leavesqueue:[0-9][0-9]*:[0-9][0-9]*$'
lines[11]='^[1-9][0-9]*:SERF[1-9][0-9]*:memberexits:[0-9][0-9]*:[0-9][0-9]*$'
lines[12]='^[1-9][0-9]*:SERF[1-9][0-9]*:captainexits:[0-9][0-9]*:[0-9][0-9]*$'
lines[13]='^[1-9][0-9]*:SERF[1-9][0-9]*:isback$'

# kontrola sytaxe vystupu
# vytiskne radky, ktere neodpovidaji formatu vytupu
echo "=== radky, ktere neodpovidaji formatu vystupu"

for i in `seq 0 13`
do
    echo "/${lines[$i]}/d" >> $$-sed
done

sed -f $$-sed $$

# kontrola chybejicich vystupu
# tiskne informaci, ktery text ve vystupu chybi
echo "=== chybejici vystupy"
echo ">> chybejici leaves queue a is back "
echo ">>>> nemusi nutne znamenat chybu, za urcitych okolnosti nemusi ve vystupu byt "
echo ">>>> pokud chybi leaves queue, musi chybet i is back "
echo ">> chybejici boards u jednoho typu procesu"
echo ">>>> nemusi byt chyba, dany typ procesu se nemusi nikdy stat kapitanem"
echo ">>>> pokud chybi boards, musi chybet i captain exits u stejneho typu procesu"

for i in `seq 0 13`
do
    cat $$ | grep "${lines[$i]}" >/dev/null || echo "${lines[$i]}"
done


rm $$*
