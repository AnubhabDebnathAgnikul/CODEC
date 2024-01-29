#include "compressor_decompressor.h"
int decoded_index = 0;
int final_decoded_index = 0;
extern int final_decoded_seq[SEQ_OG_SIZE];
extern int decoded_seq[SEQ_OG_SIZE];
char *joiner(int *seq, int seq_size, char *s)
{
    int l = seq[0];
    // printf("\n%d\n", l);
    char received_seq[l];
    memset(received_seq, '\0', sizeof(received_seq));
    // printf("\n before for loop\n");
    char buffer[33] = {'\0'};
    for (int i = 1; i < seq_size; ++i)
    {
        int rem_l = l - strlen(received_seq);
        memset(buffer, '\0', sizeof(buffer));
        int block = seq[i];
        if (rem_l >= resolution)
        {
            zfill(block, resolution, buffer);
        }
        else
        {
            zfill(block, rem_l, buffer);
        }
        strncat(received_seq, buffer, rem_l);
    }
    strcat(s, received_seq);
    // return s;
}

int *dec(char *seq, int *e)
{
    int N = resolution;
    int seq_len = strlen(seq);
    int blocked_seq_len = seq_len / N;
    int blocked_seq[blocked_seq_len];
    int i = 0;
    e[0] = seq_len;
    char block[N + 1];
    for (; i < blocked_seq_len + 1; i++)
    {
        memset(block, '\0', sizeof(block));
        strncpy(block, seq + i * N, N);
        block[N] = '\0';
        blocked_seq[i] = strtol(block, NULL, 2);
        e[i + 1] = blocked_seq[i];
    }
    // return e;
}
char *minimum(char *a, char *b)
{
    if (strlen(a) <= strlen(b))
    {
        // printf("\nsmall:%s\n", a);
        return a;
    }
    else
    {
        // printf("\nsmall:%s\n", b);
        return b;
    }
}
char *no_compression(int *block, char *no_compression_encoded)
{
    char option_id[100] = {'\0'};

    if (resolution == 1 || resolution == 2)
    {
        strcpy(option_id, "1");
    }
    else if (resolution == 3 || resolution == 4)
    {
        strcpy(option_id, "11");
    }
    else if (resolution > 4 && resolution <= 8)
    {
        strcpy(option_id, "111");
    }
    else if (resolution > 8 && resolution <= 16)
    {
        strcpy(option_id, "1111");
    }
    else if (resolution > 16 && resolution <= 32)
    {
        strcpy(option_id, "11111");
    }
    strcat(no_compression_encoded, option_id);
    char binary_p[64];
    for (int i = 0; i < 64; i++)
    {
        memset(binary_p, '\0', sizeof(binary_p));
        zfill(block[i], 16, binary_p);
        strcat(no_compression_encoded, binary_p);
    }
    // printf("\nno_compression_encoded:%s\n", no_compression_encoded);
    return no_compression_encoded;
}

int char_binary_to_decimal(char *msb, int len)
{
    // printf("n char to decimal\n");
    // printf("\n%s,%d\n", msb, len);
    char *ptr;
    ptr = msb;
    char str[200] = {'\0'};
    int dec = 0, bit_dec = 0, i = 0, exponent = 0, base = 2, result = 1;
    ;
    // for (int i = 0; i < len; i++)
    // {
    //     printf("ptr[%d]=%c,", i, ptr[i]);
    // }
    // printf("\n%s,%ld\n", ptr, strlen(ptr));
    for (; i < len; i++)
    {
        exponent = len - (i + 1);
        int n = msb[i] - '0';
        result = 1;
        if (n == 1)
        {
            while (exponent != 0)
            {
                result *= base;
                --exponent;
            }
            bit_dec = 1 * result;
            dec = dec + bit_dec;
        }
        else
        {
            dec = dec + 0;
        }
        // exponent--;
    }
    // printf("\n dec=%d\n", dec);
    return dec;
}
char *zfill(int number, int zero_resolution, char *binary_p)
{
    int bits = sizeof(number) * 8;
    char binary_arr[bits + 1];
    int_to_binary(number, binary_arr);
    int actual_resolution = strlen(binary_arr);
    char temp_zero[100] = {'\0'};
    if (actual_resolution < zero_resolution)
    {
        int n = zero_resolution - actual_resolution;
        memset(temp_zero, '0', n);
        temp_zero[n] = '\0';
        strcat(temp_zero, binary_arr);
        memcpy(binary_p, temp_zero, strlen(temp_zero));
        return binary_p;
    }
    else
    {
        memcpy(binary_p, binary_arr, strlen(binary_arr));
        // printf("\n%s\n", binary_arr);
        return binary_p;
    }
}

