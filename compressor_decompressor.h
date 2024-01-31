#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <time.h>

#define SEQ_OG_SIZE 1280
#define BLOCK_SIZE 64
#define resolution 16
#define MIN_LIMIT 0
#define MAX_LIMIT 65000
#define THRESHOLD 0
#define IN_FILE_NAME "valgring2.txt"
#define OUT_FILE_NAME "valgrind.txt"
int final_decoded_seq[SEQ_OG_SIZE];
int decoded_seq[SEQ_OG_SIZE];
extern int final_decoded_index;
extern int decoded_index;

void entropy_decoder(char *seq, int size);
int *post_processor(int *decoded_seq);
int *prop(int *seq, int index, int *x);

char *zero_block(int *sequence, int length, char *result, char *encoded);
void breaker(int *sequence, int range, int blocks, int block[blocks][BLOCK_SIZE]);
int *pre_processor(int *block, int len, int *delta);
// int *pre_processor(int *block, int len, int *seq_delayed, int *delta);
int *rand_seq_og(int min, int max, int range, int *seq_og);
void FS_element(int zeros, char *fs_element);
char *code_select(int *block, char *encoded_block, int index, int block_size);
char *split_sample(int *block, int k, int size, char *encoded, int index);
char *second_extension(int *sequence, int block_size, char *second_extension_encoded);
void executor(int *seq_og, int seq_og_size, int blocks, int *encoded_seq);
// void executor(int *seq_og, int seq_og_size, int blocks, int *delta, int *encoded_seq);
// void executor(int *seq_og, int seq_og_size, int blocks, int blocks_sequence[blocks][BLOCK_SIZE], int *delta, int *encoded_seq);
char *FS_block(int *block, char *fs_encoded, int index);
char *int_to_binary(int value, char *binary_p);
char *zfill(int number, int zero_resolution, char *binary_p);
int char_binary_to_decimal(char *msb, int len);
char *no_compression(int *block, char *no_compression);
char *minimum(char *a, char *b);
int *dec(char *seq, int *e);

int sum(int *dec_seq, int index, int total);
int FS_decoder(char *seq, int block_size_t, int seq_size, int *dec_seq, int dec_seq_index, int dec_seq_size);
int FS_block_decoder(char *seq, int seq_size, char *first_sample);
int no_compression_decoder(char *seq);
int split_sample_decoder(char *seq, int k, char *first_sample);
int second_extension_decoder(char *seq, char *first_sample);
int zero_block_decoder(char *seq, int start_index, int seq_length);

char *joiner(int *seq, int seq_size, char *s);
// char *itoa(long int value, char *result, int base);