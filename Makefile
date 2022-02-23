tools:
	gcc -o record_mem_ip sv_to_mem_ip.c
	gcc -o record_mem_mem_ip sv_to_mem_mem_ip.c

datasets:
	./make_mem_to_ip
	./make_mem_mem_to_ip

clean_dataset:
	rm -rf mem_ip_pairs/
	rm -rf mem_mem_ip_tuples/