char *int_to_binary(int value, char *binary_p)
{
    int bits = sizeof(value) * 8;
    char rev_binary[bits + 1];
    int index = 0, i = 0, temp_value = value, bit = 0;

    if (value == 0)
    {
        binary_p[0] = '0';
        binary_p[1] = '\0';
        return binary_p;
    }
    while (temp_value)
    {
        int bit = (temp_value % 2);
        rev_binary[index] = bit + '0';
        temp_value /= 2;
        index++;
    }

    for (index -= 1; index >= 0; index--, i++)
    {
        binary_p[i] = rev_binary[index];
    }

    binary_p[i] = '\0';
    return binary_p;
}

char *FS_block(int *block, char *fs_encoded, int index)
{
    char binary_p[BLOCK_SIZE] = {'\0'};
    memset(fs_encoded, '\0', sizeof(fs_encoded));
    if ((resolution == 3) || (resolution == 4))
    {
        strcat(fs_encoded, "01");
    }
    else if ((resolution > 4) && (resolution <= 8))
    {
        strcat(fs_encoded, "001");
    }
    else if ((resolution > 8) && (resolution <= 16))
    {
        strcat(fs_encoded, "0001");
    }
    else if ((resolution > 16) && (resolution <= 32))
    {
        strcat(fs_encoded, "00001");
    }
    zfill(block[0], 16, binary_p);
    strcat(fs_encoded, binary_p);
    for (int i = 1; i < BLOCK_SIZE; i++)
    {
        memset(binary_p, '\0', sizeof(binary_p));
        zfill(1, block[i] + 1, binary_p);
        strcat(fs_encoded, binary_p);
        memset(binary_p, '\0', sizeof(binary_p));
    }
}
char *second_extension(int *sequence, int block_size, char *second_extension_encoded)
{
    char binary_p[64] = {'\0'};
    char option_id[100] = {'\0'};
    sequence[block_size] = sequence[block_size - 1];
    int paired_list[(block_size - 1) / 2][2];
    for (int i = 1; i < block_size; i += 2)
    {
        paired_list[i / 2][0] = sequence[i];
        paired_list[i / 2][1] = sequence[i + 1];
    }

    if (resolution == 1 || resolution == 2)
    {
        zfill(1, 2, option_id);
    }
    else if (resolution == 3 || resolution == 4)
    {
        zfill(1, 3, option_id);
    }
    else if (resolution > 4 && resolution <= 8)
    {
        zfill(1, 4, option_id);
    }
    else if (resolution > 8 && resolution <= 16)
    {
        zfill(1, 5, option_id);
    }
    else if (resolution > 16 && resolution <= 32)
    {
        zfill(1, 6, option_id);
    }
    int n = strlen(option_id);
    memset(second_extension_encoded, '\0', sizeof(second_extension_encoded));
    strcat(second_extension_encoded, option_id);

    zfill(sequence[0], 16, binary_p);
    strcat(second_extension_encoded, binary_p);

    float gamma = 0;
    float gamma_list[(block_size - 1) / 2];
    char fs_element[64] = {'\0'};
    int *pair;
    for (int i = 0; i < ((block_size - 1) / 2); i++)
    {
        pair = paired_list[i];
        gamma = (0.5 * (pair[0] + pair[1]) * (pair[0] + pair[1] + 1) + pair[1]);
        if (gamma + 1 <= 44)
        {
            FS_element(gamma + 1, fs_element);
            strcat(second_extension_encoded, fs_element);
        }
        else
        {
            memset(second_extension_encoded, '\0', sizeof(second_extension_encoded));
            second_extension_encoded[0] = '\0';
            // if (second_extension_encoded[0] == '\0')
            // {
            //     printf("\nmemset clear\n");
            // }
            // printf("not executing second extension\n");
            return 0;
        }
        gamma_list[i] = gamma;
    }

    // printf("gamma list en: ");
    // for (int i = 0; i < ((block_size - 1) / 2); i++)
    // {
    //     printf("%f ", gamma_list[i]);
    // }
    // printf("\n");
    // printf("\nsec ext=%s\n", second_extension_encoded);
    return second_extension_encoded;
}

