#/bin/bash
get_pid()
{
    process_name=$1
    text=`ps -A|grep $process_name`
    # 去除开头的空格
    text=`echo $text | sed -e 's/^[ \t]*//g'`

    if [ "${text}" = "" ] ; then
        pid=0
        echo ${pid}
        return 0
    fi

    pos=`expr index "$text" " "`
    pos=`expr $pos - 1`

    pid=`echo $text | cut -c 1-$pos`
    echo ${pid}
    return 0
}

get_mem()
{
    process_id=$1
    text=`cat /proc/$process_id/status | grep VmRSS`
    if [ "${text}" = "" ] ; then
        memory=0
        echo ${memory}
        return 0
    fi

    pos=`expr index "$text" " "`
    text=`echo $text | cut -c $pos-`

    pos=`expr index "$text" " "`
    pos=`expr $pos - 1`
    memory=`echo $text | cut -c 1-$pos`

    echo ${memory}
    return 0
}

PROCESS_NAME=$1
pid=$(get_pid $PROCESS_NAME)

if [ "${pid}" = "0" ] ; then
    max_memory=0
else
    max_memory=$(get_mem ${pid})
fi

echo "pid=${pid},max_memory=${max_memory}"

while [ true ] ; do
    sleep 1s

    pid=$(get_pid $PROCESS_NAME)
    if [ "${pid}" = "0" ] ; then
        max_memory=0
        continue
    fi

    current_memory=$(get_mem ${pid})
    if [ "${current_memory}" = "0" ] ; then
        continue
    fi

    # echo -e "test ${current_memory} "

    if [ ${current_memory} -gt ${max_memory} ]
    then
        echo
        echo ------------------------------------------
        date
        diff=`expr ${current_memory} - ${max_memory}`
        echo ${current_memory} - ${max_memory} = ${diff}
        max_memory=${current_memory}
    fi
done