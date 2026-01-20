.PHONY:all
all:chat_client chat_server
chat_client:ChatClient.cc
	g++ -o $@ $^ -std=c++17
chat_server:ServerMain.cc
	g++ -o $@ $^ -std=c++17

.PHONY:clean
clean:
	rm -rf chat_client chat_server