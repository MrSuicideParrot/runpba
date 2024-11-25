#ifndef __TFM_TEST_APP_H__
#define __TFM_TEST_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Simple macro to mark UNUSED variables
 *
 */
#define UNUSED_VARIABLE(X) ((void)(X))

/**
 * \brief Main test application for the RTX-TFM core
 *        integration tests
 *
 */
void test_app(void *argument);


void recover_from_task_fault(void);

#ifdef __cplusplus
}
#endif

#endif /* __TFM_TEST_APP_H__ */