char *split_sample(int *block, int k, int size, char *encoded, int index)
{
    // printf("in split sample\n");
    char option_id[64], block_array[64];
    char msb[64], lsb[16];
    char binary[64];
    int n = resolution;
    char msb_fs[64];
    char msb_bunch[4096], lsb_bunch[4096];
    memset(msb_bunch, '\0', sizeof(msb_bunch));
    memset(lsb_bunch, '\0', sizeof(lsb_bunch));
    int msb_dec = 0;
    for (int sample = 1; sample < size; sample++)
    {
        msb_dec = 0;
        memset(binary, '\0', sizeof(binary));
        zfill(block[sample], n, binary);
        memset(lsb, '\0', sizeof(lsb));
        memset(msb, '\0', sizeof(msb));
        memcpy(lsb, (void *)&binary[n - k], n - (n - k));
        lsb[n - (n - k) + 1] = '\0';
        memcpy(msb, (void *)&binary[0], (n - k));
        msb[(n - k) + 1] = '\0';
        // printf("\nmsb_len=%ld,lsb_len=%ld,msb=%s,lsb=%s\n", strlen(msb), strlen(lsb), msb, lsb);
        msb_dec = char_binary_to_decimal(msb, strlen(msb));
        // printf("\nmsb_dec=%d\n", msb_dec);
        memset(msb_fs, '\0', sizeof(msb_fs));
        FS_element(msb_dec + 1, msb_fs);
        strcat(msb_bunch, msb_fs);
        strcat(lsb_bunch, lsb);
        // printf("\nmsb_bunch=%s,lsb_bunch=%s\n", msb_bunch, lsb_bunch);
    }
    // printf("\nmsb_bunch=%s,lsb_bunch=%s\n", msb_bunch, lsb_bunch);
    memset(option_id, '\0', sizeof(option_id));
    if ((resolution == 3) || (resolution == 4))
    {
        zfill(k + 1, 2, option_id);
    }
    else if ((resolution > 4) && (resolution <= 8))
    {
        zfill(k + 1, 3, option_id);
    }
    else if ((resolution > 8) && (resolution <= 16))
    {
        zfill(k + 1, 4, option_id);
    }
    else if ((resolution > 16) && (resolution <= 32))
    {
        zfill(k + 1, 5, option_id);
    }
    memset(encoded, '\0', sizeof(encoded));
    strcat(encoded, option_id);
    memset(block_array, '\0', sizeof(block_array));
    zfill(block[0], 16, block_array);
    strcat(encoded, block_array);
    memset(block_array, '\0', sizeof(block_array));
    strcat(encoded, msb_bunch);
    strcat(encoded, lsb_bunch);
    // printf("\nsplit sample:%s\n", encoded);
    // printf("\nstr len=%ld\n", strlen(encoded));
    return encoded;
}

