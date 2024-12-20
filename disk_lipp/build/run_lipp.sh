#!/bin/bash/
##run lipp,data:uniform_dense_1K_uint64
datasets_prefix="/home/ywm/DiskZM/datasets/"

# echo "test lipp bulk,dataset:uniform_dense_1K_uint64"
# ./lipp --keys_file=${datasets_prefix}/synthetic/uniform_dense_1K_uint64 --op_type=bulk --index_file=index_file --total_count=1000 --has_size=1 >test_lipp_bulk.log

# echo "test lipp lookup,dataset:uniform_dense_1K_uint64"
# ./lipp --keys_file=${datasets_prefix}/synthetic/uniform_dense_1K_uint64\
#  --op_type=insert --index_file=index_file --total_count=1000 --has_size=1\
#   --search_count=100 --step=10 >>test_lipp_lookup.log

echo "test lipp insert,dataset:mapped_keys"
./lipp --keys_file=${datasets_prefix}/synthetic/mapped_keys\
 --op_type=insert --index_file=index_file --total_count=1000 --has_size=0 >test_lipp_insert.log

