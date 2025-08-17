#include "config.h"
#include "prompt.h"

bool isBreakChar(uint8_t b) {
  switch (b) {
    case '\r':
    case '\n':
      return true;
    default:
      break;
  }
  return false;
}

PromptReader::PromptReader(uint16_t maxLen) {
  this->m_cap = 0;
  this->m_len = 0;
  this->m_maxLen = maxLen;
  this->m_stage = PromptReadStage::ZERO;
  this->m_data = nullptr;
}

PromptReader::~PromptReader() {
  if (m_data != nullptr)
    delete[] m_data;
}

void PromptReader::reset() {
  m_cap = 0;
  m_len = 0;
  m_stage = PromptReadStage::ZERO;
  if (m_data != nullptr)
    delete[] m_data;
}

bool PromptReader::onInit() {
  if (Serial) {
    Serial.print("NEW PASSWD? (maxlen: ");
    Serial.print(m_maxLen);
    Serial.println(")");
    m_stage = PromptReadStage::WAIT_LEN;
  }

  return true;
}

bool PromptReader::readLenChar(uint8_t ch) {
  if (ch < '0' || ch > '9') {
    return false;
  }

  uint8_t digit = ch - '0';
  // Check if multiplying by 10 would overflow
  if (m_cap > UINT16_MAX / 10) {
    return false;  // Overflow on multiplication
  }

  // Check if adding digit would overflow after multiplication
  if (m_cap == UINT16_MAX / 10 && digit > UINT16_MAX % 10) {
    return false;  // Overflow on addition
  }

  uint16_t newNum = digit + m_cap * 10;
  m_cap = newNum;
  return true;
}

bool PromptReader::tryReadLen() {
  int avail = Serial.available();
  if (avail == 0) {
    return true;
  }

  for (int i = 0; i < avail; i++) {
    if (m_cap > m_maxLen) {
      Serial.println("Err: cap");
      reset();
      return false;
    }

    uint8_t b = Serial.read();
    if (isBreakChar(b)) {
      if (m_cap == 0) {
        Serial.println("Err: Empty cap");
        reset();
        return false;
      }

      // Stop read size
#ifdef DEBUG
      Serial.print("OK: len=");
      Serial.println(m_cap);
#endif
      m_data = new uint8_t[m_cap];
      m_stage = PromptReadStage::WAIT_STR;
      return true;
    }

    if (!readLenChar(b)) {
      // Reset buffer on bad char
      m_cap = 0;
      Serial.println("Err: char");
    }
  }

  return true;
}

bool PromptReader::readStrChar(uint8_t ch) {
  uint16_t newLen = m_len + 1;
  if (newLen > m_cap) {
    reset();
    return false;
  }

  m_data[m_len] = ch;
  m_len = newLen;
  return true;
}

bool PromptReader::tryReadChar() {
  int avail = Serial.available();
  if (avail == 0) {
    return true;
  }

  for (int i = 0; i < avail; i++) {
    uint8_t b = Serial.read();
    if (isBreakChar(b)) {
      if (m_len == 0) {
        // HACK: handle double escape sequence if running from a terminal.
        continue;
      }

      Serial.print("OK (len=");
      Serial.print(m_len);
      Serial.println(")");
#ifdef DEBUG
      Serial.write(m_data, m_len);
      Serial.println();
#endif

      m_stage = PromptReadStage::FINISH;
      return false;
    }

    if (!readStrChar(b)) {
      reset();
      return false;
    }
  }

  return true;
}

bool PromptReader::read() {
  switch (m_stage) {
    case PromptReadStage::ZERO:
      return onInit();
    case PromptReadStage::WAIT_LEN:
      return tryReadLen();
    case PromptReadStage::WAIT_STR:
      return tryReadChar();
    case PromptReadStage::FINISH:
      return false;
  }

  return false;
}

PromptResult PromptReader::getResult() {
  if (m_stage == PromptReadStage::FINISH) {
    return PromptResult{
      m_len,
      m_data,
    };
  }

  return PromptResult{
    0,
    nullptr
  };
}
