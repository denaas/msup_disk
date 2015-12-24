ADR_CLIENT1 = ./Client1-Debug/interface
ADR_CLIENT2 = ./Client2_Debug/Client2
work_client1:
	sudo $(ADR_CLIENT1)
work_client2:
	$(ADR_CLIENT2)
work_dem:
	g++ ./dem.cpp -o dem
	sudo ./dem