char *code_select(int *block, char *encoded_block, int index, int block_size)
{
    memset(encoded_block, '\0', sizeof(encoded_block));
    char smallest[4096] = {'\0'};
    char temp[4096] = {'\0'};
    char fs_encoded[4096] = {'\0'};
    char encoded_smallest[4096] = {'\0'};
    char *smallest_encoded = encoded_smallest;
    FS_block(block, fs_encoded, index);
    // printf("\nfs block:%s\n", fs_encoded);
    split_sample(block, 1, BLOCK_SIZE, smallest, index);
    for (int i = 1; i < resolution / 2; i++)
    {
        split_sample(block, i, BLOCK_SIZE, temp, index);
        if (strlen(temp) <= strlen(smallest))
        {
            memset(smallest, '\0', sizeof(smallest));
            memcpy(smallest, temp, sizeof(temp));
        }
    }
    char *split_sample_encoded = smallest;
    char second_extension_encoded[4096] = {'\0'};
    second_extension(block, block_size, second_extension_encoded);
    char no_compression_encoded[4096] = {'\0'};
    no_compression(block, no_compression_encoded);
    if (second_extension_encoded[0] == '\0')
    {
        // printf("\n in if block\n");
        // printf("\nfs_encoded=%s\n", fs_encoded);
        // printf("\split_sample_encoded=%s\n", split_sample_encoded);
        // printf("\no_compression_encoded=%s\n", no_compression_encoded);
        smallest_encoded = minimum(
            minimum(fs_encoded, split_sample_encoded), no_compression_encoded);
        // printf("\nsmallest:%s\n", smallest_encoded);
    }
    else
    {
        // printf("\nin else block\n");
        smallest_encoded = minimum(minimum(fs_encoded, split_sample_encoded), minimum(second_extension_encoded, no_compression_encoded));
        // printf("\nsmallest:%s\n", smallest_encoded);
    }
    memcpy(encoded_block, smallest_encoded, strlen(smallest_encoded));
    // strcat(encoded_block, smallest_encoded);
    // printf("\nend code_select=%s\n", encoded_block);
    return encoded_block;
}

void FS_element(int zeros, char *fs_element)
{
    int i = 0;
    if (0 == zeros)
    {
        fs_element[i] = '1';
        fs_element[i += 1] = '\0';
        return;
    }
    for (; i < zeros; i++)
    {
        if (i == (zeros - 1))
            fs_element[i] = '1';
        else
            fs_element[i] = '0';
    }
    fs_element[i] = '\0';
}

char *zero_block(int *sequence, int length, char *result, char *encoded)
{
    int zeros = 0;
    int ROS = 4;
    char option_id[20];
    int blocks = length / BLOCK_SIZE;
    int reminder_samples = length % BLOCK_SIZE;
    if (reminder_samples > 0)
    {
        blocks += 1;
    }
    int block_list[blocks][BLOCK_SIZE];
    breaker(sequence, length, blocks, block_list);
    int encoded_length = 0;
    int non_zero_block = 0;
    int index = strlen(encoded);
    int block[BLOCK_SIZE] = {'\0'};
    for (int i = 0; i < blocks; i++)
    {
        memset(block, '\0', sizeof(block));
        memcpy(block, block_list[i], sizeof(block_list[i]));
        non_zero_block = 0;
        for (int k = 0; k < BLOCK_SIZE; k++)
        {
            if (0 != block[i])
            {
                non_zero_block++;
                break;
            }
        }

        if (!non_zero_block)
        {
            zeros += 1;
        }
        else
        {
            if (0 != zeros)
            {
                if (zeros <= ROS)
                {
                    FS_element(zeros, encoded);
                }
                else
                {
                    FS_element(zeros + 1, encoded);
                }
            }
            zeros = 0;
            index = strlen(encoded);
            // code_select(block, encoded, index, BLOCK_SIZE);
        }
    }

    if (zeros != 0)
    {
        if (zeros <= ROS)
        {
            FS_element(zeros, encoded);
        }
        else
        {
            FS_element(zeros + 1, encoded);
        }
    }

    if (resolution == 1 || resolution == 2)
    {
        strcat(option_id, "000");
    }
    else if (resolution == 3 || resolution == 4)
    {
        strcat(option_id, "0000");
    }
    else if (resolution > 4 && resolution <= 8)
    {
        strcat(option_id, "00000");
    }
    else if (resolution > 8 && resolution <= 16)
    {
        strcat(option_id, "000000");
    }
    else if (resolution > 16 && resolution <= 32)
    {
        strcat(option_id, "0000000");
    }
    int len = strlen(option_id);
    strcat(result, option_id);
    strcat(result, encoded);
    int j = strlen(result);
    result[j + 1] = '\0';
    // printf("\nresult=%s\n", result);
    return result;
}

