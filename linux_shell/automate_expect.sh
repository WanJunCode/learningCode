#!/usr/bin/expect
spawn ./interactive.sh
expect "Enter number:"
send "1\n"
expect "Enter name:"
send "wanjun\n"
expect eof
