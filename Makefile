ADR_CLIENT1 = ./Client1-Debug/interface
ADR_CLIENT2 = ./Client2_Debug/Client2

cipher.o: cipher.cpp 
	g++ -c $< -lssl -lcrypto -o $@

work_client1:
	sudo $(ADR_CLIENT1)
work_client2:
	$(ADR_CLIENT2)
work_dem: dem.cpp cipher.o
	g++ $^ -lssl -lcrypto -o dem
	sudo ./dem nikolay