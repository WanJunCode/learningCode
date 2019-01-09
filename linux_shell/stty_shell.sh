#/bin/bash
## stty -echo 禁止将输出发送到终端

echo "sh name is [${0}]"
echo -e "Enter password: "
stty -echo
read password
stty echo
echo
echo "Password : ${password}"
