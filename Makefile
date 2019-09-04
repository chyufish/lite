VPATH=src
cc = g++
OBJ = lite.o lite_tcp.o lite_event.o connection.o request_handler.o http_parser.o request.o log.o

lite : $(OBJ)
	$(cc) -o lite $(OBJ) -pthread

lite.o : lite_tcp.h lite_event.h request_handler.h request.h connection.h \
	http_parser.h thread_pool.h buffer.h connection_manager.h
lite_tcp.o : lite_tcp.h lite_event.h request_handler.h request.h connection.h \
	http_parser.h log.h thread_pool.h buffer.h connection_manager.h
lite_event.o : lite_event.h connection.h request_handler.h request.h http_parser.h log.h
connection.o : connection.h request_handler.h request.h http_parser.h lite_event.h buffer.h \
	connection_manager.h
request_handler.o : request_handler.h request.h log.h buffer.h
http_parser.o : http_parser.h request.h buffer.h
request.o : request.h buffer.h
log.o : log.h

.PHONY : clean
clean:
	rm $(OBJ)
