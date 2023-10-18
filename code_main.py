''' generate a random sequrnce of integers '''
import random
min_limit = 0
max_limit = 65000
# seq_og = [int(random.randint(9820,9850)) for i in range(1280)]      #   generating 16 sameple sequence
# seq1 = [int(random.randint(min_limit,10)) for i in range(64*5)]
# seq2 = [int(random.randint(min_limit,1000)) for i in range(64*10)]
seq3 = [int(random.randint(min_limit,0)) for i in range(64*8)]
seq_og = seq3

print(f'original sequence: {seq_og}')
resolution = 16     #   resolution: number of bits to repreent each sample   
block_size = 64     #   block_size: number of samples in a block   
'''breaks the sequence into groups'''
def breaker(sequence):
    N= block_size
    block = [sequence[i:i+N] for i in range(0, len(sequence),N)]
    # print(block)
    return block
'''Binary of a decimal sequence'''
def binary(seq):
    bin_seq = []
    for e in seq:
        binary = bin(e)[2:].zfill(resolution)
        bin_seq.append(binary)
    print(bin_seq)
    print(len(seq)*resolution)
def dec(seq):
    blocked_seq = []
    dec_seq = []
    N= resolution
    blocked_seq = [seq[i:i+N] for i in range(0, len(seq),N)]
    for block in blocked_seq:
        block = int(block,2)
        dec_seq.append(block)
    # print(blocked_seq)
    dec_seq.insert(0,len(seq))
    # print(dec_seq)
    return dec_seq
'''pre-processor 
   argument:   block of J samples
   return:     delta pre-processed block''' 
def pre_processor(block):
    seq = block
    seq_delayed = [seq[i-1] if i>0 else 0 for i in range(len(seq))]
    d = [seq[i] - seq_delayed[i] for i in range(len(seq))]
    ymax = max_limit
    ymin = min_limit
    delta = [] 
    t = []
    for i in range(len(seq_delayed)):
        a = ymax - seq_delayed[i]
        b = seq_delayed[i] - ymin
        T = min(a,b)
        t.append(T)
        # print(T)
        if(0<=d[i]<=T):
            delta.append(2*d[i])
        elif(-T<=d[i]<0):
            delta.append(2*(abs(d[i]))-1)
        else:
            delta.append(T + abs(d[i]))
    # print(f'diff{d}')
    # print(delta)
    return delta
'''Finite Sequence: 
   argument:   block
   return:     encoded block'''
def FS_block(block):
    encoded = []
    for sample in block:
        encoded.append('1'.zfill(sample+1))
        # encoded_seq.append(x)
    if resolution==3 or resolution==4:
        option_id = bin(1)[2:].zfill(2)
    elif resolution>4 and resolution<=8:
        option_id = bin(1)[2:].zfill(3)
    elif resolution>8 and resolution<=16:
        option_id = bin(1)[2:].zfill(4)
    elif resolution>16 and resolution<=32:
        option_id = bin(1)[2:].zfill(5)
    encoded.insert(0,option_id)
    encoded = "".join(encoded)
    # print(f'option id {option_id}')
    return encoded
'''FS code element wise'''
def FS_element(x):
    x = '1'.zfill(x)
    return x