int *pre_processor(int *block, int element_size, int *seq_delayed, int *delta)
{
    int *seq = block;
    for (int i = 0; i < element_size; i++)
    {
        seq_delayed[i] = (i > 0) ? seq[i - 1] : THRESHOLD;
    }
    int diff[BLOCK_SIZE] = {0}, t[BLOCK_SIZE] = {0};
    for (int i = 0; i < element_size; i++)
    {
        diff[i] = seq[i] - seq_delayed[i];
    }

    int ymax = MAX_LIMIT;
    int ymin = MIN_LIMIT;
    int a, b, T;
    for (int i = 0; i < element_size; i++)
    {
        a = ymax - seq_delayed[i];
        b = seq_delayed[i] - ymin;
        T = (a < b) ? a : b;
        t[i] = T;

        if (0 <= diff[i] && diff[i] <= T)
        {
            delta[i] = 2 * diff[i];
        }
        else if (-T <= diff[i] && diff[i] < 0)
        {
            delta[i] = 2 * abs(diff[i]) - 1;
        }
        else
        {
            delta[i] = T + abs(diff[i]);
        }
    }

    // printf("\npre processed: ");
    for (int i = 0; i < element_size; i++)
    {
        // printf("%d ", delta[i]);
    }
    // printf("\n");
    // printf("pre processor done\n");
    return delta;
}

int *rand_seq_og(int min, int max, int range, int *seq_og) // generating 16 sameple sequence
{
    int i = 0;
    srand(time(NULL));

    for (; i < range - 640; i++)
    {
        seq_og[i] = rand() % 11 + 9820;
    }
    for (; i < range - 64; i++)
    {
        seq_og[i] = 0;
    }
    for (; i < range; i++)
    {
        seq_og[i] = rand() % 11 + 9820;
    }

    return seq_og;
}

void breaker(int *sequence, int range, int blocks, int block[blocks][BLOCK_SIZE])
{
    int N = BLOCK_SIZE; // Length of sublist
    int sequence_length = range;
    int num_of_blocks = sequence_length / N; // Number of sublists
    int remain_samples = sequence_length % N;
    int sub_block = 0, i = 0;
    // printf("\nblocked sequence\n");
    for (; sub_block < num_of_blocks; sub_block++, i += N)
    {
        for (int j = 0; j < N; j++)
        {
            block[sub_block][j] = sequence[i + j];
            // printf("%d,", block[sub_block][j]);
        }
    }
    // printf("\nafter blocked sequence\n");
    if (remain_samples > 0)
    {
        for (int k = 0; k < N; k++)
        {
            block[sub_block][k] = '\0';
        }
        int rem = 0;
        for (; rem < remain_samples; rem++)
        {
            block[sub_block][rem] = sequence[i + rem];
        }
    }
}

