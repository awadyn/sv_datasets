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
	FILE *sv_seq, *mem_ip_pair;
	sv_seq = fopen(infile, "rb");
	
	// for each input sv trace, produce 1001 mem_to_next_ip mappings
	//
	char suffix[5];
	uint32_t row = 0;
	uint16_t pc;
	uint8_t regs[6];
	uint8_t mem[65536];

	// skip 1st 8 bytes (these are initial register values)
	//
	fread(&pc, sizeof(uint16_t), 1, sv_seq);
	fread(&regs, sizeof(uint8_t), 6, sv_seq);

	// start recording mem_{row} and pc_{row+1} pairs
	//
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
		mem_ip_pair = fopen(outfile, "w");

		// read mem_{row}
		fread(&mem, sizeof(uint8_t), 65536, sv_seq);
		// write mem_{row}
		fwrite(&mem, sizeof(uint8_t), 65536, mem_ip_pair);
	
		if (feof(sv_seq)) break;	// assert(not last row)

		// read regs_{row + 1}
		fread(&pc, sizeof(uint16_t), 1, sv_seq);
		fread(&regs, sizeof(uint8_t), 6, sv_seq);
		// write pc_{row + 1}
		fwrite(&pc, sizeof(uint16_t), 1, mem_ip_pair);
	
		{
			if (ferror(sv_seq)) {
				fprintf(stderr, "ERROR: error reading from %s.", infile);
				return -1;
			}
			if (ferror(mem_ip_pair)) {
				fprintf(stderr, "ERROR: error writing to %s.", outfile);
				return -1;
			}
		}

		// close output file for this mem-ip pair	
		fclose(mem_ip_pair);
		row ++;
	}

	assert(feof(sv_seq));
	fclose(sv_seq);

	return 0;
}





