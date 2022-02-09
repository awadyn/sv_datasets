#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char* argv[]) {
	if (argc == 1) {
		fprintf(stderr, "Missing sv file.\n");
		return -1;
	}

	// infile: input sv trace (may contain >= 1 sv)
	char *infile = argv[1];
	FILE *sv_seq, *mem_ip_pair;
	sv_seq = fopen(infile, "rb");
	
	// for each input sv trace, produce 1001 mem_to_next_ip mappings
	char suffix[5];
	uint32_t row = 0;
	while (!feof(sv_seq)) {
		char *outfile = (char *)malloc(16);
		sprintf(suffix, "%d", row);

		int i = 0;
		char *itr = infile;
		while (*itr != '.') {
			outfile[i] = *itr;
			itr ++;
			i ++;
		}
		outfile[i+0] = '_';
		itr = suffix;
		while (*itr != 0) {
			outfile[++i] = *itr;
			itr ++;
		}

		mem_ip_pair = fopen(outfile, "w");

		uint16_t pc;
		uint8_t regs[6];
		uint8_t mem[65536];
		uint8_t padding = 0;

		// read registers
		fread(&pc, sizeof(uint16_t), 1, sv_seq);
		fread(&regs, sizeof(uint8_t), 6, sv_seq);
		// write rpc
		fwrite(&pc, sizeof(uint16_t), 1, mem_ip_pair);
		fprintf(mem_ip_pair, "\n");
		
		// read memory
		fread(&mem, sizeof(uint8_t), 65536, sv_seq);
		// write memory
		fwrite(&mem, sizeof(uint8_t), 65536, mem_ip_pair);

		fclose(mem_ip_pair);
		row ++;
	}

	fclose(sv_seq);

	return 0;


}
