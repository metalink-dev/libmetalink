#include <stdio.h>
#include <string.h>
#include <CUnit/Basic.h>
/* include test cases' include files here */
#include "metalink_list_test.h"
#include "metalink_pctrl_test.h"
#include "metalink_parser_test.h"
#include "metalink_parser_test_v4.h"
#include "metalink_helper_test.h"

static int init_suite1(void)
{
  return 0;
}

static int clean_suite1(void)
{
  return 0;
}


int main(void)
{
   CU_pSuite pSuite = NULL;
   unsigned int num_tests_failed;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("libmetalink_TestSuite", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if((!CU_add_test(pSuite, "test of metalink_list", test_metalink_list))
      ||
      (!CU_add_test(pSuite, "test of metalink_pctrl_file_transaction",
		    test_metalink_pctrl_file_transaction))
      ||
      (!CU_add_test(pSuite, "test of metalink_pctrl_resource_transaction",
		    test_metalink_pctrl_resource_transaction))
      ||
      (!CU_add_test(pSuite, "test of metalink_pctrl_checksum_transaction",
		    test_metalink_pctrl_checksum_transaction))
      ||
      (!CU_add_test(pSuite, "test of metalink_pctrl_piece_hash_transaction",
		    test_metalink_pctrl_piece_hash_transaction))
      ||
      (!CU_add_test(pSuite, "test of metalink_pctrl_chunk_checksum_transaction",
		    test_metalink_pctrl_chunk_checksum_transaction))
      ||
      (!CU_add_test(pSuite, "test of metalink_parse_file",
		    test_metalink_parse_file))
      ||
      (!CU_add_test(pSuite, "test of metalink_parse_fp",
		    test_metalink_parse_fp))
      ||
      (!CU_add_test(pSuite, "test of metalink_parse_fd",
		    test_metalink_parse_fd))
      ||
      (!CU_add_test(pSuite, "test of metalink_parse_memory",
		    test_metalink_parse_memory))
      ||
      (!CU_add_test(pSuite, "test of metalink_parse_update",
		    test_metalink_parse_update))
      ||
      (!CU_add_test(pSuite, "test of metalink_parser_update_fail",
		    test_metalink_parse_update_fail))
      ||
      (!CU_add_test(pSuite, "test of metalink_check_safe_path",
                    test_metalink_check_safe_path))
      ||
      (!CU_add_test(pSuite, "test of metalink_get_version",
                    test_metalink_get_version))
      ||
      (!CU_add_test(pSuite, "test of metalink_parse_file_v4",
		    test_metalink_parse_file_v4))
      ) {
     CU_cleanup_registry();
     return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   num_tests_failed = CU_get_number_of_tests_failed();
   CU_cleanup_registry();
   if(CU_get_error() == CUE_SUCCESS) {
     return num_tests_failed;
   } else {
     printf("CUnit Error: %s\n", CU_get_error_msg());
     return CU_get_error();
   }
}
