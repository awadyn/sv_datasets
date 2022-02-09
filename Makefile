tool:
	gcc -o make_dataset sv_to_mem_ip.c

dataset:
	./loop_dataset

clean_dataset:
	rm mem_ip_pairs/*


