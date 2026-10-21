#include "Preferences.h"
#include <map>
#include <string>

static std::map<std::string, int> enterosGuardados;
static std::map<std::string, bool> boolsGuardados;

void Preferences::begin(const char*, bool) {}

void Preferences::end() {}

bool Preferences::isKey(const char* key) {
  return boolsGuardados.find(key) != boolsGuardados.end()
      || enterosGuardados.find(key) != enterosGuardados.end();
}

int Preferences::getInt(const char* key, int valorPorDefecto) {
  auto it = enterosGuardados.find(key);
  return it != enterosGuardados.end() ? it->second : valorPorDefecto;
}

bool Preferences::putInt(const char* key, int valor) {
  enterosGuardados[key] = valor;
  return true;
}

bool Preferences::getBool(const char* key, bool valorPorDefecto) {
  auto it = boolsGuardados.find(key);
  return it != boolsGuardados.end() ? it->second : valorPorDefecto;
}

bool Preferences::putBool(const char* key, bool valor) {
  boolsGuardados[key] = valor;
  return true;
}
