
#ifndef _H_PROMPT
#define _H_PROMPT
#include <Arduino.h>

#ifndef UINT16_MAX
#define UINT16_MAX 65535
#endif

struct PromptResult {
  uint16_t len;
  uint8_t *data;
};

enum PromptReadStage {
  ZERO = 0,
  WAIT_LEN = 1,
  WAIT_STR = 2,
  FINISH = 3,
};

/**
 * PromptReader reads a new password string from serial input.
 *
 * Expected string should look like:
 *  "<len>\n<value>"
 * For example:
 *  "5\nhello"
 */
class PromptReader {
private:
  PromptReadStage m_stage;
  uint16_t m_cap;
  uint16_t m_len;
  uint16_t m_maxLen;
  uint8_t *m_data;

  bool readStrChar(uint8_t ch);
  bool readLenChar(uint8_t ch);

  bool onInit();
  bool tryReadLen();
  bool tryReadChar();
public:
  PromptReader(uint16_t maxLen);
  ~PromptReader();

  /**
   * Resets a reader.
   */
  void reset();

  /**
   * Try to read a an input from Serial.
   *
   * Returns false on finish.
   */
  bool read();

  /**
   * Returns collected result.
   *
   * Returns a zero-length result on error.
   */
  PromptResult getResult();
};

#endif
