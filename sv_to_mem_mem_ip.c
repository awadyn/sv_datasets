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
	//
	char *infile = argv[1];
	FILE *sv_seq, *mem_mem_ip_tuple;
	sv_seq = fopen(infile, "rb");
	
	// for each input sv trace, produce 1001 mem_to_next_mem mappings
	//
	char suffix[5];
	uint32_t row = 0;
	uint16_t pc, pc_next;
	uint8_t regs[6];
	uint8_t mem[65536];
	int counter = 0;

	// start recording mem_{row} and mem_{row+1} pairs
	//
	uint8_t first_itr = 1;
	while (!feof(sv_seq)) {

		// creating output file...
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
		mem_mem_ip_tuple = fopen(outfile, "w");

		// read pc and skip other regs
		//
		// if pc_next exists, then pc_{row} was read in the previous iteration
		// else, this is the first iteration
		if (first_itr) {
			fread(&pc, sizeof(uint16_t), 1, sv_seq);
			fread(&regs, sizeof(uint8_t), 6, sv_seq);
		} else pc = pc_next;

		// read and write mem_{row}
		//
		// if mem exists, mem_{row} was read in the previous iteration
		// else, this is the first iteration
		if (first_itr) {
			fread(&mem, sizeof(uint8_t), 65536, sv_seq);
			first_itr = 0;
		}
		fwrite(&mem, sizeof(uint8_t), 65536, mem_mem_ip_tuple);
	
		if (feof(sv_seq)) break;	// assert(not last row)

		// read next pc and skip other regs
		fread(&pc_next, sizeof(uint16_t), 1, sv_seq);
		fread(&regs, sizeof(uint8_t), 6, sv_seq);

		// read and write mem_{row + 1}
		//
		fread(&mem, sizeof(uint8_t), 65536, sv_seq);
		fwrite(&mem, sizeof(uint8_t), 65536, mem_mem_ip_tuple);	

		// write pc
		//
		fwrite(&pc, sizeof(uint16_t), 1, mem_mem_ip_tuple);

		{
			if (ferror(sv_seq)) {
				fprintf(stderr, "ERROR: error reading from %s.", infile);
				return -1;
			}
			if (ferror(mem_mem_ip_tuple)) {
				fprintf(stderr, "ERROR: error writing to %s.", outfile);
				return -1;
			}
		}

		// close output file for this mem-ip pair	
		fclose(mem_mem_ip_tuple);
		row ++;
	}

	fclose(sv_seq);

	return 0;
}