''' Split Sample'''
def split_sample(block,k):
    n = resolution   # n bit representation
    #k'th split
    option_id = 0
    msb_bunch = []
    lsb_bunch = []

    for sample in block:
        x = bin(sample)[2:].zfill(n)         
        # print(x)
        lsb = x[(n-k):n]    # k LSB
        msb = x[:(n-k)]     # (n-k) MSB
        msb_dec = int(msb,base=2)
        # print(msb_dec)
        msb_fs = FS_element(msb_dec+1)     
        # print(msb_fs + '|' + lsb)
        msb_bunch.append(msb_fs)
        lsb_bunch.append(lsb)
    msb_bunch = "".join(msb_bunch)
    lsb_bunch = "".join(lsb_bunch)
    encoded = []
    encoded.append(msb_bunch)
    encoded.append(lsb_bunch)
    # print(encoded)
    if resolution==3 or resolution==4:
        option_id = bin(k+1)[2:].zfill(2)
    elif resolution>4 and resolution<=8:
        option_id = bin(k+1)[2:].zfill(3)
    elif resolution>8 and resolution<=16:
        option_id = bin(k+1)[2:].zfill(4)
    elif resolution>16 and resolution<=32:
        option_id = bin(k+1)[2:].zfill(5)
    encoded.insert(0,option_id)
    # print(f'option id {option_id}')
    encoded = "".join(encoded)
    # print(f'split: {encoded}')
    return encoded

'''No Compression'''
def no_copression(block):
    option_id = 0 
    n_block = [bin(e)[2:].zfill(resolution) for e in block]

    if resolution==1 or resolution==2:
        option_id = '1'
    elif resolution==3 or resolution==4:
        option_id = '11'
    elif resolution>4 and resolution<=8:
        option_id = '111'
    elif resolution>8 and resolution<=16:
        option_id = '1111'
    elif resolution>16 and resolution<=32:
        option_id = '11111'
    n_block.insert(0,option_id)

    n_block = "".join(n_block)

    return n_block
'''Second Extension Compression'''
def second_extension(sequence):
    encoded = []
    option_id = 0
    paired_list = [[sequence[i],sequence[i+1]] for i in range(0, len(sequence)-1,2)]
    # print(paired_list) 
    gamma = 0

    for pair in paired_list:
        gamma = 0.5*(pair[0] + pair[1])*(pair[0] + pair[1] + 1) + pair[1]
        if int(gamma+1)<=5:
            encoded.append(FS_element(int(gamma+1)))
        else:
            print("not executing second extension")
            return 0
        # print(gamma)
    if resolution==1 or resolution==2:
        option_id = bin(1)[2:].zfill(2) 
    elif resolution==3 or resolution==4:
        option_id = bin(1)[2:].zfill(3)
    elif resolution>4 and resolution<=8:
        option_id = bin(1)[2:].zfill(4)
    elif resolution>8 and resolution<=16:
        option_id = bin(1)[2:].zfill(5)
    elif resolution>16 and resolution<=32:
        option_id = bin(1)[2:].zfill(6)
    encoded.insert(0,option_id)
    encoded = "".join(encoded)
    return encoded

def minimum(a,b):
    smallest = 0
    if len(a)<=len(b):
        smallest = a
    else:
        smallest = b
    return smallest

'''Code Selection
    checks whick compression technique minimizes the length 
    and return the encoded block'''
def code_select(block):
    fs_encoded = FS_block(block)
    '''modify k split selection method to optimise the memory usage'''
    smallest = split_sample(block,1)
    for i in range(1,int(resolution/2)):
        temp = split_sample(block,i)
        if len(temp)<=len(smallest):
            smallest = temp
    split_sample_encoded = smallest    
    
    second_extension_encoded = second_extension(block)
    no_compression_encoded = no_copression(block)
    if second_extension_encoded == 0:
        encoded_block = minimum(minimum(fs_encoded,split_sample_encoded), no_compression_encoded)
    else:
        encoded_block = minimum(minimum(fs_encoded,split_sample_encoded),minimum(second_extension_encoded , no_compression_encoded))
    return encoded_block

