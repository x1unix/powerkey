'use strict';

const PROMPT_REQ_REGEX = /^NEW PASSWD\? \(maxlen: ([\d]+)\)/;
const CONFIRM_RSP_REGEX = /^OK \(len=([\d]+)\)$/;

const sceneNames = ['start', 'progress', 'prompt', 'error', 'finish'];

const isLinux = !!navigator.userAgent.match(/Linux/);
const scenes = {
  start: 0,
  progress: 1,
  prompt: 2,
  error: 3,
  finish: 4,
};

// Arduino-specific USB filters
const arduinoFilters = [
  // Arduino Pro Micro (SparkFun)
  { usbVendorId: 0x1B4F, usbProductId: 0x9206 },
  { usbVendorId: 0x1B4F, usbProductId: 0x9205 },

  // Arduino Leonardo/Micro (original)
  { usbVendorId: 0x2341, usbProductId: 0x8036 },
  { usbVendorId: 0x2341, usbProductId: 0x0036 },
  { usbVendorId: 0x2341, usbProductId: 0x8037 },
  { usbVendorId: 0x2341, usbProductId: 0x0037 },

  // Arduino Uno R3
  { usbVendorId: 0x2341, usbProductId: 0x0043 },
  { usbVendorId: 0x2341, usbProductId: 0x0001 },

  // Arduino Nano (CH340 chip)
  { usbVendorId: 0x1A86, usbProductId: 0x7523 },

  // Arduino Nano (FTDI chip)
  { usbVendorId: 0x0403, usbProductId: 0x6001 },

  // Arduino Mega
  { usbVendorId: 0x2341, usbProductId: 0x0042 },
  { usbVendorId: 0x2341, usbProductId: 0x0010 },

  // Generic Arduino-compatible boards
  { usbVendorId: 0x16C0, usbProductId: 0x0483 }, // Teensyduino
  { usbVendorId: 0x1b4f, usbProductId: 0x9206 }, // SparkFun Pro Micro
];

const $ = (s) => document.querySelector(s);

const progressMsg = $("#progressTitle");
const errMsg = $("#errMessage");
const inputForm = $("#pwdForm");
const inputText = $("#pwd");
const btnStart = $("#btnStart");
const cancelBtn = $("#btnCancel");

const state = {
  scene: 0,
  maxLen: 0,
  port: null,
  isWriting: false
};

function setErrorMsg(msg) {
  errMsg.innerText = msg;
  setScene(scenes.error);
}

function setProgress(msg, cancellable = false) {
  progressMsg.innerText = msg;
  if (cancellable) {
    cancelBtn.classList.remove("hidden");
  } else {
    cancelBtn.classList.add("hidden");
  }

  setScene(scenes.progress);
}

function init() {
  if (!("serial" in navigator)) {
    setErrorMsg("This browser is not supported. Try using Google Chrome or other Chrome-based browser.");
    return;
  }

  btnStart.addEventListener("click", () => {
    start().catch((err) => {
      if (err.name === "NotFoundError") {
        setScene(scenes.start);
        return;
      }

      setErrorMsg(err.toString())
    });
  });

  btnCancel.addEventListener("click", () => {
    cancel();
  });

  inputForm.addEventListener("submit", (e) => {
    e.preventDefault();
    writePassword(inputText.value).catch(err => setErrorMsg(err.toString()));
  })
}

function setScene(id) {
  $(`#stage-${sceneNames[state.scene]}`).classList.add("hidden");
  $(`#stage-${sceneNames[id]}`).classList.remove("hidden");
  state.scene = id;
}

function resetPort() {
  void state.port.close();
  state.port = null;
}

const parseNumRegex = (str, regex) => {
  const m = regex.exec(str)
  if (!m) {
    return { ok: false };
  }

  return { val: parseInt(m[1]), ok: true };
}

const validateResponse = (str) => str.match(CONFIRM_RSP_REGEX);
const parseResponse = (str) => {
  if (str.startsWith("Err:")) {
    throw new Error("Device returned an error");
  }

  return parseNumRegex(str, CONFIRM_RSP_REGEX);
}

const validatePrompt = (str) => str.match(PROMPT_REQ_REGEX);
const parsePrompt = (str) => parseNumRegex(str, PROMPT_REQ_REGEX);

async function readPortUntil(port, cb) {
  const reader = port.readable.getReader();
  let buff = '';
  while (port && port.readable) {
    let { value, done } = await reader.read();

    const dec = new TextDecoder();
    buff += dec.decode(value).trim();

    if (cb(buff)) {
      break;
    }

    if (done) {
      break;
    }
  }

  void reader.cancel();
  reader.releaseLock();
  return buff;
}

const waitForPrompt = async (port) => await readPortUntil(port, validatePrompt);
const waitForResponse = async (port) => await readPortUntil(port, validateResponse);

async function writePassword(value) {
  if (!value.length || !state.port) {
    return
  }

  if (value.length > state.maxLen) {
    resetPort();
    throw new Error(`Password is too long (max: ${maxLen})`);
  }

  try {
    inputText.value = "";
    setProgress("Writing...");
    const enc = new TextEncoder();
    const data = enc.encode(`${value.length}\r${value}\r`);

    const writer = state.port.writable.getWriter();
    await writer.write(data);
    await writer.close();
    writer.releaseLock();

    setProgress("Waiting for device response...");
    const rsp = await waitForResponse(state.port);
    const { ok, val } = parseResponse(rsp);
    if (!ok) {
      throw new Error("Device didn't acknowledge. Please try again.");
    }

    if (val !== value.length) {
      throw new Error(`Device acknowledged password length doesn't match (want: ${value.length}, got: ${val})`);
    }

    setScene(scenes.finish);
    setTimeout(() => setScene(scenes.start), 3000);
  } finally {
    resetPort();
  }
}

async function start() {
  setProgress("Waiting for keyboard...");

  // On Linux, device descriptors aren't available for some reason.
  const opts = isLinux ? undefined : {
    filters: arduinoFilters,
  };
  let port = await navigator.serial.requestPort(opts);
  state.port = port;

  await state.port.open({
    baudRate: 9600,
    dataBits: 8,
    stopBits: 1,
    parity: "none"
  })

  console.clear();
  setProgress("Waiting for handshake...");
  const buff = await waitForPrompt(port);
  console.log('REMOTE: ', buff);
  const { ok, val } = parsePrompt(buff);
  if (!ok) {
    setErrorMsg("Bad handshake response from device. Please try again");
    resetPort();
    return;
  }

  console.log('maxLen: ', val);
  state.maxLen = val;
  state.isWriting = true;
  inputText.setAttribute("maxlength", state.maxLen);
  setScene(scenes.prompt);
}

function cancel() {
  if (state.isWriting) {
    resetPort();
  }

  setScene(scenes.start);
}

init();
