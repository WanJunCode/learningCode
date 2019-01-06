#/bin/bash
echo begin
echo this is a test line > input.txt
exec 3<input.txt
cat <&3
echo end