'''zero block'''
def zero_block(sequence):
    zeros = 0
    ROS = 4
    option_id = 0
    block_list = breaker(sequence)
    # print(block_list)
    encoded = []
    for block in block_list:
        if not any(block):
            zeros +=1
        else:
            if zeros!=0:
                # print(f'zeros = {zeros}')   # process zeros here
                if zeros<=ROS:
                    encoded.append(FS_element(zeros))
                else:
                    encoded.append(FS_element(zeros+1))
            zeros = 0
            # print(f'block: {block}')
            encoded.append(code_select(block))
    if zeros!=0:
            # print(f'zeros = {zeros}')   # process zeros here
            if zeros<=ROS:
                encoded.append(FS_element(zeros))
            else:
                encoded.append(FS_element(zeros+1))
    if resolution==1 or resolution==2:
        option_id = bin(0)[2:].zfill(3) 
    elif resolution==3 or resolution==4:
        option_id = bin(0)[2:].zfill(4)
    elif resolution>4 and resolution<=8:
        option_id = bin(0)[2:].zfill(5)
    elif resolution>8 and resolution<=16:
        option_id = bin(0)[2:].zfill(6)
    elif resolution>16 and resolution<=32:
        option_id = bin(0)[2:].zfill(7)         
    encoded.insert(0,option_id)
    encoded = "".join(encoded)
    print(f'encoded {encoded}')
    return encoded
# # print(zero_block())
# m = [[0,0,0,0,0,0,0,0],[0,0,0,0,0,0,0,0]]
# m = [e for block in m for e in block]
# print(m)
'''Executes the chain'''
def executor(sequence):
    encoded_seq = []
    blocked_sequence = breaker(sequence)
    zero_block_seq = []
    for block in blocked_sequence:
        #   now send each block to the preprocessor
        pre_processed_block = pre_processor(block)
        # print(f'pre processed block {pre_processed_block}')
        if not any(pre_processed_block):
            # print("all zero block")
            #   if the block is all zero wait for next block and make a 
            #   list of all zero blocks  
            zero_block_seq.append(pre_processed_block)
        else:
            #   if the all zero sequence is present send it to zero block compression
            # print("non zero block")
            if(len(zero_block_seq)!=0):
                zero_block_seq = [e for block in zero_block_seq for e in block]
                encoded_seq.append(zero_block(zero_block_seq))
                zero_block_seq.clear()
            #   if the pre_processed block is non-zero send it to code selection    
            encoded_seq.append(code_select(pre_processed_block))
    #   if the block is all zero wait for next block and make a 
    #   list of all zero blocks
    if(len(zero_block_seq)!=0):
        zero_block_seq = [e for block in zero_block_seq for e in block]
        encoded_seq.append(zero_block(zero_block_seq))
    #   return encoded sequence     
    # print(f'ecoded blocked sequence: {encoded_seq}')
    encoded_seq = "".join(encoded_seq)
    # print(encoded_seq)
    # print(len(encoded_seq))
    e = dec(encoded_seq)
    c_ratio = len(seq_og)/len(e)
    print(f'enoded seq: {e}')
    print(f'compression ratio: {c_ratio}')
    return e



''' dummy decoder'''
# Decoder Part
'''list to store the decoded sequnce'''
decoded_seq = []
'''the received sequence is convereted to binary 
    and length matched to original transmitted sequence length'''        
def joiner(seq):
    seq = list(seq)
    l = seq[0]
    # print(l)
    target_seq = seq[1:]
    received_seq = []
    for block in target_seq:
        rem_l = l-len(received_seq)
        if (rem_l>=resolution):
            received_seq.append(bin(block)[2:].zfill(resolution))
        else:
            received_seq.append(bin(block)[2:].zfill(rem_l))
        received_seq = list("".join(received_seq))
    received_seq = "".join(received_seq)
    return received_seq
zero_block_id = '00000'
second_extension_id = '00001'
FS_block_id = '0001'
k1_id = '0010'
k2_id = '0011'
k3_id = '0100'
k4_id = '0101'
k5_id = '0110'
k6_id = '0111'
k7_id = '1000'
k8_id = '1001'
k9_id = '1010'
k10_id = '1011'
k11_id = '1100'
k12_id = '1101'
k13_id = '1110'
no_copression_id = '1111'

