.PHONY:all
all:UdpClient UdpServer
UdpClient:UdpClient.cc
	g++ -o $@ $^ -std=c++14
UdpServer:UdpServer.cc
	g++ -o $@ $^ -std=c++14
.PHONY:clean
clean:
	rm -f UdpClient UdpServer