# learningCode
平时碰到并学习的开源代码，自己写好注释

Tool 文件夹中积累平时一些 api 的封装


STL 容器 函数列表

std::vector
  元素访问
    at                  访问指定的元素，同时进行越界检查
    operator[]          访问指定的元素
    front               访问第一个元素
    back                访问最后一个元素
    data                返回指向内存中数组第一个元素
  迭代器
    begin   cbegin      
    end     cend
    rbegin  crbegin
    rend    crend
  容量
    empty               检查容器是否为空
    size                返回容纳的元素数
    max_size            返回可容纳的最大元素数
    reserve             预留存储空间
    capacity            返回当前存储空间能够容纳的元素数
    shrink_to_fit       通过释放未使用的内存减少内存的使用
  修改器
    clear               清除内容
    insert              插入元素
    emplace             原位构造元素
    erase               擦除元素
    push_back           将元素添加到容器末尾
    emplace_back        在容器尾部就地构造元素
    pop_back            移除末元素
    resize              改变容器中可存储元素的个数

std::queue
    front       访问第一个元素
    back        访问最后一个元素
    empty       检查容器是否为空
    size        返回容器的元素数量
    push        向队尾部插入元素
    emplace     于尾部原位构造
    pop         删除第一个元素

std::deque
  元素访问
    at
    operator[]
    front       访问第一个元素
    back        访问最后一个元素
  迭代器
    begin   cbegin
    end     cend
    rbegin  crbegin
    rend    crend
  容量
    empty
    size
    max_size    返回可容纳的最大元素数
    shrink_to_fit   通过释放未使用的内存减少内存的使用
  修改器
    clear       清除内容
    insert      插入元素
    emplace     原位构造元素
    erase       擦除元素
    push_back   将元素添加到容器末尾
    emplace_back在容器末尾就地构造元素
    pop_back    移除末元素
    push_front  插入元素到容器起始
    emplace_front 在容器头部就地构造元素
    pop_front   移除首元素
    resize      改变容器中可存储元素的个数

std::stack
  元素访问
    top         访问栈顶元素
  容量
    empty
    size
  修改器
    push        想栈顶插入元素
    emplace     于顶原位构造元素
    pop         删除栈顶元素

关联容器
std::map
  元素访问
    at
    operator[]
  迭代器
    begin   cbegin
    end     cend
    rbegin  crbegin
    rend    crend
  容量
    empty
    size
    max_size            返回可容纳的最大元素数
  修改器
    clear
    insert
    insert_or_assign
    emplace
    emplace_hint
    try_emplace
    erase
    swap
    extract
    merge
  查找
    count
    find
    equal_range
    lower_bound
    upper_bound
  观察器
    key_comp
    value_comp