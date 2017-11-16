tinyhttp:
	gcc -O3 server/config.c server/handle.c server/helper.c server/mempool.c server/mysocket.c server/parser.c server/sig.c server/subreactor.c server/threadpool.c server/util.c server/main.c -lpthread -levent -levent_pthreads -o tinyhttp
