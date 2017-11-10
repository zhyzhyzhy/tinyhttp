# tinyhttp

# 实现功能
* Reactor模式  
* 基于Libevent事件驱动库  
* 内存池  
* 线程池  

# 使用

## 配置文件参数
* index_file_name     
  主页名字  
* sub_reactor     
  子Reactor个数  
* target_dir     
  存放主要的root目录  
* thread_num     
  线程池线程个数  
* host     
  绑定的本地host  
* port   
  端口设定  

例：
```
index_file_name = index.html
sub_reactor = 2
target_dir = /Users/zhuyichen/git/hexoblog/public/
thread_num = 10
host = 127.0.0.1
port = 4002
```


