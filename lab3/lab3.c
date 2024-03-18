#include <lcom/lcf.h>

#include <lcom/lab3.h>

#include <stdbool.h>
#include <stdint.h>

int main(int argc, char *argv[]) {
  // sets the language of LCF messages (can be either EN-US or PT-PT)
  lcf_set_language("EN-US");

  // enables to log function invocations that are being "wrapped" by LCF
  // [comment this out if you don't want/need it]
  lcf_trace_calls("/home/lcom/labs/lab3/trace.txt");

  // enables to save the output of printf function calls on a file
  // [comment this out if you don't want/need it]
  lcf_log_output("/home/lcom/labs/lab3/output.txt");

  // handles control over to LCF
  // [LCF handles command line arguments and invokes the right function]
  if (lcf_start(argc, argv))
    return 1;

  // LCF clean up tasks
  // [must be the last statement before return]
  lcf_cleanup();

  return 0;
}

int(kbd_test_scan)() {
  // Initialize variables
    int ipc_status;
    message msg;
    uint8_t irq_set;
    uint8_t scancode;

    // Subscribe to keyboard interrupts
    if (keyboard_subscribe_int(&irq_set) != 0) {
        return 1;
    }

    // Main loop to handle keyboard interrupts
    while (scancode != ESC_BREAKCODE) { // Loop until ESC key is pressed
        // Get a request message
        int r = driver_receive(ANY, &msg, &ipc_status);
        if (r != 0) {
            printf("driver_receive failed with: %d", r);
            continue;
        }

        // Check if the received message is a notification
        if (is_ipc_notify(ipc_status)) {
            switch (_ENDPOINT_P(msg.m_source)) {
                case HARDWARE: // Hardware interrupt notification
                    if (msg.m_notify.interrupts & BIT(irq_set)) { // Check if it's from the keyboard
                        
                    }
                    break;
                default:
                    break; // No other notifications expected
            }
        }
    }

    // Unsubscribe from keyboard interrupts
    if (keyboard_unsubscribe_int() != 0) {
        printf("Failed to unsubscribe from keyboard interrupts\n");
        return 1;
    }

    return 0;
}


int(kbd_test_poll)() {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}

int(kbd_test_timed_scan)(uint8_t n) {
  /* To be completed by the students */
  printf("%s is not yet implemented!\n", __func__);

  return 1;
}