id_lut = [zero_block_id, second_extension_id, FS_block_id,
          k1_id,k2_id,k3_id,k4_id,k5_id,k6_id,k7_id,k8_id,
          k9_id,k10_id,k11_id,k12_id,k13_id,no_copression_id]

k_id = [k1_id,k2_id,k3_id,k4_id,k5_id,k6_id,k7_id,k8_id,
          k9_id,k10_id,k11_id,k12_id,k13_id]
def FS_decoder(seq,block_size):
    zeros = 0
    ones = 0
    dec_seq = []
    for i in range(len(seq)):
        if (seq[i] == 0) or (seq[i] == '0'):
            zeros+=1
        else:
            ones+=1            
            dec_seq.append(zeros)    # one FS sample
            zeros = 0
            if ones == block_size:  # finished FS code for J(block size) samples
                return dec_seq
# print(FS_decoder('0100010001',3))
def FS_block_decoder(seq):
    msb_dec = FS_decoder(seq,block_size)
    msb_bin = [bin(i)[2:].zfill(resolution) for i in msb_dec]
    current_index = len(msb_dec)+sum(msb_dec)
    # print(f'msb_dec in FS block {msb_dec}')
    decoded_seq.append(msb_dec)
    return (current_index+4)
def split_sample_decoder(seq,k):
    msb_dec = FS_decoder(seq,block_size)
    msb_bin = [bin(i)[2:].zfill(resolution-k) for i in msb_dec]
    current_index = len(msb_dec)+sum(msb_dec)                               
    block_reamining = seq[current_index: (current_index)+(k*block_size)]
    lsb_bin = [block_reamining[i:i+k] for i in range(0,len(block_reamining),k)]          
    # print(f'msb {msb_bin} & lsb {lsb_bin}')    
    decoded_bin = []
    for i in range(len(msb_bin)):
        decoded_bin.append(msb_bin[i]+lsb_bin[i])
    # print(decoded_bin)
    decoded_dec = [int(i,2) for i in decoded_bin]
    # print(decoded_dec)
    decoded_seq.append(decoded_dec)
    current_index = (current_index)+(k*block_size)
    # print(f'current block: {seq[:current_index]} |  current index: {current_index+4}')
    return (current_index+4)
def no_compression_decoder(seq):
    sampled_block = [seq[i:i+resolution] for i in range(0,resolution*block_size,resolution)]
    sampled_dec = [int(sample,2) for sample in sampled_block]
    # print(sampled_dec)
    decoded_seq.append(sampled_dec)
    return (resolution*block_size + 4)
'''Secon Extension Decoder'''
'''extract the 5 bit id first and then run'''
'''gets a binary sequence
    '''
def second_extension_decoder(seq):
    gamma_list = FS_decoder(seq,block_size/2)
    # gamma_list = [1,0,4,1]
    delta = []
    for i in range(len(gamma_list)):
        if gamma_list[i]==0:
            beta = 0
            ms = 0
        elif gamma_list[i]==1 or gamma_list[i]==2:
            beta = 1
            ms = 1
        elif gamma_list[i]>=3 and gamma_list[i]<=5:
            beta = 2
            ms = 3
        elif gamma_list[i]>=6 and gamma_list[i]<=9:
            beta = 3
            ms = 6
        elif gamma_list[i]>= 10 and gamma_list[i]<=14:
            beta = 4
            ms = 10
        elif gamma_list[i]>=15 and gamma_list[i]<=20:
            beta = 5
            mas = 15
        elif gamma_list[i]>=21 and gamma_list[i]<=27:
            beta = 6
            ms = 21
        elif gamma_list[i]>=28 and gamma_list[i]<=35:
            beta = 7
            ms  = 28
        elif gamma_list[i]>=36 and gamma_list[i]<=44:
            beta = 8
            ms = 36
        
        del_even = gamma_list[i] - ms
        del_odd = beta - del_even
        delta.append(del_odd) 
        delta.append(del_even)   
    print(f'gamma list {gamma_list}')
    # print(delta)
    decoded_seq.append(delta)
    current_index = len(gamma_list)+sum(gamma_list)
    return (current_index+5)

