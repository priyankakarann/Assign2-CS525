.PHONY: all
all: test_assign1 test_assign2

test_assign1: test_assign2_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c
	gcc -g -o test_assign1 test_assign2_1.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c

test_assign2: test_assign2_2.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c
	gcc -g -o test_assign2 test_assign2_2.c storage_mgr.c dberror.c buffer_mgr.c buffer_mgr_stat.c replacement_mgr_strat.c

.PHONY: clean
clean:
	rm test_assign1 test_assign2
