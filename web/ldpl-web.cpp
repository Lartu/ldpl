#ifndef __EMSCRIPTEN__
#error "ldpl-web can only be used when compiling for the web."
#else

#include <emscripten.h>
#include <cmath>
#include "ldpl_types.h"

ldpl_text WEB_CODE;
ldpl_text WEB_SYMBOL;
ldpl_text WEB_TEXT;
ldpl_number WEB_NUMBER;

void WEB_RUN_RETURNING_NUMBER() {
  std::string code = WEB_CODE.str_rep();
  const char* data = emscripten_run_script_string(code.c_str());
  ldpl_text number_string(data);

  WEB_NUMBER = number_string.isNumber() ? number_string.getNumber() : NAN;
}

void WEB_RUN_RETURNING_TEXT() {
  std::string code = WEB_CODE.str_rep();
  const char* data = emscripten_run_script_string(code.c_str());
  /* Return empty string on NULL result */
  WEB_TEXT = data ? data : "";
}

void WEB_RUN() {
  std::string code = WEB_CODE.str_rep();
  emscripten_run_script(code.c_str());
}

void WEB_LOAD_NUMBER() {
  std::string symbol = WEB_SYMBOL.str_rep();

  WEB_NUMBER = EM_ASM_DOUBLE({
    const symbol = UTF8ToString($0).split('.');
    const obj = symbol.slice(0, -1).reduce((obj, key) => obj[key], window);

    return obj[symbol.slice(-1)[0]];
  }, symbol.c_str());
}

void WEB_LOAD_TEXT() {
  std::string symbol = WEB_SYMBOL.str_rep();

  char* text_value = (char*)EM_ASM_INT({
    const symbol = UTF8ToString($0).split('.');
    const obj = symbol.slice(0, -1).reduce((obj, key) => obj[key], window);
    const string = obj[symbol.slice(-1)[0].toString()];

    const string_length = lengthBytesUTF8(string) + 1;
    const string_ptr = _malloc(string_length);

    stringToUTF8(string, string_ptr, string_length); 

    return string_ptr;
  }, symbol.c_str());

  WEB_TEXT = text_value;

  free(text_value);
}

void WEB_STORE_NUMBER() {
  std::string symbol = WEB_SYMBOL.str_rep();
  double number = WEB_NUMBER;

  EM_ASM({
    const symbol = UTF8ToString($0).split('.');
    const obj = symbol.slice(0, -1).reduce((obj, key) => obj[key], window);
    obj[symbol.slice(-1)[0]] = $1;
  }, symbol.c_str(), number);
}

void WEB_STORE_TEXT() {
  std::string symbol = WEB_SYMBOL.str_rep();
  const char* text = WEB_TEXT.str_rep().c_str();

  EM_ASM({
    const symbol = UTF8ToString($0).split('.');
    const obj = symbol.slice(0, -1).reduce((obj, key) => obj[key], window);
    obj[symbol.slice(-1)[0]] = UTF8ToString($1);
  }, symbol.c_str(), text);
}
#endif /* __EMSCRIPTEN__ */
