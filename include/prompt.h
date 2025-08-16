
#ifndef _H_PROMPT
#define _H_PROMPT
#include <Arduino.h>

struct PromptResult {
  uint16_t len;
  char *data;
};

enum PromptReadStage {
  ZERO = 0,
  WAIT_LEN = 1,
  WAIT_STR = 2,
  FINISH = 3,
};

class PromptReader {
private:
  PromptReadStage m_stage;
  uint16_t m_cap;
  uint16_t m_len;
  uint16_t m_maxLen;
  char *m_data;

  bool readStrChar(uint8_t ch);
  bool readLenChar(uint8_t ch);

  bool onInit();
  bool tryReadLen();
  bool tryReadChar();
public:
  PromptReader(uint16_t maxLen);
  ~PromptReader();
  void reset();
  bool readPassword();
  PromptResult getResult();
};

#endif