'''zero block decoder'''
def zero_block_decoder(seq):
    zeros = 0
    all_zero_blocks = 0
    ROS = 4
    for sample in seq:
        if sample == 0 or sample == '0':
            zeros+=1
        else:
            if zeros<=ROS:
                all_zero_blocks = zeros
            else:
                all_zero_blocks = zeros-1
    decoded_all_zero_block = [0 for i in range(all_zero_blocks*block_size)]
    decoded_seq.append(decoded_all_zero_block)
    current_index = zeros+1
    return current_index+5
'''ID extraction & entropy decoding'''
def entropy_decoder(seq):
    # print(seq)
    current_index = 0
    block_id = ''
    while(current_index<=len(seq)):
        # print(f'current index {current_index}')
        if str(seq[current_index:current_index+4]) in id_lut:
            # print('4 bit')
            ''' so its FS/ split_sample or no compression'''
            block_id = str(seq[current_index:current_index+4])
            if block_id==FS_block_id:
                # print('FS block')
                current_index = current_index + FS_block_decoder(seq[current_index+4:])
                # current_index = current_index + FS_block_decodeer(seq[current_index+4:])
            elif block_id==no_copression_id:
                # print('no compression')
                current_index = current_index + no_compression_decoder(seq[current_index+4:])
            elif block_id in k_id:
                # print('split sample')
                i = k_id.index(block_id)
                print(block_id, i+1)
                current_index = current_index + split_sample_decoder(seq[current_index+4:],i+1)
        elif str(seq[current_index:current_index+5]) in id_lut:
            # print('5 bit')
            ''' so its zero_block or second extension'''
            block_id = str(seq[current_index:current_index+5])
            if block_id == second_extension_id:
                # print('secondf extension')
                current_index = current_index + second_extension_decoder(seq[current_index+5:])
            elif block_id == zero_block_id:
                # print("zero block decoder")
                current_index = current_index + zero_block_decoder(seq[current_index+5:])
        else:
            # print('nope')
            break
'''Post Processing'''
def prop(seq):
    x = [0]
    for i in range(len(seq)):
    #     if i<2:
    #         delta = seq[i]/2 + x[i]
    #     else:
    #         delta = seq[i] + x[i]
        delta = seq[i] + x[i]
        x.append(delta)
    # print(f'prop o/p {x[1:]}')
    return x[2:]
def demapper(seq):
    x=[0]
    xmax = max_limit
    xmin = min_limit
    delta= [0]
    for i in range(len(seq)):
        a = x[i-1] - xmin
        b = xmax - x[i-1]
        theta = min(a,b)
        if seq[i]<= 2*theta:
            if seq[i]%2==0:
                di = seq[i]/2
            else:
                di = -(seq[i]+1)/2
            delta.append(di)
        else:
            if theta==a:
                di = seq[i] - theta
            elif theta==b:
                di = theta - seq[i]
            delta.append(di)
        x = prop(delta)
    # print(x)
    return x

def post_processor(seq):
    return demapper(seq)


# Execution & Test

def post_processor(seq):
    return demapper(seq)

s = executor(seq_og)
# print(f'encoded {s}')
s = joiner(s)
entropy_decoder(s)
# for block in decoded_seq:
#     print(post_processor(block))
seq_reconstruct = []
for block in decoded_seq:
    seq_reconstruct.extend(post_processor(block))
# print(seq_reconstruct)
# print(len(seq_reconstruct))

seq_reconstruct = [int(i) for i in seq_reconstruct]
if seq_reconstruct==seq_og:
    print("true")

print(f'decoded seq: {seq_reconstruct}')
print(f'decoded length {len(seq_reconstruct)}')