void executor(int *seq_og, int seq_og_size, int blocks, int blocks_sequence[blocks][BLOCK_SIZE], int *pre_processed_block, int *e)
{
    int zero_block_seq[seq_og_size];
    char encoded_seq[10000];
    int zero_block_seq_len = 0;
    int N = BLOCK_SIZE;
    breaker(seq_og, seq_og_size / sizeof(seq_og[0]), blocks, blocks_sequence);
    int seq_delayed[BLOCK_SIZE];
    int sub_block = 0;
    int remain_samples = (seq_og_size / sizeof(seq_og[0])) % N;
    int zero_block_seq_index = 0, encoded_seq_index = 0;
    int zero_block_count = 0, nonzero_block_count = 0;
    int zero_flag = 0, non_zero_flag = 0;
    char encoded[4096] = {'\0'};
    char result[4096] = {'\0'};
    for (; sub_block < blocks; sub_block++)
    {
        zero_flag = 0, non_zero_flag = 0;
        memset(pre_processed_block, '\0', sizeof(pre_processed_block));
        pre_processor(blocks_sequence[sub_block], BLOCK_SIZE, seq_delayed, pre_processed_block);
        for (int i = 0; i < BLOCK_SIZE; i++)
        {
            if (pre_processed_block[i] != 0)
            {
                non_zero_flag += 1;
                break;
            }
        }
        if (non_zero_flag)
        {
            if (zero_block_count != 0)
            {
                memset(encoded, '\0', sizeof(encoded));
                memset(result, '\0', sizeof(result));
                printf("\nzero block count !=0\n");
                zero_block(zero_block_seq, zero_block_seq_index, &result[0], &encoded[0]);
                strcat(encoded_seq, result);
                zero_block_count = 0;
                zero_block_seq_index = 0;
            }
            int j = 0;
            memset(encoded, '\0', sizeof(encoded));
            // printf("in non zero flag before code_select fun\n");
            // code_select(pre_processed_block, encoded, encoded_seq_index, BLOCK_SIZE);
            // printf("\nafter code select\n");
            strcat(encoded_seq, encoded);

            nonzero_block_count++;
        }
        else
        {
            int j = 0;
            for (; j < BLOCK_SIZE; j++)
            {
                zero_block_seq[zero_block_seq_index] = pre_processed_block[j];
                zero_block_seq_index += 1;
            }
            zero_block_count++;
            // printf("\nzero block count=%d,index=%d\n", zero_block_count, zero_block_seq_index);
        }
    }
    if (zero_block_count != 0)
    {
        memset(encoded, '\0', sizeof(encoded));
        memset(result, '\0', sizeof(result));
        zero_block(zero_block_seq, zero_block_seq_index, &result[0], &encoded[0]);
        strcat(encoded_seq, result);
        zero_block_count = 0;
        zero_block_seq_index = 0;
    }
    // printf("\nencoded_seq before dec call=%s\n", encoded_seq);
    dec(encoded_seq, e);
}
int main()
{
    int seq_og[SEQ_OG_SIZE] = {0};
    // int delta[BLOCK_SIZE] = {0};
    int *delta = (int *)malloc(BLOCK_SIZE * sizeof(int));
    int e[4096];
    // int *e = (int *)malloc(BLOCK_SIZE * sizeof(int));
    int *e_ptr = &e[0];
    int blocks = SEQ_OG_SIZE / BLOCK_SIZE;
    int remain_samples = SEQ_OG_SIZE % BLOCK_SIZE;
    if (remain_samples > 0)
    {
        blocks += 1;
    }
    int blocks_sequence[blocks][BLOCK_SIZE];
    int sizeof_seq_og = sizeof(seq_og);
    // printf("before for loop\n");
    for (int i = 0; i < SEQ_OG_SIZE; i++)
    {
        seq_og[i] = 1900 + (i * 10);
        // printf("%d,", seq_og[i]);
    }
    // printf("after for loop\n");
    for (int i = 0; i < blocks; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            blocks_sequence[i][j] = 0;
        }
    }
    struct timespec tnsec1, tnsec2;
    long int diff;
    clock_gettime(CLOCK_REALTIME, &tnsec1);
    executor(seq_og, sizeof_seq_og, blocks, blocks_sequence, delta, e_ptr);
    clock_gettime(CLOCK_REALTIME, &tnsec2);
    diff = tnsec2.tv_nsec - tnsec1.tv_nsec;
    printf("\ndiff time in nano sec=%ld\n", diff);
    printf("\n");
    free(delta);
    int e_size = 0;
    printf("\n e array:");
    for (int m = 0; e[m] != NULL; m++)
    {
        printf("%d,", e[m]);
        e_size += 1;
    }
    printf("\n");
    int len_seq = sizeof(seq_og) / sizeof(seq_og[0]);
    int len_e = e_size;
    float c_ratio = (float)len_seq / (float)len_e;
    printf("\nlen_seq=%d,len_e=%d,c_ratio=%f\n", len_seq, len_e, c_ratio);
    char s[4096] = {'\0'};
    joiner(e, SEQ_OG_SIZE, s);
    int s_len = strlen(s);
    // printf("\ns_len=%d\n", s_len);
    // printf("\ndecoder bin=%s\n", s);
    entropy_decoder(s, SEQ_OG_SIZE);
    // printf("\ndecoded_seq index=%d\n", decoded_index);
    memset(s, '\0', sizeof(s));
    for (int i = 0; i < decoded_index; i++)
    {
        // printf("%d,", decoded_seq[i]);
    }

    printf("\npost processor\n");
    // int final_decoded_seq[SEQ_OG_SIZE];
    for (int i = 0; i < decoded_index; i += BLOCK_SIZE)
    {

        printf("\ni=%d\n", i);
        post_processor(&decoded_seq[i]);
        // post_processor(&decoded_seq[i], &final_decoded_seq);
    }
    printf("\n=========== after post processor=========\n");
    for (int i = 0; i < SEQ_OG_SIZE; i++)
    {
        printf("%d,", final_decoded_seq[i]);
    }
    printf("\nfinal decoded seq done\n");
    return 0;
}